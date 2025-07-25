/*##############################################################################

    HPCC SYSTEMS software Copyright (C) 2012 HPCC Systems®.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
############################################################################## */

#include <string>

#ifndef _WIN32
#include <sys/types.h>
#include <dirent.h>
#endif

#include <stdio.h>
#include <time.h>

#include "jcontainerized.hpp"
#include "jexcept.hpp"
#include "jfile.hpp"
#include "jmisc.hpp"
#include "jsocket.hpp"
#include "jmutex.hpp"

#include "jhtree.hpp"

#include "commonext.hpp"
#include "dadfs.hpp"
#include "dasds.hpp"
#include "dafdesc.hpp"

#include "thor.hpp"
#include "thorport.hpp"
#include "thormisc.hpp"
#include "thgraph.hpp"
#include "thbufdef.hpp"
#include "thmem.hpp"
#include "thcompressutil.hpp"

#include "eclrtl.hpp"
#include "eclhelper.hpp"
#include "eclrtl_imp.hpp"
#include "rtlread_imp.hpp"
#include "rtlfield.hpp"
#include "rtlrecord.hpp"
#include "rtlds_imp.hpp"
#include "rtlformat.hpp"
#include "rmtfile.hpp"
#include "roxiestream.hpp"
#include "hpccconfig.hpp"


#define SDS_LOCK_TIMEOUT 30000

static Owned<INode> masterNode;
static Owned<IGroup> processGroup; // group of slave processes
static Owned<IGroup> nodeGroup;    // master + processGroup
static Owned<IGroup> slaveGroup;   // group containing all channels
static Owned<IGroup> clusterGroup; // master + slaveGroup
static Owned<IGroup> dfsGroup;     // same as slaveGroup, but without ports
static Owned<IGroup> localGroup;   // used as a placeholder in IFileDescriptors for local files (spills)
static Owned<ICommunicator> nodeComm; // communicator based on nodeGroup (master+slave processes)


mptag_t managerWorkerMpTag;
mptag_t kjServiceMpTag;
Owned<IPropertyTree> globals;
static Owned<IMPtagAllocator> ClusterMPAllocator;

// stat. mappings shared between master and slave activities
const StatisticsMapping spillStatistics({StTimeSpillElapsed, StTimeSortElapsed, StNumSpills, StSizeSpillFile, StSizePeakTempDisk});
const StatisticsMapping executeStatistics({StWhenFirstRow, StTimeElapsed, StTimeTotalExecute, StTimeLocalExecute, StTimeBlocked});
const StatisticsMapping soapcallStatistics({StTimeSoapcall, StTimeSoapcallDNS, StTimeSoapcallConnect, StNumSoapcallConnectFailures, StNumSoapcallRetries});
const StatisticsMapping basicActivityStatistics({StNumParallelExecute, StTimeLookAhead}, executeStatistics, spillStatistics);
const StatisticsMapping groupActivityStatistics({StNumGroups, StNumGroupMax}, basicActivityStatistics);
const StatisticsMapping indexReadFileStatistics({}, diskReadRemoteStatistics, jhtreeCacheStatistics);
const StatisticsMapping indexReadActivityStatistics({StNumRowsProcessed}, indexReadFileStatistics, basicActivityStatistics);
const StatisticsMapping indexWriteActivityStatistics({StPerReplicated, StNumLeafCacheAdds, StNumNodeCacheAdds, StNumBlobCacheAdds, StNumDuplicateKeys, StSizeOffsetBranches, StSizeBranchMemory, StSizeLeafMemory, StSizeLargestExpandedLeaf}, basicActivityStatistics, diskWriteRemoteStatistics);
const StatisticsMapping keyedJoinActivityStatistics({ StNumIndexAccepted, StNumPreFiltered, StNumDiskSeeks, StNumDiskAccepted, StNumDiskRejected}, basicActivityStatistics, indexReadFileStatistics);
const StatisticsMapping commonJoinActivityStatistics({StNumMatchLeftRowsMax, StNumMatchRightRowsMax, StNumMatchCandidates, StNumMatchCandidatesMax}, basicActivityStatistics);
const StatisticsMapping hashJoinActivityStatistics({StNumLeftRows, StNumRightRows}, commonJoinActivityStatistics);
const StatisticsMapping allJoinActivityStatistics({}, commonJoinActivityStatistics);
const StatisticsMapping lookupJoinActivityStatistics({StNumSmartJoinSlavesDegradedToStd, StNumSmartJoinDegradedToLocal}, commonJoinActivityStatistics);
const StatisticsMapping joinActivityStatistics({StNumLeftRows, StNumRightRows}, commonJoinActivityStatistics);
const StatisticsMapping diskReadActivityStatistics({StNumDiskRowsRead, }, basicActivityStatistics, diskReadRemoteStatistics);
const StatisticsMapping diskWriteActivityStatistics({StPerReplicated}, basicActivityStatistics, diskWriteRemoteStatistics);
const StatisticsMapping sortActivityStatistics({}, basicActivityStatistics);
const StatisticsMapping diskReadPartStatistics({StNumDiskRowsRead}, diskReadRemoteStatistics);
const StatisticsMapping soapcallActivityStatistics({}, basicActivityStatistics, soapcallStatistics);
const StatisticsMapping hashDistribActivityStatistics({StNumLocalRows, StNumRemoteRows, StSizeRemoteWrite}, basicActivityStatistics);
const StatisticsMapping hashDedupActivityStatistics({}, hashDistribActivityStatistics, diskWriteRemoteStatistics);
const StatisticsMapping indexDistribActivityStatistics({}, hashDistribActivityStatistics, jhtreeCacheStatistics);
const StatisticsMapping loopActivityStatistics({StNumIterations}, basicActivityStatistics);
const StatisticsMapping graphStatistics({StNumExecutions, StSizeSpillFile, StSizeGraphSpill, StSizePeakTempDisk, StSizePeakEphemeralDisk, StTimeUser, StTimeSystem, StNumContextSwitches, StSizeMemory, StSizePeakMemory, StSizeRowMemory, StSizePeakRowMemory}, executeStatistics);
const StatisticsMapping tempFileStatistics({StNumSpills}, diskRemoteStatistics);

const StatKindMap diskToTempStatsMap
={ {StSizeDiskWrite, StSizeSpillFile},
   {StTimeDiskWriteIO, StTimeSpillElapsed}
 };

MODULE_INIT(INIT_PRIORITY_STANDARD)
{
    ClusterMPAllocator.setown(createMPtagRangeAllocator(MPTAG_THORGLOBAL_BASE,MPTAG_THORGLOBAL_COUNT));
    return true;
}

MODULE_EXIT()
{
    masterNode.clear();
    nodeGroup.clear();
    processGroup.clear();
    clusterGroup.clear();
    slaveGroup.clear();
    dfsGroup.clear();
    localGroup.clear();
    nodeComm.clear();
    ClusterMPAllocator.clear();
}


#define EXTRAS 1024
#define NL 3
StringBuffer &ActPrintLogArgsPrep(StringBuffer &res, const CGraphElementBase *container, const ActLogEnum flags, const char *format, va_list args)
{
    if (format)
        res.valist_appendf(format, args).append(" - ");
    res.appendf("activity(ch=%d, %s, %" ACTPF "d)", container->queryOwner().queryJobChannelNumber(), activityKindStr(container->getKind()), container->queryId());
    if (0 != (flags & thorlog_ecl))
    {
        StringBuffer ecltext;
        container->getEclText(ecltext);
        ecltext.trim();
        if (ecltext.length() > 0)
            res.append(" [ecl=").append(ecltext.str()).append(']');
    }
#ifdef _WIN32
#ifdef MEMLOG
    MEMORYSTATUS mS;
    GlobalMemoryStatus(&mS);
    res.appendf(", mem=%ld",mS.dwAvailPhys);
#endif
#endif
    return res;
}

void ActPrintLogArgs(const CGraphElementBase *container, const ActLogEnum flags, const LogMsgCategory &logCat, const char *format, va_list args)
{
    if ((0 == (flags & thorlog_all)) && !container->doLogging())
        return; // suppress logging child activities unless thorlog_all flag
    StringBuffer res;
    ActPrintLogArgsPrep(res, container, flags, format, args);
    LOG(logCat, "%s", res.str());
}

void ActPrintLogArgs(const CGraphElementBase *container, IException *e, const ActLogEnum flags, const LogMsgCategory &logCat, const char *format, va_list args)
{
    StringBuffer res;
    ActPrintLogArgsPrep(res, container, flags, format, args);
    if (e)
    {
        res.append(" : ");
        e->errorMessage(res);
    }
    LOG(logCat, "%s", res.str());
}

void ActPrintLogEx(const CGraphElementBase *container, const ActLogEnum flags, const LogMsgCategory &logCat, const char *format, ...)
{
    if ((0 == (flags & thorlog_all)) && (NULL != container->queryOwner().queryOwner() && !container->queryOwner().isGlobal()))
        return; // suppress logging child activities unless thorlog_all flag
    StringBuffer res;
    va_list args;
    va_start(args, format);
    ActPrintLogArgsPrep(res, container, flags, format, args);
    va_end(args);
    LOG(logCat, "%s", res.str());
}

void ActPrintLog(const CActivityBase *activity, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    ActPrintLogArgs(&activity->queryContainer(), thorlog_null, MCdebugProgress, format, args);
    va_end(args);
}

void ActPrintLog(const CActivityBase *activity, unsigned traceLevel, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    ActPrintLogArgs(&activity->queryContainer(), thorlog_null, MCdebugInfo(traceLevel), format, args);
    va_end(args);
}

void ActPrintLog(const CActivityBase *activity, IException *e, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    ActPrintLogArgs(&activity->queryContainer(), e, thorlog_null, MCexception(e, MSGCLS_error), format, args);
    va_end(args);
}

void ActPrintLog(const CActivityBase *activity, IException *e)
{
    ActPrintLog(activity, e, "%s", "");
}

void GraphPrintLogArgsPrep(StringBuffer &res, CGraphBase *graph, const ActLogEnum flags, const LogMsgCategory &logCat, const char *format, va_list args)
{
    if (format)
        res.valist_appendf(format, args).append(" - ");
    res.appendf("graph(%s, %" GIDPF "d)", graph->queryJob().queryGraphName(), graph->queryGraphId());
}

void GraphPrintLogArgs(CGraphBase *graph, const ActLogEnum flags, const LogMsgCategory &logCat, const char *format, va_list args)
{
    if ((0 == (flags & thorlog_all)) && (NULL != graph->queryOwner() && !graph->isGlobal()))
        return; // suppress logging from child graph unless thorlog_all flag
    StringBuffer res;
    GraphPrintLogArgsPrep(res, graph, flags, logCat, format, args);
    LOG(logCat, "%s", res.str());
}

void GraphPrintLogArgs(CGraphBase *graph, IException *e, const ActLogEnum flags, const LogMsgCategory &logCat, const char *format, va_list args)
{
    if ((0 == (flags & thorlog_all)) && (NULL != graph->queryOwner() && !graph->isGlobal()))
        return; // suppress logging from child graph unless thorlog_all flag
    StringBuffer res;
    GraphPrintLogArgsPrep(res, graph, flags, logCat, format, args);
    if (e)
    {
        res.append(" : ");
        e->errorMessage(res);
    }
    LOG(logCat, "%s", res.str());
}

void GraphPrintLog(CGraphBase *graph, IException *e, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    GraphPrintLogArgs(graph, e, thorlog_null, MCexception(e, MSGCLS_error), format, args);
    va_end(args);
}


class DECL_EXCEPTION CThorException : public CSimpleInterface, implements IThorException
{
protected:
    ThorExceptionAction action;
    ThorActivityKind kind;
    activity_id id;
    graph_id graphId;
    StringAttr jobId;
    int errorcode;
    StringAttr msg;
    LogMsgAudience audience;
    unsigned slave;
    MemoryBuffer data; // extra exception specific data
    unsigned line, column;
    StringAttr file, origin, graphName;
    ErrorSeverity severity;
    Linked<IException> originalException;
public:
    IMPLEMENT_IINTERFACE_USING(CSimpleInterface);
    CThorException(LogMsgAudience _audience,int code, const char *str) 
        : audience(_audience), errorcode(code), msg(str), action(tea_null), graphId(0), id(0), slave(0), line(0), column(0), severity(SeverityInformation), kind(TAKnone) { };
    CThorException(MemoryBuffer &mb)
    {
        readUnderlyingType<ThorExceptionAction>(mb, action);
        mb.read(jobId);
        mb.read(graphName);
        mb.read(graphId);
        readUnderlyingType(mb, kind);
        mb.read(id);
        mb.read(slave);
        readUnderlyingType(mb, audience);
        mb.read(errorcode);
        mb.read(msg);
        mb.read(file);
        mb.read(line);
        mb.read(column);
        readUnderlyingType(mb, severity);
        mb.read(origin);
        if (0 == origin.length()) // simpler to clear serialized 0 length terminated string here than check on query
            origin.clear();
        bool oe;
        mb.read(oe);
        if (oe)
            originalException.setown(deserializeThorException(mb));
        size32_t sz;
        mb.read(sz);
        if (sz)
            data.append(sz, mb.readDirect(sz));
    }

// IThorException
    virtual ThorExceptionAction queryAction() const { return action; }
    virtual ThorActivityKind queryActivityKind() const { return kind; }
    virtual activity_id queryActivityId() const { return id; }
    virtual const char *queryGraphName() const { return graphName; }
    virtual graph_id queryGraphId() const { return graphId; }
    virtual const char *queryJobId() const { return jobId; }
    virtual unsigned querySlave() const { return slave; }
    virtual void getAssert(StringAttr &_file, unsigned &_line, unsigned &_column) const { _file.set(file); _line = line; _column = column; }
    virtual const char *queryOrigin() const { return origin; }
    virtual const char *queryMessage() const { return msg; }
    virtual ErrorSeverity querySeverity() const { return severity; }
    virtual MemoryBuffer &queryData() { return data; }
    virtual IException *queryOriginalException() const { return originalException; }
    virtual void setActivityId(activity_id _id) { id = _id; }
    virtual void setActivityKind(ThorActivityKind _kind) { kind = _kind; }
    virtual void setGraphInfo(const char *_graphName, graph_id _graphId) { graphName.set(_graphName); graphId = _graphId; }
    virtual void setJobId(const char *_jobId) { jobId.set(_jobId); }
    virtual void setAction(ThorExceptionAction _action) { action = _action; }
    virtual void setAudience(MessageAudience _audience) { audience = _audience; }
    virtual void setSlave(unsigned _slave) { slave = _slave; }
    virtual void setMessage(const char *_msg) { msg.set(_msg); }
    virtual void setAssert(const char *_file, unsigned _line, unsigned _column) { file.set(_file); line = _line; column = _column; }
    virtual void setOrigin(const char *_origin) { origin.set(_origin); }
    virtual void setSeverity(ErrorSeverity _severity) { severity = _severity; }
    virtual void setOriginalException(IException *e) { originalException.set(e); }

// IException
    int errorCode() const { return errorcode; }
    StringBuffer &errorMessage(StringBuffer &str) const
    {
        if (!origin.length() || 0 != stricmp("user", origin.get())) // don't report slave in user message
        {
            if (graphName.length())
            {
                str.append("Graph ").append(graphName);
                if (graphId)
                    str.append("[").append(graphId).append("]");
                str.append(", ");
            }
            if (kind)
                str.append(activityKindStr(kind));
            if (id)
            {
                if (kind) str.append('[');
                str.append(id);
                if (kind) str.append(']');
                str.append(": ");
            }
            if (slave)
            {
                if (((int)slave) < 0)
                {
                    unsigned node = (unsigned)-slave;
                    str.appendf("NODE #%d [", node);
                    queryNodeGroup().queryNode(node).endpoint().getEndpointHostText(str);
                    str.append("]: ");
                }
                else
                {
                    str.appendf("WORKER #%d [", slave);
                    queryClusterGroup().queryNode(slave).endpoint().getEndpointHostText(str);
                    str.append("]: ");
                }
            }
        }
        str.append(msg);
        if (originalException)
        {
            if (msg.length())
                str.append(" - ");
            str.append("caused by (");
            str.append(originalException->errorCode());
            str.append(", ");
            originalException->errorMessage(str);
            str.append(")");
        }
        return str;
    }
    MessageAudience errorAudience() const { return audience; }
};

CThorException *_MakeThorException(LogMsgAudience audience,int code, const char *format, va_list args) __attribute__((format(printf,3,0)));
CThorException *_MakeThorException(LogMsgAudience audience, int code, const char *format, va_list args)
{
    StringBuffer eStr;
    eStr.limited_valist_appendf(1024, format, args);
    return new CThorException(audience, code, eStr.str());
}

CThorException *_ThorWrapException(IException *e, const char *format, va_list args) __attribute__((format(printf,2,0)));
CThorException *_ThorWrapException(IException *e, const char *format, va_list args)
{
    StringBuffer eStr;
    eStr.appendf("%d, ", e->errorCode());
    e->errorMessage(eStr).append(" : ");
    eStr.limited_valist_appendf(2048, format, args);
    CThorException *te = new CThorException(e->errorAudience(), e->errorCode(), eStr.str());
    if (QUERYINTERFACE(e, IMP_Exception))
    {
        IMP_Exception *me = QUERYINTERFACE(e, IMP_Exception);
        unsigned workerNum = queryNodeComm().queryGroup().rank(me->queryEndpoint());
        te->setSlave(workerNum);
    }
    return te;
}

// convert exception (if necessary) to an exception with action=shutdown
IThorException *MakeThorFatal(IException *e, int code, const char *format, ...)
{
    CThorException *te = QUERYINTERFACE(e, CThorException);
    if (te)
        te->Link();
    else
    {
        va_list args;
        va_start(args, format);
        if (e) te = _ThorWrapException(e, format, args);
        else te = _MakeThorException(MSGAUD_user,code, format, args);
        va_end(args);
    }
    te->setAction(tea_shutdown);
    return te;
}

IThorException *MakeThorAudienceException(LogMsgAudience audience, int code, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    IThorException *e = _MakeThorException(audience, code, format, args);
    va_end(args);
    return e;
}

IThorException *MakeThorOperatorException(int code, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    IThorException *e = _MakeThorException(MSGAUD_operator,code, format, args);
    va_end(args);
    return e;
}

void setExceptionActivityInfo(CGraphElementBase &container, IThorException *e)
{
    e->setActivityKind(container.getKind());
    e->setActivityId(container.queryId());
    e->setGraphInfo(container.queryJob().queryGraphName(), container.queryOwner().queryGraphId());
}

IThorException *_MakeActivityException(CGraphElementBase &container, int code, const char *format, va_list args) __attribute__((format(printf,3,0)));
IThorException *_MakeActivityException(CGraphElementBase &container, int code, const char *format, va_list args)
{
    IThorException *e = _MakeThorException(MSGAUD_user, code, format, args);
    setExceptionActivityInfo(container, e);
    return e;
}

IThorException *_MakeActivityException(CGraphElementBase &container, IException *e, const char *_format, va_list args) __attribute__((format(printf,3,0)));
IThorException *_MakeActivityException(CGraphElementBase &container, IException *e, const char *_format, va_list args)
{
    StringBuffer msg;
    e->errorMessage(msg);
    if (_format)
        msg.append(", ").limited_valist_appendf(1024, _format, args);
    IThorException *e2 = new CThorException(e->errorAudience(), e->errorCode(), msg.str());
    e2->setOriginalException(e);
    setExceptionActivityInfo(container, e2);
    if (QUERYINTERFACE(e, IMP_Exception))
    {
        IMP_Exception *me = QUERYINTERFACE(e, IMP_Exception);
        unsigned workerNum = queryNodeComm().queryGroup().rank(me->queryEndpoint());
        e2->setSlave(workerNum);
    }
    return e2;
}

IThorException *MakeActivityException(CActivityBase *activity, int code, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    IThorException *e = _MakeActivityException(activity->queryContainer(), code, format, args);
    va_end(args);
    return e;
}

IThorException *MakeActivityException(CActivityBase *activity, IException *e, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    IThorException *e2 = _MakeActivityException(activity->queryContainer(), e, format, args);
    va_end(args);
    return e2;
}

IThorException *MakeActivityException(CActivityBase *activity, IException *e)
{
    return MakeActivityException(activity, e, "%s", "");
}

IThorException *MakeActivityWarning(CActivityBase *activity, int code, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    IThorException *e = _MakeActivityException(activity->queryContainer(), code, format, args);
    e->setAction(tea_warning);
    e->setSeverity(SeverityWarning);
    va_end(args);
    return e;
}

IThorException *MakeActivityWarning(CActivityBase *activity, IException *e, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    IThorException *e2 = _MakeActivityException(activity->queryContainer(), e, format, args);
    e2->setAction(tea_warning);
    e2->setSeverity(SeverityWarning);
    va_end(args);
    return e2;
}

IThorException *MakeActivityException(CGraphElementBase *container, int code, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    IThorException *e = _MakeActivityException(*container, code, format, args);
    va_end(args);
    return e;
}

IThorException *MakeActivityException(CGraphElementBase *container, IException *e, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    IThorException *e2 = _MakeActivityException(*container, e, format, args);
    va_end(args);
    return e2;
}

IThorException *MakeActivityException(CGraphElementBase *container, IException *e)
{
    return MakeActivityException(container, e, "%s", "");
}

IThorException *MakeActivityWarning(CGraphElementBase *container, int code, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    IThorException *e = _MakeActivityException(*container, code, format, args);
    e->setAction(tea_warning);
    e->setSeverity(SeverityWarning);
    va_end(args);
    return e;
}

IThorException *MakeActivityWarning(CGraphElementBase *container, IException *e, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    IThorException *e2 = _MakeActivityException(*container, e, format, args);
    e2->setAction(tea_warning);
    e2->setSeverity(SeverityWarning);
    va_end(args);
    return e2;
}

IThorException *MakeThorException(int code, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    IThorException *e2 = _MakeThorException(MSGAUD_user,code, format, args);
    va_end(args);
    return e2;
}

IThorException *MakeThorException(IException *e)
{
    IThorException *te = QUERYINTERFACE(e, IThorException);
    if (te)
        return LINK(te);
    StringBuffer msg;
    return new CThorException(MSGAUD_user, e->errorCode(), e->errorMessage(msg).str());
}

IThorException *ThorWrapException(IException *e, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    ThorExceptionAction action=tea_null;
    if (QUERYINTERFACE(e, ISEH_Exception))
        action = tea_shutdown;
    CThorException *te = _ThorWrapException(e, format, args);
    te->setAction(action);
    va_end(args);
    return te;
}

IThorException *MakeGraphException(CGraphBase *graph, int code, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    IThorException *e = _MakeThorException(MSGAUD_user, code, format, args);
    e->setGraphInfo(graph->queryJob().queryGraphName(), graph->queryGraphId());
    va_end(args);
    return e;
}

IThorException *MakeGraphException(CGraphBase *graph, IException *e)
{
    StringBuffer msg;
    IThorException *e2 = new CThorException(MSGAUD_user, e->errorCode(), e->errorMessage(msg).str());
    e2->setGraphInfo(graph->queryJob().queryGraphName(), graph->queryGraphId());
    return e2;
}


class CTempNameHandler
{
public:
    unsigned num;
    StringBuffer rootDir, subDirName, prefix, subDirPath;
    CriticalSection crit;

    CTempNameHandler()
    {
        num = 0;
    }
    const char *queryTempDir() 
    { 
        return subDirPath; 
    }
    void clearTempDirectory(bool log)
    {
        assertex(subDirPath.length());
        Owned<IDirectoryIterator> iter = createDirectoryIterator(subDirPath);
        ForEach (*iter)
        {
            IFile &file = iter->query();
            if (file.isFile()==fileBool::foundYes)
            {
                if (log)
                    DBGLOG("Deleting %s", file.queryFilename());
                try { file.remove(); }
                catch (IException *e)
                {
                    if (log)
                        FLLOG(MCwarning, e);
                    e->Release();
                }
            }
        }
    }
    void setTempDir(const char *_rootDir, const char *_subDirName, const char *_prefix, bool clearDir)
    {
        assertex(!isEmptyString(_rootDir) && !isEmptyString(_prefix) && !isEmptyString(_subDirName));
        CriticalBlock block(crit);
        rootDir.set(_rootDir);
        addPathSepChar(rootDir);
        subDirName.set(_subDirName);
        prefix.set(_prefix);
        // NB: subDirPath will be empty, unless there was a problem during the job ctor. Either way ok to clear/set.
        subDirPath.setf("%s%s", rootDir.str(), subDirName.str());
        bool ret = recursiveCreateDirectory(subDirPath);
        VStringBuffer msg("%s to create temp directory %s", ret ? "Succeeded" : "Failed", subDirPath.str());
        DBGLOG("%s", msg.str());
        if (!ret)
        {
            // temp dir. should not exist, but if it does issue warning only.
            if (checkDirExists(subDirPath))
                IWARNLOG("Existing temp directory %s already exists", subDirPath.str());
            else
                throw MakeThorException(0, "%s", msg.str());
        }
        if (clearDir)
            clearTempDirectory(true);
#ifdef _CONTAINERIZED
        // setTempDir is called exactly once in containerized mode
        // Output the unique directory used by this manager/worker to a file,
        // so the postStop command can find it and ensure cleared up.
        Owned<IFile> nameTmpDir = createIFile("tmpdir"); // NB: each pod is in it's own private working directory
        Owned<IFileIO> nameTmpDirIO = nameTmpDir->open(IFOcreate);
        if (!nameTmpDirIO)
            throw makeStringException(0, "Failed to create file 'tmpdir' with content of temp directory name");
        nameTmpDirIO->write(0, subDirPath.length(), subDirPath.str());
#endif
    }
    void clear(bool log)
    {
        clearTempDirectory(log);
        try
        {
            Owned<IFile> dirIFile = createIFile(subDirPath);
            if (!dirIFile->remove() && log)
                IWARNLOG("Failed to delete temp directory: %s", subDirPath.str());
        }
        catch (IException *e)
        {
            if (log)
                FLLOG(MCwarning, e);
            e->Release();
        }
        subDirPath.clear();
    }
    void getTempName(StringBuffer &name, const char *suffix, bool inTempDir)
    {
        CriticalBlock block(crit);
        assertex(!subDirPath.isEmpty());
        if (inTempDir)
        {
            name.append(rootDir);
            name.append(subDirName);
            addPathSepChar(name);
        }
        else
            name.append(subDirName).append('_');
        name.append(prefix).append('_').append(++num);
        if (suffix)
            name.append("__").append(suffix);
        name.append(".tmp");
    }
} TempNameHandler;



void GetTempFileName(StringBuffer &name, const char *suffix)
{
    TempNameHandler.getTempName(name, suffix, false);
}

void GetTempFilePath(StringBuffer &name, const char *suffix)
{
    TempNameHandler.getTempName(name, suffix, true);
}

void SetTempDir(const char *rootTempDir, const char *uniqueSubDir, const char *tempPrefix, bool clearDir)
{
    TempNameHandler.setTempDir(rootTempDir, uniqueSubDir, tempPrefix, clearDir);
    DBGLOG("temporary rootTempdir: %s, uniqueSubDir: %s, prefix: %s", rootTempDir, uniqueSubDir, tempPrefix);
}

void ClearTempDir()
{
    try
    {
        TempNameHandler.clear(true);
        LOG(MCthorDetailedDebugInfo, "temp directory cleared");
    }
    catch (IException *e)
    {
        IERRLOG(e, "ClearTempDir");
        e->Release();
    }
}

const char *queryTempDir()
{
    return TempNameHandler.queryTempDir();
}

class DECL_EXCEPTION CBarrierAbortException: public CSimpleInterface, public IBarrierException
{
public:
    IMPLEMENT_IINTERFACE_USING(CSimpleInterface);
// IThorException
    int errorCode() const { return -1; }
    StringBuffer &errorMessage(StringBuffer &str) const { str.append("Barrier Aborted"); return str; }
    MessageAudience errorAudience() const { return MSGAUD_user; }
};

IBarrierException *createBarrierAbortException()
{
    return new CBarrierAbortException();
}

void loadCmdProp(IPropertyTree *tree, const char *cmdProp)
{
    StringBuffer prop("@"), val;
    while (*cmdProp && *cmdProp != '=')
        prop.append(*cmdProp++);
    if (*cmdProp)
    {
        cmdProp++;
        while (isspace(*cmdProp))
            cmdProp++;
        while (*cmdProp)
            val.append(*cmdProp++);
        prop.clip();
        val.clip();
        if (prop.length())
            tree->setProp(prop.str(), val.str());
    }
}

void ensureDirectoryForFile(const char *fName)
{
    if (!recursiveCreateDirectoryForFile(fName))
        throw makeOsExceptionV(GetLastError(), "Failed to create directory for file: %s", fName);
}

// Not recommended to be used from slaves as tend to be one or more trying at same time.
void reportExceptionToWorkunit(IConstWorkUnit &workunit,IException *e, ErrorSeverity severity)
{
    LOG(MCprogress, e, "Reporting exception to WU");
    Owned<IWorkUnit> wu = &workunit.lock();
    if (wu)
    {
        Owned<IWUException> we = wu->createException();
        StringBuffer s;
        we->setExceptionMessage(e->errorMessage(s.clear()).str());
        we->setExceptionCode(e->errorCode());
        IThorException *te = QUERYINTERFACE(e, IThorException);
        if (te)
        {
            we->setSeverity(te->querySeverity());
            if (!te->queryOrigin()) // will have an origin if from slaves already
                te->setOrigin("master");
            we->setExceptionSource(te->queryOrigin());
            StringAttr file;
            unsigned line, column;
            te->getAssert(file, line, column);
            if (file.length())
                we->setExceptionFileName(file);
            if (line || column)
            {
                we->setExceptionLineNo(line);
                we->setExceptionColumn(column);
            }
            if (te->queryActivityId())
                we->setActivityId(te->queryActivityId());
        }
        else
            we->setSeverity(severity);
    }
}

void reportExceptionToWorkunitCheckIgnore(IConstWorkUnit &workunit, IException *e, ErrorSeverity severity)
{
    ErrorSeverity mappedSeverity = workunit.getWarningSeverity(e->errorCode(), severity);
    if (SeverityIgnore == mappedSeverity)
        return;
    reportExceptionToWorkunit(workunit, e, mappedSeverity);
}

StringBuffer &getCompoundQueryName(StringBuffer &compoundName, const char *queryName, unsigned version)
{
    return compoundName.append('V').append(version).append('_').append(queryName);
}

void setupGroups(INode *_masterNode, IGroup *_processGroup, IGroup *_slaveGroup)
{
    masterNode.set(_masterNode);
    processGroup.set(_processGroup);
    slaveGroup.set(_slaveGroup);

    // nodeGroup contains master + all slave processes (excludes virtual slaves)
    nodeGroup.setown(processGroup->add(LINK(masterNode), 0));

    // clusterGroup contains master + all slaves (including virtuals)
    clusterGroup.setown(slaveGroup->add(LINK(masterNode), 0));

    // dfsGroup is same as slaveGroup, but stripped of ports. So is a IP group as wide as slaveGroup, used for publishing
    IArrayOf<INode> dfsGroupNodes;
    Owned<INodeIterator> nodeIter = slaveGroup->getIterator();
    ForEach(*nodeIter)
        dfsGroupNodes.append(*createINodeIP(nodeIter->query().endpoint(), 0));
    dfsGroup.setown(createIGroup(dfsGroupNodes.ordinality(), dfsGroupNodes.getArray()));

    Owned<INode> localNode = createINode("localhost");
    INode *p = localNode;
    localGroup.setown(createIGroup(1, &p));

    nodeComm.setown(createCommunicator(nodeGroup));
}
    
void setupCluster(INode *_masterNode, IGroup *_processGroup, unsigned channelsPerSlave, unsigned portBase, unsigned portInc)
{
    IArrayOf<INode> slaveGroupNodes;
    for (unsigned s=0; s<channelsPerSlave; s++)
    {
        for (unsigned p=0; p<_processGroup->ordinality(); p++)
        {
            INode &processNode = _processGroup->queryNode(p);
            SocketEndpoint ep = processNode.endpoint();
            ep.port = ep.port + (s * portInc);
            Owned<INode> node = createINode(ep);
            slaveGroupNodes.append(*node.getClear());
        }
    }
    Owned<IGroup> _slaveGroup = createIGroup(slaveGroupNodes.ordinality(), slaveGroupNodes.getArray());
    setupGroups(_masterNode, _processGroup, _slaveGroup);
}

void setClusterGroup(INode *_masterNode, IGroup *rawGroup, unsigned slavesPerNode, unsigned channelsPerSlave, unsigned portBase, unsigned portInc)
{
    SocketEndpointArray epa;
    OwnedMalloc<unsigned> hostStartPort, hostNextStartPort;
    hostStartPort.allocateN(rawGroup->ordinality());
    hostNextStartPort.allocateN(rawGroup->ordinality());
    for (unsigned n=0; n<rawGroup->ordinality(); n++)
    {
        SocketEndpoint ep = rawGroup->queryNode(n).endpoint();
        unsigned hostPos = epa.find(ep);
        if (NotFound == hostPos)
        {
            hostPos = epa.ordinality();
            epa.append(ep);
            hostStartPort[n] = portBase;
            hostNextStartPort[hostPos] = portBase + (slavesPerNode * channelsPerSlave * portInc);
        }
        else
        {
            hostStartPort[n] = hostNextStartPort[hostPos];
            hostNextStartPort[hostPos] += (slavesPerNode * channelsPerSlave * portInc);
        }
    }
    IArrayOf<INode> slaveGroupNodes, processGroupNodes;
    for (unsigned s=0; s<channelsPerSlave; s++)
    {
        for (unsigned p=0; p<slavesPerNode; p++)
        {
            for (unsigned n=0; n<rawGroup->ordinality(); n++)
            {
                SocketEndpoint ep = rawGroup->queryNode(n).endpoint();
                ep.port = hostStartPort[n] + (((p * channelsPerSlave) + s) * portInc);
                Owned<INode> node = createINode(ep);
                slaveGroupNodes.append(*node.getLink());
                if (0 == s)
                    processGroupNodes.append(*node.getLink());
            }
        }
    }
    Owned<IGroup> _processGroup = createIGroup(processGroupNodes.ordinality(), processGroupNodes.getArray());
    Owned<IGroup> _slaveGroup = createIGroup(slaveGroupNodes.ordinality(), slaveGroupNodes.getArray());
    setupGroups(_masterNode, _processGroup, _slaveGroup);
}

bool clusterInitialized() { return NULL != nodeComm; }
INode &queryMasterNode() { return *masterNode; }
ICommunicator &queryNodeComm() { return *nodeComm; }
IGroup &queryNodeGroup() { return *nodeGroup; }
IGroup &queryProcessGroup() { return *processGroup; }
IGroup &queryClusterGroup() { return *clusterGroup; }
IGroup &querySlaveGroup() { return *slaveGroup; }
IGroup &queryDfsGroup() { return *dfsGroup; }
IGroup &queryLocalGroup() { return *localGroup; }
unsigned queryClusterWidth() { return clusterGroup->ordinality()-1; }
unsigned queryNodeClusterWidth() { return nodeGroup->ordinality()-1; }


mptag_t allocateClusterMPTag()
{
    return ClusterMPAllocator->alloc();
}

void freeClusterMPTag(mptag_t tag)
{
    ClusterMPAllocator->release(tag);
}

IThorException *deserializeThorException(MemoryBuffer &in)
{
    unsigned te;
    in.read(te);
    if (!te)
    {
        Owned<IException> e = deserializeException(in);
        StringBuffer s;
        return new CThorException(e->errorAudience(), e->errorCode(), e->errorMessage(s).str());
    }
    return new CThorException(in);
}

void serializeThorException(IException *e, MemoryBuffer &out)
{
    IThorException *te = QUERYINTERFACE(e, IThorException);
    if (!te)
    {
        out.append(0);
        serializeException(e, out);
        return;
    }
    out.append(1);
    out.append((unsigned)te->queryAction());
    out.append(te->queryJobId());
    out.append(te->queryGraphName());
    out.append(te->queryGraphId());
    out.append((unsigned)te->queryActivityKind());
    out.append(te->queryActivityId());
    out.append(te->querySlave());
    out.append((unsigned)te->errorAudience());
    out.append(te->errorCode());
    out.append(te->queryMessage());
    StringAttr file;
    unsigned line, column;
    te->getAssert(file, line, column);
    out.append(file);
    out.append(line);
    out.append(column);
    out.append((unsigned)te->querySeverity());
    out.append(te->queryOrigin());
    IException *oe = te->queryOriginalException();
    if (oe)
    {
        out.append(true);
        serializeThorException(oe, out);
    }
    else
        out.append(false);
    MemoryBuffer &data = te->queryData();
    out.append((size32_t)data.length());
    if (data.length())
        out.append(data.length(), data.toByteArray());
}

bool getBestFilePart(CActivityBase *activity, IPartDescriptor &partDesc, OwnedIFile & ifile, unsigned &location, StringBuffer &path, IExceptionHandler *eHandler)
{
    if (0 == partDesc.numCopies()) // not sure this is poss.
        return false;
    SocketEndpoint slfEp((unsigned short)0);
    unsigned l;

    RemoteFilename rfn;
    StringBuffer locationName, primaryName;
    //First check for local matches
    unsigned copies = partDesc.numCopies();
    unsigned localCopies = 0;
    Owned<IException> accessException;
    IMultiException *multiException = nullptr;
    for (l=0; l<copies; l++)
    {
        rfn.clear();
        partDesc.getFilename(l, rfn);
        if (0 == l)
        {
            rfn.getPath(locationName.clear());
            assertex(locationName.length());
            primaryName.append(locationName);
            locationName.clear();
        }
        if (rfn.isLocal())
        {
            localCopies++;
            rfn.getPath(locationName.clear());
            assertex(locationName.length());

            Owned<IFile> file;
            if (activity->getOptBool("forceDafilesrv"))
            {
                DBGLOG("Using dafilesrv for: %s", locationName.str());
                file.setown(createDaliServixFile(rfn));
            }
            else
                file.setown(createIFile(rfn)); // use rfn not locationName, to preserve port through hooking mechanisms
            try
            {
                if (file->exists())
                {
                    ifile.set(file);
                    location = l;
                    path.append(locationName);
                    return true;
                }
            }
            catch (IException *e)
            {
                ActPrintLog(&activity->queryContainer(), e, "getBestFilePart");
                if (!accessException)
                    accessException.setown(e);
                else
                {
                    if (!multiException)
                    {
                        multiException = makeMultiException();
                        multiException->append(*accessException.getClear());
                        accessException.setown(multiException);
                    }
                    multiException->append(*e);
                }
            }
        }
    }

    if (localCopies < copies)
    {
        //Now check for a remote match...
        for (l=0; l<partDesc.numCopies(); l++)
        {
            rfn.clear();
            partDesc.getFilename(l, rfn);
            if (!rfn.isLocal())
            {
                rfn.getPath(locationName.clear());
                assertex(locationName.length());
                Owned<IFile> file = createIFile(locationName.str());
                try
                {
                    if (file->exists())
                    {
                        ifile.set(file);
                        location = l;
                        if (0 != l)
                        {
                            Owned<IThorException> e = MakeActivityWarning(activity, 0, "Primary file missing: %s, using remote copy: %s", primaryName.str(), locationName.str());
                            if (!eHandler)
                                throw e.getClear();
                            eHandler->fireException(e);
                        }
                        path.append(locationName);
                        return true;
                    }
                }
                catch (IException *e)
                {
                    ActPrintLog(&activity->queryContainer(), e, "In getBestFilePart");
                    if (!accessException)
                        accessException.setown(e);
                    else
                    {
                        if (!multiException)
                        {
                            multiException = makeMultiException();
                            multiException->append(*accessException.getClear());
                            accessException.setown(multiException);
                        }
                        multiException->append(*e);
                    }
                }
            }
        }
    }
    if (accessException)
    {
        if (!eHandler)
            throw accessException.getClear();

        eHandler->fireException(accessException);
    }
    return false;
}

StringBuffer &getFilePartLocations(IPartDescriptor &partDesc, StringBuffer &locations)
{
    unsigned l;
    for (l=0; l<partDesc.numCopies(); l++)
    {
        RemoteFilename rfn;
        partDesc.getFilename(l, rfn);
        rfn.getRemotePath(locations);
        if (l != partDesc.numCopies()-1)
            locations.append(", ");
    }
    return locations;
}

StringBuffer &getPartFilename(IPartDescriptor &partDesc, unsigned copy, StringBuffer &filePath, bool localMount)
{
    RemoteFilename rfn;
    if (localMount && copy)
    {
        partDesc.getFilename(0, rfn);
        if (!rfn.isLocal())
            localMount = false;
        rfn.clear();
    }
    partDesc.getFilename(copy, rfn);
    rfn.getPath(filePath);
    return filePath;
}

// CFifoFileCache impl.

void CFifoFileCache::deleteFile(IFile &ifile)
{
    try 
    {
        if (!ifile.remove())
            FLLOG(MCoperatorWarning, "CFifoFileCache: Failed to remove file (missing) : %s", ifile.queryFilename());
    }
    catch (IException *e)
    {
        StringBuffer s("Failed to remove file: ");
        FLLOG(MCoperatorWarning, e, s.append(ifile.queryFilename()));
    }
}

void CFifoFileCache::init(const char *cacheDir, unsigned _limit, const char *pattern)
{
    limit = _limit;
    Owned<IDirectoryIterator> iter = createDirectoryIterator(cacheDir, pattern);
    ForEach (*iter)
    {
        IFile &file = iter->query();
        if (file.isFile()==fileBool::foundYes)
            deleteFile(file);
    }
}

void CFifoFileCache::add(const char *filename)
{
    unsigned pos = files.find(filename);
    if (NotFound != pos)
        files.remove(pos);
    files.add(filename, 0);
    if (files.ordinality() > limit)
    {
        const char *toRemoveFname = files.item(limit);
        DBGLOG("Removing %s from fifo cache", toRemoveFname);
        OwnedIFile ifile = createIFile(toRemoveFname);
        deleteFile(*ifile);
        files.remove(limit);
    }
}

bool CFifoFileCache::isAvailable(const char *filename)
{
    unsigned pos = files.find(filename);
    if (NotFound != pos)
    {
        OwnedIFile ifile = createIFile(filename);
        if (ifile->exists())
            return true;
    }
    return false;
}

IOutputMetaData *createFixedSizeMetaData(size32_t sz)
{
    // sure if this allowed or is cheating!
    return new CFixedOutputMetaData(sz);
}


class CRowStreamFromNode : public CSimpleInterface, implements IRowStream
{
    CActivityBase &activity;
    unsigned node, myNode;
    ICommunicator &comm;
    MemoryBuffer mb;
    bool eos;
    const bool &abortSoon;
    mptag_t mpTag, replyTag;
    Owned<IBufferedSerialInputStream> bufferStream;
    CThorStreamDeserializerSource memDeserializer;
    CMessageBuffer msg;

public:
    IMPLEMENT_IINTERFACE_USING(CSimpleInterface);

    CRowStreamFromNode(CActivityBase &_activity, unsigned _node, ICommunicator &_comm, mptag_t _mpTag, const bool &_abortSoon) : activity(_activity), node(_node), comm(_comm), mpTag(_mpTag), abortSoon(_abortSoon)
    {
        bufferStream.setown(createMemoryBufferSerialStream(mb));
        memDeserializer.setStream(bufferStream);
        myNode = comm.queryGroup().rank(activity.queryMPServer().queryMyNode());
        replyTag = activity.queryMPServer().createReplyTag();
        msg.setReplyTag(replyTag);
        eos = false;
    }
// IRowStream
    const void *nextRow()
    {
        if (eos) return NULL;

        for (;;)
        {
            while (!memDeserializer.eos()) 
            {
                RtlDynamicRowBuilder rowBuilder(activity.queryRowAllocator());
                size32_t sz = activity.queryRowDeserializer()->deserialize(rowBuilder, memDeserializer);
                return rowBuilder.finalizeRowClear(sz);
            }
            // no msg just give me data
            if (!comm.send(msg, node, mpTag, LONGTIMEOUT)) // should never timeout, unless other end down
                throw MakeStringException(0, "CRowStreamFromNode: Failed to send data request from node %d, to node %d", myNode, node);
            for (;;)
            {
                if (abortSoon)
                    break;
                if (comm.recv(msg, node, replyTag, NULL, 60000))
                    break;
                ActPrintLog(&activity, "CRowStreamFromNode, request more from node %d, tag %d timedout, retrying", node, mpTag);
            }
            if (!msg.length())
                break;
            if (abortSoon)
                break;
            msg.swapWith(mb);
            msg.clear();
        }
        eos = true;
        return NULL;
    }
    void stop()
    {
        CMessageBuffer msg;
        msg.append(1); // stop
#ifdef TRACE_GLOBAL_GROUP
        ActPrintLog(&activity, "%s - sending to %u", __func__, node);
#endif
        verifyex(comm.send(msg, node, mpTag));
    }
};

IRowStream *createRowStreamFromNode(CActivityBase &activity, unsigned node, ICommunicator &comm, mptag_t mpTag, const bool &abortSoon)
{
    return new CRowStreamFromNode(activity, node, comm, mpTag, abortSoon);
}

#define DEFAULT_ROWSERVER_BUFF_SIZE                 (0x10000)               // 64K
class CRowServer : public CSimpleInterface, implements IThreaded, implements IRowServer
{
    CThreaded threaded;
    ICommunicator &comm;
    CActivityBase *activity;
    mptag_t mpTag;
    unsigned fetchBuffSize;
    Linked<IRowStream> seq;
    std::atomic<bool> running;

public:
    IMPLEMENT_IINTERFACE_USING(CSimpleInterface);

    CRowServer(CActivityBase *_activity, IRowStream *_seq, ICommunicator &_comm, mptag_t _mpTag) 
        : activity(_activity), seq(_seq), comm(_comm), mpTag(_mpTag), threaded("CRowServer")
    {
        fetchBuffSize = DEFAULT_ROWSERVER_BUFF_SIZE;
        running = true;
        threaded.init(this, true);
    }
    ~CRowServer()
    {
#ifdef TRACE_GLOBAL_GROUP
        ActPrintLog(activity, "%s", __func__);
#endif
        stop();
        threaded.join();
    }
    virtual void threadmain() override
    {
        CMessageBuffer mb;
        while (running)
        {
            rank_t sender;
#ifdef TRACE_GLOBAL_GROUP
            ActPrintLog(activity, "%s - recv()", __func__);
#endif
            if (comm.recv(mb, RANK_ALL, mpTag, &sender))
            {
                unsigned code;
                if (mb.length())
                {
                    mb.read(code);
                    if (1 == code) // stop
                    {
#ifdef TRACE_GLOBAL_GROUP
                        ActPrintLog(activity, "%s - received stop mb.len=%u, sender=%u", __func__, mb.length(), (unsigned)sender);
#endif
                        seq->stop();
                        break;
                    }
                    else
                        throwUnexpected();
                }
                mb.clear();
                CMemoryRowSerializer mbs(mb);
                do
                {
                    OwnedConstThorRow row = seq->nextRow();
                    if (!row)
                        break;
                    activity->queryRowSerializer()->serialize(mbs,(const byte *)row.get());
                } while (mb.length() < fetchBuffSize); // NB: allows at least 1
                if (!comm.reply(mb, LONGTIMEOUT))
                    throw MakeStringException(0, "CRowStreamFromNode: Failed to send data back to node: %d", activity->queryContainer().queryJobChannel().queryMyRank());
                mb.clear();
            }
        }
        running = false;
    }
    void stop()
    {
#ifdef TRACE_GLOBAL_GROUP
        ActPrintLog(activity, "%s", __func__);
#endif
        bool wanted = true;
        if (running.compare_exchange_strong(wanted, false))
            comm.cancel(RANK_ALL, mpTag);
    }
};

IRowServer *createRowServer(CActivityBase *activity, IRowStream *seq, ICommunicator &comm, mptag_t mpTag)
{
    return new CRowServer(activity, seq, comm, mpTag);
}

IEngineRowStream *createUngroupStream(IRowStream *input)
{
    class CUngroupStream : public CSimpleInterfaceOf<IEngineRowStream>
    {
        IRowStream *input;
    public:
        CUngroupStream(IRowStream *_input) : input(_input) { input->Link(); }
        ~CUngroupStream() { input->Release(); }
        virtual const void *nextRow() override
        {
            const void *ret = input->nextRow(); 
            if (ret) 
                return ret;
            else
                return input->nextRow();
        }
        virtual void stop() override
        {
            input->stop();
        }
        virtual void resetEOF() override { throwUnexpected(); }
    };
    return new CUngroupStream(input);
}

void sendInChunks(ICommunicator &comm, rank_t dst, mptag_t mpTag, IRowStream *input, IThorRowInterfaces *rowIf)
{
    CMessageBuffer msg;
    MemoryBuffer mb;
    CMemoryRowSerializer mbs(mb);
    IOutputRowSerializer *serializer = rowIf->queryRowSerializer();
    for (;;)
    {
        for (;;)
        {
            OwnedConstThorRow row = input->nextRow();
            if (!row)
            {
                row.setown(input->nextRow());
                if (!row)
                    break;
            }
            serializer->serialize(mbs, (const byte *)row.get());
            if (mb.length() > 0x80000)
                break;
        }
        msg.clear();
        if (mb.length())
        {
            msg.append(false); // no error
            ThorCompress(mb.toByteArray(), mb.length(), msg);
            mb.clear();
        }
        comm.send(msg, dst, mpTag, LONGTIMEOUT);
        if (0 == msg.length())
            break;
    }
}

void logDiskSpace()
{
    StringBuffer diskSpaceMsg("Disk space: ");
    diskSpaceMsg.append(queryBaseDirectory(grp_unknown, 0)).append(" = ").append(getFreeSpace(queryBaseDirectory(grp_unknown, 0))/0x100000).append(" MB, ");
    diskSpaceMsg.append(queryBaseDirectory(grp_unknown, 1)).append(" = ").append(getFreeSpace(queryBaseDirectory(grp_unknown, 1))/0x100000).append(" MB, ");
    const char *tempDir = globals->queryProp("@thorTempDirectory");
    diskSpaceMsg.append(tempDir).append(" = ").append(getFreeSpace(tempDir)/0x100000).append(" MB");
    PROGLOG("%s", diskSpaceMsg.str());
}

IPerfMonHook *createThorMemStatsPerfMonHook(CJobBase &job, int maxLevel, IPerfMonHook *chain)
{
    class CPerfMonHook : public CSimpleInterfaceOf<IPerfMonHook>
    {
        CJobBase &job;
        int maxLevel;
        Linked<IPerfMonHook> chain;
    public:
        CPerfMonHook(CJobBase &_job, unsigned _maxLevel, IPerfMonHook *_chain) : chain(_chain), maxLevel(_maxLevel), job(_job)
        {
        }
        virtual void processPerfStats(unsigned processorUsage, unsigned memoryUsage, unsigned memoryTotal, unsigned __int64 firstDiskUsage, unsigned __int64 firstDiskTotal, unsigned __int64 secondDiskUsage, unsigned __int64 secondDiskTotal, unsigned threadCount)
        {
            if (chain)
                chain->processPerfStats(processorUsage, memoryUsage, memoryTotal, firstDiskUsage,firstDiskTotal, secondDiskUsage, secondDiskTotal, threadCount);
        }
        virtual StringBuffer &extraLogging(StringBuffer &extra)
        {
            if (chain)
                return chain->extraLogging(extra);
            return extra;
        }
        virtual void log(int level, const char *msg)
        {
            PROGLOG("%s", msg);
            if ((maxLevel != -1) && (level <= maxLevel)) // maxLevel of -1 means disabled
            {
                Owned<IThorException> e = MakeThorException(TE_KERN, "%s", msg);
                e->setSeverity(SeverityAlert);
                e->setAction(tea_warning);
                job.fireException(e);
            }
        }
    };
    return new CPerfMonHook(job, maxLevel, chain);
}

bool isOOMException(IException *_e)
{
    if (_e)
    {
        IThorException *e = QUERYINTERFACE(_e, IThorException);
        IException *oe = e && e->queryOriginalException() ? e->queryOriginalException() : _e;
        int ecode = oe->errorCode();
        if (ecode >= ROXIEMM_ERROR_START && ecode <= ROXIEMM_ERROR_END)
            return true;
    }
    return false;
}

IThorException *checkAndCreateOOMContextException(CActivityBase *activity, IException *e, const char *msg, rowcount_t numRows, IOutputMetaData *meta, const void *row)
{
    VStringBuffer errorMsg("Out of memory whilst %s", msg);
    if (RCUNSET != numRows)
        errorMsg.appendf(", group/set size = %" RCPF "u", numRows);
    if (meta)
    {
        if (meta->isFixedSize())
            errorMsg.appendf(", Fixed rows, size = %d", meta->getFixedSize());
        else
            errorMsg.appendf(", Variable rows, min. size = %d", meta->getMinRecordSize());
        if (row && meta->hasXML())
        {
            CommonXmlWriter xmlwrite(0);
            meta->toXML((byte *) row, xmlwrite);
            errorMsg.newline().append("Leading row of group: ").append(xmlwrite.str());
        }
    }
    Owned<IThorException> te = MakeActivityException(activity, e, "%s", errorMsg.str());
    e->Release();
    return te.getClear();
}

RecordTranslationMode getTranslationMode(CActivityBase &activity)
{
    bool local = true;
    StringBuffer val;
    activity.queryContainer().queryXGMML().getProp("hint[@name=\"layouttranslation\"]/@value", val);
    if (!val.length())
    {
        local = false;
        activity.queryJob().getOpt("layoutTranslation", val);
        if (!val.length())
            globals->getProp("@fieldTranslationEnabled", val);
    }
    return getTranslationMode(val, local);
}

void getLayoutTranslations(IConstPointerArrayOf<ITranslator> &translators, const char *fname, IArrayOf<IPartDescriptor> &partDescriptors, RecordTranslationMode translationMode, unsigned expectedFormatCrc, IOutputMetaData *expectedFormat, unsigned projectedFormatCrc, IOutputMetaData *projectedFormat)
{
    if (0 == partDescriptors.ordinality())
        return;
    IPropertyTree &props = partDescriptors.item(0).queryOwner().queryProperties();
    typedef OwningHTMapping<const ITranslator, unsigned> CITranslatorMapping;
    OwningSimpleHashTableOf<CITranslatorMapping, unsigned> translatorTable;
    ForEachItemIn(p, partDescriptors)
    {
        unsigned publishedFormatCrc = (unsigned)props.getPropInt("@formatCrc", 0);
        Owned<const ITranslator> translatorContainer;
        if (translatorTable.ordinality())
        {
            CITranslatorMapping *entry = translatorTable.find(publishedFormatCrc);
            if (entry)
                translatorContainer.set(&entry->queryElement());
        }
        if (!translatorContainer)
        {
            Owned<IOutputMetaData> publishedFormat = getDaliLayoutInfo(props);
            translatorContainer.setown(getTranslators(fname, expectedFormatCrc, expectedFormat, publishedFormatCrc, publishedFormat, projectedFormatCrc, projectedFormat, translationMode));
            if (translatorContainer)
                translatorTable.replace(*new CITranslatorMapping(*translatorContainer.getLink(), publishedFormatCrc));
        }
        translators.append(translatorContainer.getClear());
    }
}

const ITranslator *getLayoutTranslation(const char *fname, IPartDescriptor &partDesc, RecordTranslationMode translationMode, unsigned expectedFormatCrc, IOutputMetaData *expectedFormat, unsigned projectedFormatCrc, IOutputMetaData *projectedFormat)
{
    IPropertyTree const &props = partDesc.queryOwner().queryProperties();
    Owned<IOutputMetaData> actualFormat = getDaliLayoutInfo(props);
    unsigned publishedFormatCrc = (unsigned)props.getPropInt("@formatCrc", 0);
    return getTranslators(fname, expectedFormatCrc, expectedFormat, publishedFormatCrc, actualFormat, projectedFormatCrc, projectedFormat, translationMode);
}

bool isRemoteReadCandidate(const CActivityBase &activity, const RemoteFilename &rfn)
{
#ifndef _CONTAINERIZED
    if (!activity.getOptBool(THOROPT_FORCE_REMOTE_DISABLED))
    {
        if (!rfn.isLocal())
            return true;
        StringBuffer localPath;
        rfn.getLocalPath(localPath);
        if (activity.getOptBool(THOROPT_FORCE_REMOTE_READ, testForceRemote(localPath)))
            return true;
    }
#endif
    return false;
}

void checkAndDumpAbortInfo(const char *cmd)
{
    try
    {
        bool validateAllowedPrograms = true;
        StringBuffer allowedPipePrograms;
        StringBuffer dumpInfoCmd(cmd);
        if (dumpInfoCmd.length())
        {
            /* add some params that might be useful to script
             * 1) Thor instance name
             * 2) base port
             * 3) exe path
             * 4) PID
             */
            const char *myInstanceName = globals->queryProp("@name");
            unsigned myBasePort = getMachinePortBase();
            StringBuffer exePath(queryCurrentProcessPath());
            if (0 == exePath.length())
                exePath.append("process-name-unknown");
            unsigned pid = GetCurrentProcessId();
            dumpInfoCmd.appendf(" %s %u %s %u", myInstanceName, myBasePort, exePath.str(), pid);

            // only allow custom command if listed as allowedPipeProgram. NB: default is "" (none) above.
            getAllowedPipePrograms(allowedPipePrograms, false);
        }
        else
        {
            getDebuggerGetStacksCmd(dumpInfoCmd);
            validateAllowedPrograms = false; // explicitly allow default through
        }
        StringBuffer cmdOutput;
        unsigned retCode = getCommandOutput(cmdOutput, dumpInfoCmd, "slave dump info", validateAllowedPrograms ? allowedPipePrograms.str() : nullptr);
        PROGLOG("\n%s, return code = %u\n%s\n", dumpInfoCmd.str(), retCode, cmdOutput.str());
    }
    catch (IException *e)
    {
        IERRLOG(e);
        e->Release();
    }
}

void checkFileType(CActivityBase *activity, IDistributedFile *file, const char *expectedType, bool throwException)
{
    if (activity->getOptBool(THOROPT_VALIDATE_FILE_TYPE, true))
    {
        const char *kind = queryFileKind(file);
        if (isEmptyString(kind)) // file has no published kind, can't validate
            return;
        if (!strieq(kind, expectedType))
        {
            Owned<IThorException> e = MakeActivityException(activity, ENGINEERR_FILE_TYPE_MISMATCH, "File format mismatch reading file: '%s'. Expected type '%s', but file is type '%s'", file->queryLogicalName(), expectedType, kind);
            if (throwException)
                throw e.getClear();
            e->setAction(tea_warning);
            e->setSeverity(SeverityWarning);
            activity->fireException(e); // will propagate to workunit warning
        }
    }
}

void CThorPerfTracer::start(const char *_workunit, unsigned _subGraphId, double interval)
{
    workunit.set(_workunit);
    subGraphId = _subGraphId;
    DBGLOG("Starting perf trace of subgraph %u, with interval %.3g seconds", subGraphId, interval);
    perf.setInterval(interval);
    perf.start();    
}

void CThorPerfTracer::stop()
{
    DBGLOG("Stopping perf trace of subgraph %u", subGraphId);
    perf.stop();
    StringBuffer flameGraphName;
    if (getConfigurationDirectory(globals->queryPropTree("Directories"), "debug", "thor", globals->queryProp("@name"), flameGraphName))
        addPathSepChar(flameGraphName);
    flameGraphName.appendf("%s/%u/flame_%u.svg", workunit.get(), globals->getPropInt("@slavenum"), subGraphId);
    ensureDirectoryForFile(flameGraphName);
    Owned<IFile> iFile = createIFile(flameGraphName);
    try
    {
        Owned<IFileIO> iFileIO = iFile->open(IFOcreate);
        if (iFileIO)
        {
            StringBuffer &svg = perf.queryResult();
            iFileIO->write(0, svg.length(), svg.str());
            PROGLOG("Flame graph for subgraph %u written to %s", subGraphId, flameGraphName.str());
        }
    }
    catch (IException *E)
    {
        IERRLOG(E);
        ::Release(E);
    }
}

void saveWuidToFile(const char *wuid)
{
    // Store current wuid to a local file, so post mortem script can find it (and if necessary publish files to it)
    Owned<IFile> wuidFile = createIFile("wuid"); // NB: each pod is in it's own private working directory
    Owned<IFileIO> wuidFileIO = wuidFile->open(IFOcreate);
    if (!wuidFileIO)
        throw makeStringException(0, "Failed to create file 'wuid' to store current workunit for post mortem script");
    wuidFileIO->write(0, strlen(wuid), wuid);
    wuidFileIO->close();
}

bool hasTLK(IDistributedFile &file, CActivityBase *activity)
{
    unsigned np = file.numParts();
    if (np<=1) // NB: a better test would be to only continue if this is a width that is +1 of group it's based on, but not worth checking
        return false;
    IDistributedFilePart &part = file.queryPart(np-1);
    bool keyHasTlk = strisame("topLevelKey", part.queryAttributes().queryProp("@kind"));
    if (!keyHasTlk)
    {
        // See HPCC-32845 - check if TLK flag is missing from TLK part
        // It is very likely the last part should be a TLK. Even a local key (>1 parts) has a TLK by default (see buildLocalTlks)
        RemoteFilename rfn;
        part.getFilename(rfn);
        StringBuffer filename;
        rfn.getPath(filename);
        Owned<IKeyIndex> index = createKeyIndex(filename, 0, false, 0);
        dbgassertex(index);
        if (index->isTopLevelKey())
        {
            if (activity)
            {
                Owned<IException> e = MakeActivityException(activity, 0, "TLK file part of file %s is missing kind=\"topLevelKey\" flag. The meta data should be fixed!", file.queryLogicalName());
                reportExceptionToWorkunitCheckIgnore(activity->queryJob().queryWorkUnit(), e, SeverityWarning);
                StringBuffer errMsg;
                UWARNLOG("%s", e->errorMessage(errMsg).str());
            }
            keyHasTlk = true;
        }
    }
    return keyHasTlk;
}

std::vector<std::string> captureDebugInfo(const char *_dir, const char *prefix, const char *suffix)
{
    if (!recursiveCreateDirectory(_dir))
    {
        IWARNLOG("Failed to create debug directory: %s", _dir);
        return {};
    }

    StringBuffer dir(_dir);
    addPathSepChar(dir);

    // utility function to build filename based on prefix, suffix, and extension
    auto getFilename = [&](StringBuffer &result, const char *name, const char *ext) -> StringBuffer &
    {
        result.append(dir);
        if (!isEmptyString(prefix))
        {
            result.append(prefix);
            result.append('_');
        }
        result.append(name);
        if (!isEmptyString(suffix))
        {
            result.append('_');
            result.append(suffix);
        }
        if (!isEmptyString(ext))
        {
            result.append('.');
            result.append(ext);
        }
        return result;
    };
    StringBuffer stacksFName;
    getFilename(stacksFName, "stacks", "txt");
    StringBuffer gdbCmd;
    getDebuggerGetStacksCmd(gdbCmd);
    gdbCmd.append(" > ").append(stacksFName);
    if (0 != system(gdbCmd.str()))
    {
        OWARNLOG("Failed to run gdb to capture stack info. Cmd = %s", gdbCmd.str());
        return { };
    }
    return { stacksFName.str() }; // JCSMORE capture/return other files
}

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


#include "platform.h"

#include <algorithm>
#include "stdio.h"
#include "jlog.hpp"
#include "jlog.ipp"
#include "jmutex.hpp"
#include "jarray.hpp"
#include "jsocket.hpp"
#include "jmisc.hpp"
#include "jprop.hpp"
#include "lnuid.h"
#include <sys/stat.h>
#include "jtrace.hpp"

using namespace ln_uid;

#define FILE_LOG_ENABLES_QUEUEUING

#ifndef _WIN32
#define AUDIT_DATA_LOG_TEMPLATE "/var/log/seisint/log_data_XXXXXX"
#endif

// Time, in nanoseconds, after which the clock field loops --- 3600000000000ns = 1hr
#define CLOCK_LOOP_NANOSECONDS I64C(3600000000000)

// Standard filters, handlers, manager, and audit event logger
static ILogMsgManager * theManager = nullptr;
static PassAllLogMsgFilter * thePassAllFilter = nullptr;
static PassLocalLogMsgFilter * thePassLocalFilter = nullptr;
static PassNoneLogMsgFilter * thePassNoneFilter = nullptr;
static HandleLogMsgHandler * theStderrHandler = nullptr;
static PostMortemLogMsgHandler * thePostMortemHandler = nullptr;
static CSysLogEventLogger * theSysLogEventLogger = nullptr;


// LogMsgSysInfo

static FILE *getNullHandle()
{
#ifdef _WIN32
    return fopen("nul","w");
#else
    return fopen("/dev/null","w");
#endif
}

static bool stdErrIsDevNull()
{
    bool result = false;

#ifndef _WIN32
    int fd2 = fileno(stderr);
    if (fd2 >= 0)
    {
        struct stat stdErr;
        struct stat devNull;
        int srtn = fstat(fd2, &stdErr);
        if (srtn == 0)
        {
            if (S_ISCHR(stdErr.st_mode))
            {
                srtn = stat("/dev/null", &devNull);
                if (srtn == 0)
                {
                    if ( (stdErr.st_ino == devNull.st_ino) &&
                         (stdErr.st_dev == devNull.st_dev) )
                    {
                        result = true;
                    }
                }
            }
        }
    }
#endif

    return result;
}

LogMsgSysInfo::LogMsgSysInfo(LogMsgId _id, unsigned port, LogMsgSessionId session)
{
    id = _id;
#ifdef _WIN32
    // Hack for the fact that Windows doesn't handle gettimeofday
    // Subsecond timing granularities in log files will not be available
    time(&timeStarted);
#else
    gettimeofday(&timeStarted, NULL);
#endif
    processID = GetCurrentProcessId();
    threadID = threadLogID();
    sessionID = session;
#ifdef INCLUDE_LOGMSGSYSINFO_NODE
    node.setLocalHost(port);
#endif
}

void LogMsgSysInfo::serialize(MemoryBuffer & out) const
{
    out.append(id).append((unsigned) queryTime()).append(processID).append(threadID).append(sessionID); node.serialize(out);
}

void LogMsgSysInfo::deserialize(MemoryBuffer & in)
{
    unsigned t;
    in.read(id).read(t).read(processID).read(threadID).read(sessionID); node.deserialize(in);
#ifdef _WIN32
    timeStarted = t;
#else
    timeStarted.tv_sec = t;
    timeStarted.tv_usec = 0;  // For back-compatibility reasons, the subsecond timings are not serialized
#endif
}

class LoggingFieldColumns
{
    const EnumMapping MsgFieldMap[18] =
    {
        { MSGFIELD_msgID,     "MsgID    " },
        { MSGFIELD_audience,  "Audience " },
        { MSGFIELD_class,     "Class    " },
        { MSGFIELD_detail,    "Detail     " },
        { MSGFIELD_date,      "Date       " },
        { MSGFIELD_microTime, "Time(micro)     " },
        { MSGFIELD_milliTime, "Time(milli)  " },
        { MSGFIELD_time,      "Time        " },
        { MSGFIELD_process,   "PID   " },
        { MSGFIELD_thread,    "TID   " },
        { MSGFIELD_session,   "SessionID           " },
        { MSGFIELD_node,      "Node                " },
        { MSGFIELD_job,       "JobID  " },
        { MSGFIELD_user,      "UserID  " },
        { MSGFIELD_component, "Compo " },
        { MSGFIELD_trace,     "Trace " },
        { MSGFIELD_span,      "Span " },
        { MSGFIELD_quote,     "Quoted "}
    };
    const unsigned sizeMsgFieldMap = arraysize(MsgFieldMap);
public:
    unsigned getMaxHeaderSize()
    {
        // Note: return length is slightly longer than necessary as only one time field is valid
        // but the length of all time fields are added
        unsigned size = 0;
        for (unsigned i=0; i<sizeMsgFieldMap; ++i)
            size += strlen(MsgFieldMap[i].str);
        return size+2; // 2 extra characters for \r\n
    }
    unsigned getPositionOfField(unsigned logfields, unsigned positionoffield)
    {
        unsigned pos = 0;
        for (unsigned i=0; i<sizeMsgFieldMap; ++i)
        {
            if (MsgFieldMap[i].val==MSGFIELD_time && (logfields & (MSGFIELD_microTime|MSGFIELD_milliTime)) )
                continue;
            if (MsgFieldMap[i].val & positionoffield)
                break;
            if (MsgFieldMap[i].val & logfields)
                ++pos;
        }
        return pos;
    }
    unsigned extractMessageFieldsFromHeader(const char *line, bool hashPrefixed)
    {
        unsigned fieldHeader = 0;

        if (line && *line=='#')
        {
            ++line;
            const unsigned sizeFieldMap = arraysize(MsgFieldMap);
            for (unsigned i=0; i<sizeFieldMap; ++i)
            {
                const char * linep = line;
                const char * fieldp = MsgFieldMap[i].str;
                while( *fieldp && *linep==*fieldp)
                {
                    ++linep;
                    ++fieldp;
                }
                if (*fieldp==0) // At the end of the field, so whole field matched
                {
                    fieldHeader |= MsgFieldMap[i].val;
                    if (*linep==0 || *linep=='\n')
                        break;
                    line = linep;
                }
            }
        }
        if (fieldHeader & (MSGFIELD_microTime | MSGFIELD_milliTime))
            fieldHeader |= MSGFIELD_time;
        return fieldHeader;
    }
    StringBuffer & generateHeaderRow(StringBuffer & out, unsigned fields, bool prefixHash)
    {
        if (prefixHash)
            out.append('#');
        for (unsigned i=0; i<sizeMsgFieldMap; ++i)
            if (fields & MsgFieldMap[i].val)
            {
                if (MsgFieldMap[i].val==MSGFIELD_time && (fields & (MSGFIELD_microTime|MSGFIELD_milliTime)) )
                    continue;
                out.append(MsgFieldMap[i].str);
            }
        return out;
    }

} loggingFieldColumns;

unsigned getPositionOfField(unsigned logfields, unsigned positionoffield)
{
    return loggingFieldColumns.getPositionOfField(logfields, positionoffield);
}

// LogMsg

LogMsgJobInfo::~LogMsgJobInfo()
{
    if (isDeserialized)
        free((void *) jobIDStr);
}

const char * LogMsgJobInfo::queryJobIDStr() const
{
    if (isDeserialized)
        return jobIDStr;
    else if (jobID == UnknownJob)
        return "UNK";
    else
        return theManager->queryJobId(jobID);
}

LogMsgJobId LogMsgJobInfo::queryJobID() const
{
    if (isDeserialized)
        return UnknownJob;  // Or assert?
    else
        return jobID;
}

void LogMsgJobInfo::setJobID(LogMsgUserId id)
{
    if (isDeserialized)
        free((void *) jobIDStr);
    jobID = id;
    isDeserialized = false;
}

void LogMsgJobInfo::serialize(MemoryBuffer & out) const
{
    if (isDeserialized)
        out.append(jobIDStr);
    else
        out.append(theManager->queryJobId(jobID));
    out.append(userID);
}

void LogMsgJobInfo::deserialize(MemoryBuffer & in)
{
// kludge for backward compatibility of pre 8.0 clients that send a LogMsgJobId: (_uint64), not a string
// NB: jobID pre 8.0 was redundant as always equal to UnknownJob
    if (isDeserialized)
    {
        free((void *) jobIDStr);
        jobIDStr = nullptr;
    }
    dbgassertex(in.remaining() >= sizeof(LogMsgJobId)); // should always be at least this amount, because userID follows the jobID
    if (0 == memcmp(in.toByteArray()+in.getPos(), &UnknownJob, sizeof(LogMsgJobId))) // pre 8.0 client
    {
        in.skip(sizeof(jobID));
        jobID = UnknownJob;
        isDeserialized = false;
    }
    else
    {
        // >= 8.0 client
        StringBuffer idStr;
        in.read(idStr);
        jobIDStr = idStr.detach();
        isDeserialized = true;
    }
    in.read(userID);
}

//--------------------------------------------------------------------------------------------------------------------

static const LogMsgJobInfo queryDefaultJobInfo()
{
    return LogMsgJobInfo(queryThreadedJobId(), UnknownUser);
}

static const LogMsgTraceInfo queryActiveTraceInfo()
{
    return LogMsgTraceInfo(queryThreadedActiveSpan());
}

LogMsg::LogMsg(LogMsgJobId id, const char *job) : category(MSGAUD_programmer, job ? MSGCLS_addid : MSGCLS_removeid), sysInfo(), jobInfo(id), remoteFlag(false)
{
    if (job)
        text.append(job);
}

LogMsg::LogMsg(const LogMsgCategory & _cat, LogMsgId _id, LogMsgCode _code, const char * _text, unsigned port, LogMsgSessionId session)
  : category(_cat), sysInfo(_id, port, session), jobInfo(queryDefaultJobInfo()), traceInfo(queryActiveTraceInfo()),  msgCode(_code), remoteFlag(false)
{
    text.append(_text);
}

LogMsg::LogMsg(const LogMsgCategory & _cat, LogMsgId _id, LogMsgCode _code, size32_t sz, const char * _text, unsigned port, LogMsgSessionId session)
  : category(_cat), sysInfo(_id, port, session), jobInfo(queryDefaultJobInfo()), traceInfo(queryActiveTraceInfo()), msgCode(_code), remoteFlag(false)
{
    text.append(sz, _text);
}

LogMsg::LogMsg(const LogMsgCategory & _cat, LogMsgId _id, LogMsgCode _code, const char * format, va_list args,
       unsigned port, LogMsgSessionId session)
  : category(_cat), sysInfo(_id, port, session), jobInfo(queryDefaultJobInfo()), traceInfo(queryActiveTraceInfo()), msgCode(_code), remoteFlag(false)
{
    text.valist_appendf(format, args);
}

StringBuffer & LogMsg::toStringPlain(StringBuffer & out, unsigned fields) const
{
    out.ensureCapacity(LOG_MSG_FORMAT_BUFFER_LENGTH);
    if(fields & MSGFIELD_msgID)
        out.appendf("id=%X ", sysInfo.queryMsgID());
    if(fields & MSGFIELD_audience)
        out.append("aud=").append(LogMsgAudienceToVarString(category.queryAudience())).append(' ');
    if(fields & MSGFIELD_class)
        out.append("cls=").append(LogMsgClassToFixString(category.queryClass())).append(' ');
    if(fields & MSGFIELD_detail)
        out.appendf("det=%d ", category.queryDetail());
    if(fields & MSGFIELD_timeDate)
    {
        time_t timeNum = sysInfo.queryTime();
        char timeString[12];
        struct tm timeStruct;
        localtime_r(&timeNum, &timeStruct);
        if(fields & MSGFIELD_date)
        {
            strftime(timeString, 12, "%Y-%m-%d ", &timeStruct);
            out.append(timeString);
        }
        if(fields & MSGFIELD_microTime)
        {
            out.appendf("%02d:%02d:%02d.%06d ", timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec, sysInfo.queryUSecs());
        }
        else if(fields & MSGFIELD_milliTime)
        {
            out.appendf("%02d:%02d:%02d.%03d ", timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec, sysInfo.queryUSecs()/1000);
        }
        else if(fields & MSGFIELD_time)
        {
            strftime(timeString, 12, "%H:%M:%S ", &timeStruct);
            out.append(timeString);
        }
    }
    if(fields & MSGFIELD_process)
        out.appendf("pid=%d ",sysInfo.queryProcessID());
    if(fields & MSGFIELD_thread)
        out.appendf("tid=%d ",sysInfo.queryThreadID());
    if(fields & MSGFIELD_session)
    {
        if(sysInfo.querySessionID() == UnknownSession)
            out.append("sid=unknown ");
        else
            out.appendf("sid=%" I64F "u ", sysInfo.querySessionID());
    }
    if(fields & MSGFIELD_node)
    {
        sysInfo.queryNode()->getEndpointHostText(out);
        out.append(" ");
    }
    if(fields & MSGFIELD_job)
    {
        out.appendf("job=%s ", jobInfo.queryJobIDStr());
    }
    if(fields & MSGFIELD_user)
    {
        if(jobInfo.queryUserID() == UnknownUser)
            out.append("usr=unknown ");
        else
            out.appendf("usr=%" I64F "u ", jobInfo.queryUserID());
    }
    if(fields & MSGFIELD_trace)
    {
        //it seems the existing convention is to use 3 char abbreviation
        out.appendf("trc=%s ", traceInfo.queryTraceID());
    }
    if (fields & MSGFIELD_span)
    {
        //it seems the existing convention is to use 3 char abbreviation
        out.appendf("spn=%s ", traceInfo.querySpanID());
    }
    if (fields & MSGFIELD_quote)
        out.append('"');
    if (fields & MSGFIELD_prefix)
        out.append(msgPrefix(category.queryClass()));
    if((fields & MSGFIELD_code) && (msgCode != NoLogMsgCode))
        out.append(msgCode).append(": ").append(text.str());
    else
        out.append(text.str());
    if (fields & MSGFIELD_quote)
        out.append('"');
    return out;
}

StringBuffer & LogMsg::toStringXML(StringBuffer & out, unsigned fields) const
{
    out.ensureCapacity(LOG_MSG_FORMAT_BUFFER_LENGTH);
    out.append("<msg ");
    if(fields & MSGFIELD_msgID)
        out.append("MessageID=\"").append(sysInfo.queryMsgID()).append("\" ");
    if(fields & MSGFIELD_audience)
        out.append("Audience=\"").append(LogMsgAudienceToVarString(category.queryAudience())).append("\" ");
    if(fields & MSGFIELD_class)
        out.append("Class=\"").append(LogMsgClassToVarString(category.queryClass())).append("\" ");
    if(fields & MSGFIELD_detail)
        out.append("Detail=\"").append(category.queryDetail()).append("\" ");
#ifdef LOG_MSG_NEWLINE
    if(fields & MSGFIELD_allCategory) out.append("\n     ");
#endif
    if(fields & MSGFIELD_timeDate)
    {
        time_t timeNum = sysInfo.queryTime();
        char timeString[20];
        struct tm timeStruct;
        localtime_r(&timeNum, &timeStruct);
        if(fields & MSGFIELD_date)
        {
            strftime(timeString, 20, "date=\"%Y-%m-%d\" ", &timeStruct);
            out.append(timeString);
        }
        if(fields & MSGFIELD_microTime)
        {
            out.appendf("time=\"%02d:%02d:%02d.%06d\" ", timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec, sysInfo.queryUSecs());
        }
        else if(fields & MSGFIELD_milliTime)
        {
            out.appendf("time=\"%02d:%02d:%02d.%03d\" ", timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec, sysInfo.queryUSecs()/1000);
        }
        else if(fields & MSGFIELD_time)
        {
            strftime(timeString, 20, "time=\"%H:%M:%S\" ", &timeStruct);
            out.append(timeString);
        }
    }
    if(fields & MSGFIELD_process)
        out.append("PID=\"").append(sysInfo.queryProcessID()).append("\" ");
    if(fields & MSGFIELD_thread)
        out.append("TID=\"").append(sysInfo.queryThreadID()).append("\" ");
    if(fields & MSGFIELD_session)
    {
        if(sysInfo.querySessionID() == UnknownSession)
            out.append("SessionID=\"unknown\" ");
        else
            out.append("SessionID=\"").append(sysInfo.querySessionID()).append("\" ");
    }
    if(fields & MSGFIELD_node)
    {
        out.append("Node=\"");
        sysInfo.queryNode()->getEndpointHostText(out);
        out.append("\" ");
    }
#ifdef LOG_MSG_NEWLINE
    if(fields & MSGFIELD_allSysInfo) out.append("\n     ");
#endif
    if(fields & MSGFIELD_job)
    {
        out.appendf("JobID=\"%s\" ", jobInfo.queryJobIDStr());
    }
    if(fields & MSGFIELD_user)
    {
        if(jobInfo.queryUserID() == UnknownUser)
            out.append("UserID=\"unknown\" ");
        else
            out.append("UserID=\"").append(jobInfo.queryUserID()).append("\" ");
    }
    if(fields & MSGFIELD_trace)
    {
        out.append("TraceID=\"").append(traceInfo.queryTraceID()).append("\" ");
    }
    if (fields & MSGFIELD_span)
    {
        out.append("SpanID=\"").append(traceInfo.querySpanID()).append("\" ");
    }
#ifdef LOG_MSG_NEWLINE
    if(fields & MSGFIELD_allJobInfo) out.append("\n     ");
#endif
    if((fields & MSGFIELD_code) && (msgCode != NoLogMsgCode))
        out.append("code=\"").append(msgCode).append("\" ");
    out.append("text=\"").append(text.str()).append("\" />\n");
    return out;
}

StringBuffer & LogMsg::toStringJSON(StringBuffer & out, unsigned fields) const
{
    out.ensureCapacity(LOG_MSG_FORMAT_BUFFER_LENGTH);
    StringBuffer encodedMsg;
    out.append("{ \"MSG\": \"").append(encodeJSON(encodedMsg, text.str())).append("\"");

    if(fields & MSGFIELD_msgID)
        out.append(", \"MID\": \"").append(sysInfo.queryMsgID()).append("\"");
    if(fields & MSGFIELD_audience)
        out.append(", \"AUD\": \"").append(LogMsgAudienceToFixString(category.queryAudience())).append("\"");
    if(fields & MSGFIELD_class)
        out.append(", \"CLS\": \"").append(LogMsgClassToFixString(category.queryClass())).append("\"");
    if(fields & MSGFIELD_detail)
        out.append(", \"DET\": \"").append(category.queryDetail()).append("\"");
    if(fields & MSGFIELD_timeDate)
    {
        time_t timeNum = sysInfo.queryTime();
        char timeString[23];
        struct tm timeStruct;
        localtime_r(&timeNum, &timeStruct);
        if(fields & MSGFIELD_date)
        {
            strftime(timeString, 23, ", \"DATE\": \"%Y-%m-%d\"", &timeStruct);
            out.append(timeString);
        }
        if(fields & MSGFIELD_microTime)
        {
            out.appendf(", \"TIME\": \"%02d:%02d:%02d.%06d\"", timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec, sysInfo.queryUSecs());
        }
        else if(fields & MSGFIELD_milliTime)
        {
            out.appendf(", \"TIME\": \"%02d:%02d:%02d.%03d\"", timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec, sysInfo.queryUSecs()/1000);
        }
        else if(fields & MSGFIELD_time)
        {
            out.appendf(", \"TIME\": \"%02d:%02d:%02d\"", timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec);
        }
    }
    if(fields & MSGFIELD_process)
        out.append(", \"PID\": \"").append(sysInfo.queryProcessID()).append("\"");
    if(fields & MSGFIELD_thread)
        out.append(", \"TID\": \"").append(sysInfo.queryThreadID()).append("\"");
    if(fields & MSGFIELD_session)
    {
        if(sysInfo.querySessionID() == UnknownSession)
            out.append(", \"SES\": \"UNK\" ");
        else
            out.append(", \"SES\": \"").append(sysInfo.querySessionID()).append("\"");
    }
    if(fields & MSGFIELD_node)
    {
        out.append(", \"NODE\": \"");
        sysInfo.queryNode()->getEndpointHostText(out);
        out.append("\"");
    }
    if(fields & MSGFIELD_job)
    {
        out.appendf(", \"JOBID\": \"%s\"", jobInfo.queryJobIDStr());
    }
    if(fields & MSGFIELD_user)
    {
        if(jobInfo.queryUserID() == UnknownUser)
            out.append(", \"USER\": \"UNK\" ");
        else
            out.append(", \"USER\": \"").append(jobInfo.queryUserID()).append("\"");
    }
    if(fields & MSGFIELD_trace)
    {
        out.appendf("\"TRACEID\"=\"%s\"", traceInfo.queryTraceID());
    }
    if (fields & MSGFIELD_span)
    {
        out.appendf("\"SPANID\"=\"%s\"",traceInfo.querySpanID());
    }
    if((fields & MSGFIELD_code) && (msgCode != NoLogMsgCode))
        out.append(", \"CODE\": \"").append(msgCode).append("\"");

    out.append(" }\n");
    return out;
}

StringBuffer & LogMsg::toStringTable(StringBuffer & out, unsigned fields) const
{
    if(fields & MSGFIELD_msgID)
        out.appendf("%08X ", sysInfo.queryMsgID());
    out.ensureCapacity(LOG_MSG_FORMAT_BUFFER_LENGTH);
    if(fields & MSGFIELD_audience)
        out.appendf("%s ", LogMsgAudienceToFixString(category.queryAudience()));
    if(fields & MSGFIELD_class)
        out.appendf("%s ", LogMsgClassToFixString(category.queryClass()));
    if(fields & MSGFIELD_detail)
        out.appendf("%10d ", category.queryDetail());
    if(fields & MSGFIELD_timeDate)
    {
        time_t timeNum = sysInfo.queryTime();
        char timeString[12];
        struct tm timeStruct;
        localtime_r(&timeNum, &timeStruct);
        if(fields & MSGFIELD_date)
        {
            strftime(timeString, 12, "%Y-%m-%d ", &timeStruct);
            out.append(timeString);
        }
        if(fields & MSGFIELD_microTime)
        {
            out.appendf("%02d:%02d:%02d.%06d ", timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec, sysInfo.queryUSecs());
        }
        else if(fields & MSGFIELD_milliTime)
        {
            out.appendf("%02d:%02d:%02d.%03d ", timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec, sysInfo.queryUSecs()/1000);
        }
        else if(fields & MSGFIELD_time)
        {
            strftime(timeString, 12, "%H:%M:%S ", &timeStruct);
            out.append(timeString);
        }
    }
    if(fields & MSGFIELD_process)
        out.appendf("%5d ",sysInfo.queryProcessID());
    if(fields & MSGFIELD_thread)
        out.appendf("%5d ",sysInfo.queryThreadID());
    if(fields & MSGFIELD_session)
    {
        if(sysInfo.querySessionID() == UnknownSession)
            out.append("      unknown        ");
        else
            out.appendf("%20" I64F "u ", sysInfo.querySessionID());
    }
    if(fields & MSGFIELD_node)
    {
        size32_t len = out.length();
        sysInfo.queryNode()->getEndpointHostText(out);
        out.appendN(20 + len - out.length(), ' ');
    }
    if(fields & MSGFIELD_job)
    {
        out.appendf("%-7s ", jobInfo.queryJobIDStr());
    }
    if(fields & MSGFIELD_trace)
    {
        out.appendf("%s ", traceInfo.queryTraceID());
    }
    if(fields & MSGFIELD_span)
    {
        out.appendf("%s ", traceInfo.querySpanID());
    }
    if(fields & MSGFIELD_user)
    {
        if(jobInfo.queryUserID() == UnknownUser)
            out.append("unknown ");
        else
            out.appendf("%7" I64F "u ", jobInfo.queryUserID());
    }
    if (fields & MSGFIELD_quote)
        out.append('"');
    if (fields & MSGFIELD_prefix)
        out.append(msgPrefix(category.queryClass()));
    if((fields & MSGFIELD_code) && (msgCode != NoLogMsgCode))
        out.append(msgCode).append(": ").append(text.str());
    else
        out.append(text.str());
    if (fields & MSGFIELD_quote)
        out.append('"');
    out.append('\n');
    return out;
}

StringBuffer & LogMsg::toStringTableHead(StringBuffer & out, unsigned fields)
{
    loggingFieldColumns.generateHeaderRow(out, fields, false).append("\n\n");
    return out;
}

void LogMsg::fprintTableHead(FILE * handle, unsigned fields)
{
    StringBuffer  header;
    loggingFieldColumns.generateHeaderRow(header, fields, true).append("\n");
    fputs(header.str(), handle);
}

void LogMsg::deserialize(MemoryBuffer & in)
{
    remoteFlag = true;
    category.deserialize(in);
    sysInfo.deserialize(in);
    jobInfo.deserialize(in);
    in.read(msgCode);
    text.clear();
    text.deserialize(in);
}

unsigned getMessageFieldsFromHeader(FILE *handle)
{
    unsigned currentFieldHeader = 0;
    try
    {
        MemoryBuffer mb(loggingFieldColumns.getMaxHeaderSize());
        fpos_t pos;
        fgetpos (handle,&pos);
        rewind (handle);

        mb.reserve(loggingFieldColumns.getMaxHeaderSize());
        const char * line = fgets (static_cast<char *>(mb.bufferBase()),  loggingFieldColumns.getMaxHeaderSize(), handle );
        if (line && *line)
            currentFieldHeader = loggingFieldColumns.extractMessageFieldsFromHeader(line, true);
        fsetpos (handle, &pos);
    }
    catch (...)
    {
        currentFieldHeader = 0;
    }
    return currentFieldHeader;
}

unsigned getMessageFieldsFromHeader(const char * line)
{
    return loggingFieldColumns.extractMessageFieldsFromHeader(line, true);

}

// Implementations of ILogMsgFilter

void PassAllLogMsgFilter::addToPTree(IPropertyTree * tree) const
{
    IPropertyTree * filterTree = createPTree(ipt_caseInsensitive);
    filterTree->setProp("@type", "all");
    tree->addPropTree("filter", filterTree);
}

void PassLocalLogMsgFilter::addToPTree(IPropertyTree * tree) const
{
    IPropertyTree * filterTree = createPTree(ipt_caseInsensitive);
    filterTree->setProp("@type", "local");
    tree->addPropTree("filter", filterTree);
}

void PassNoneLogMsgFilter::addToPTree(IPropertyTree * tree) const
{
    IPropertyTree * filterTree = createPTree(ipt_caseInsensitive);
    filterTree->setProp("@type", "none");
    tree->addPropTree("filter", filterTree);
}

void CategoryLogMsgFilter::addToPTree(IPropertyTree * tree) const
{
    IPropertyTree * filterTree = createPTree(ipt_caseInsensitive);
    filterTree->setProp("@type", "category");
    filterTree->setPropInt("@audience", audienceMask);
    filterTree->setPropInt("@class", classMask);
    filterTree->setPropInt("@detail", maxDetail);
    if(localFlag) filterTree->setPropInt("@local", 1);
    tree->addPropTree("filter", filterTree);
}

void PIDLogMsgFilter::addToPTree(IPropertyTree * tree) const
{
    IPropertyTree * filterTree = createPTree(ipt_caseInsensitive);
    filterTree->setProp("@type", "pid");
    filterTree->setPropInt("@pid", pid);
    if(localFlag) filterTree->setPropInt("@local", 1);
    tree->addPropTree("filter", filterTree);
}

void TIDLogMsgFilter::addToPTree(IPropertyTree * tree) const
{
    IPropertyTree * filterTree = createPTree(ipt_caseInsensitive);
    filterTree->setProp("@type", "tid");
    filterTree->setPropInt("@tid", tid);
    if(localFlag) filterTree->setPropInt("@local", 1);
    tree->addPropTree("filter", filterTree);
}

void NodeLogMsgFilter::addToPTree(IPropertyTree * tree) const
{
    IPropertyTree * filterTree = createPTree(ipt_caseInsensitive);
    filterTree->setProp("@type", "node");
    StringBuffer buff;
    node.getHostText(buff);
    filterTree->setProp("@ip", buff.str());
    filterTree->setPropInt("@port", node.port);
    if(localFlag) filterTree->setPropInt("@local", 1);
    tree->addPropTree("filter", filterTree);
}

void IpLogMsgFilter::addToPTree(IPropertyTree * tree) const
{
    IPropertyTree * filterTree = createPTree(ipt_caseInsensitive);
    filterTree->setProp("@type", "ip");
    StringBuffer buff;
    ip.getHostText(buff);
    filterTree->setProp("@ip", buff.str());
    if(localFlag) filterTree->setPropInt("@local", 1);
    tree->addPropTree("filter", filterTree);
}

void SessionLogMsgFilter::addToPTree(IPropertyTree * tree) const
{
    IPropertyTree * filterTree = createPTree(ipt_caseInsensitive);
    filterTree->setProp("@type", "session");
    filterTree->setPropInt("@session", (int)session);
    if(localFlag) filterTree->setPropInt("@local", 1);
    tree->addPropTree("filter", filterTree);
}

bool RegexLogMsgFilter::includeMessage(const LogMsg & msg) const
{
    if(localFlag && msg.queryRemoteFlag()) return false;
    SpinBlock b(lock);
    return const_cast<RegExpr &>(regex).find(msg.queryText()) != NULL;
}

void RegexLogMsgFilter::addToPTree(IPropertyTree * tree) const
{
    IPropertyTree * filterTree = createPTree(ipt_caseInsensitive);
    filterTree->setProp("@type", "regex");
    filterTree->setProp("@regex", regexText);
    if(localFlag) filterTree->setPropInt("@local", 1);
    tree->addPropTree("filter", filterTree);
}

void NotLogMsgFilter::addToPTree(IPropertyTree * tree) const
{
    IPropertyTree * filterTree = createPTree(ipt_caseInsensitive);
    filterTree->setProp("@type", "not");
    arg->addToPTree(filterTree);
    tree->addPropTree("filter", filterTree);
}

void AndLogMsgFilter::addToPTree(IPropertyTree * tree) const
{
    IPropertyTree * filterTree = createPTree(ipt_caseInsensitive);
    filterTree->setProp("@type", "and");
    arg1->addToPTree(filterTree);
    arg2->addToPTree(filterTree);
    tree->addPropTree("filter", filterTree);
}

void OrLogMsgFilter::addToPTree(IPropertyTree * tree) const
{
    IPropertyTree * filterTree = createPTree(ipt_caseInsensitive);
    filterTree->setProp("@type", "or");
    arg1->addToPTree(filterTree);
    arg2->addToPTree(filterTree);
    tree->addPropTree("filter", filterTree);
}

void SwitchLogMsgFilter::addToPTree(IPropertyTree * tree) const
{
    IPropertyTree * filterTree = createPTree(ipt_caseInsensitive);
    filterTree->setProp("@type", "switch");
    cond->addToPTree(filterTree);
    yes->addToPTree(filterTree);
    no->addToPTree(filterTree);
    tree->addPropTree("filter", filterTree);
}

void CategoryLogMsgFilter::orWithFilter(const ILogMsgFilter * filter)
{
    audienceMask |= filter->queryAudienceMask();
    classMask |= filter->queryClassMask();
    maxDetail = std::max(maxDetail, filter->queryMaxDetail());
}

void CategoryLogMsgFilter::reset()
{
    audienceMask = 0;
    classMask = 0;
    maxDetail = 0;
}

// HandleLogMsgHandler

void HandleLogMsgHandlerTable::handleMessage(const LogMsg & msg)
{
    CriticalBlock block(crit);
    msg.toStringTable(curMsgText.clear(), messageFields);
    fputs(curMsgText.str(), handle);
}

void HandleLogMsgHandlerTable::addToPTree(IPropertyTree * tree) const
{
    IPropertyTree * handlerTree = createPTree(ipt_caseInsensitive);
    if(handle==stderr)
        handlerTree->setProp("@type", "stderr");
    else
        handlerTree->setProp("@type", "mischandle");
    handlerTree->setPropInt("@fields", messageFields);
    tree->addPropTree("handler", handlerTree);
}

void HandleLogMsgHandlerJSON::handleMessage(const LogMsg & msg)
{
    CriticalBlock block(crit);
    msg.toStringJSON(curMsgText.clear(), messageFields);
    fputs(curMsgText.str(), handle);
}

void HandleLogMsgHandlerJSON::addToPTree(IPropertyTree * tree) const
{
    IPropertyTree * handlerTree = createPTree(ipt_caseInsensitive);
    if(handle==stderr)
        handlerTree->setProp("@type", "stderr");
    else
        handlerTree->setProp("@type", "mischandle");
    handlerTree->setPropInt("@fields", messageFields);
    handlerTree->setProp("@writeJSON", "true");
    tree->addPropTree("handler", handlerTree);
}

void HandleLogMsgHandlerXML::handleMessage(const LogMsg & msg)
{
    CriticalBlock block(crit);
    msg.toStringXML(curMsgText.clear(), messageFields);
    fputs(curMsgText.str(), handle);
}

void HandleLogMsgHandlerXML::addToPTree(IPropertyTree * tree) const
{
    IPropertyTree * handlerTree = createPTree(ipt_caseInsensitive);
    if(handle==stderr)
        handlerTree->setProp("@type", "stderr");
    else
        handlerTree->setProp("@type", "mischandle");
    handlerTree->setPropInt("@fields", messageFields);
    handlerTree->setProp("@writeXML", "true");
    tree->addPropTree("handler", handlerTree);
}

// FileLogMsgHandler

FileLogMsgHandler::FileLogMsgHandler(const char * _filename, const char * _headerText, unsigned _fields, bool _append, bool _flushes)
   : messageFields(_fields), filename(_filename), headerText(_headerText), append(_append), flushes(_flushes)
{
    recursiveCreateDirectoryForFile(filename);
    if(append)
        handle = fopen(filename, "a");
    else
        handle = fopen(filename, "w");
    if(!handle) {
        handle = getNullHandle();
        StringBuffer err;
        err.appendf("LOGGING: could not open file '%s' for output",filename.get());
        OERRLOG("%s",err.str()); // make sure doesn't get lost!
        throw MakeStringException(3000,"%s",err.str()); // 3000: internal error
    }
    if(headerText) fprintf(handle, "--- %s ---\n", (const char *)headerText);
}

static void closeAndDeleteEmpty(const char * filename, FILE *handle)
{
    if (handle) {
        fpos_t pos;
        bool del = (fgetpos(handle, &pos)==0)&&
#if defined( _WIN32) || defined(__FreeBSD__) || defined(__APPLE__)
            (pos==0);
#elif defined(EMSCRIPTEN)
            (pos.__lldata==0);
#else
            (pos.__pos==0);
#endif
        fclose(handle);
        if (del)
            remove(filename);
    }
}

FileLogMsgHandler::~FileLogMsgHandler()
{
    closeAndDeleteEmpty(filename,handle);
}

void FileLogMsgHandlerTable::handleMessage(const LogMsg & msg)
{
    CriticalBlock block(crit);
    msg.toStringTable(curMsgText.clear(), messageFields);
    fputs(curMsgText.str(), handle);
    if(flushes)
        fflush(handle);
}

void FileLogMsgHandlerTable::addToPTree(IPropertyTree * tree) const
{
    IPropertyTree * handlerTree = createPTree(ipt_caseInsensitive);
    handlerTree->setProp("@type", "file");
    handlerTree->setProp("@filename", filename.get());
    if(headerText) handlerTree->setProp("@headertext", headerText.get());
    handlerTree->setPropInt("@fields", messageFields);
    handlerTree->setProp("@writeTable", "true");
    if(append) handlerTree->setProp("@append", "true");
    if(flushes) handlerTree->setProp("@flushes", "true");
    tree->addPropTree("handler", handlerTree);
}

void FileLogMsgHandlerXML::handleMessage(const LogMsg & msg)
{
    CriticalBlock block(crit);
    msg.toStringXML(curMsgText.clear(), messageFields);
    fputs(curMsgText.str(), handle);
    if(flushes)
        fflush(handle);
}

void FileLogMsgHandlerXML::addToPTree(IPropertyTree * tree) const
{
    IPropertyTree * handlerTree = createPTree(ipt_caseInsensitive);
    handlerTree->setProp("@type", "file");
    handlerTree->setProp("@filename", filename.get());
    if(headerText) handlerTree->setProp("@headertext", headerText.get());
    handlerTree->setPropInt("@fields", messageFields);
    if(append) handlerTree->setProp("@append", "true");
    if(flushes) handlerTree->setProp("@flushes", "true");
    tree->addPropTree("handler", handlerTree);
}

void FileLogMsgHandlerJSON::handleMessage(const LogMsg & msg)
{
    CriticalBlock block(crit);
    msg.toStringJSON(curMsgText.clear(), messageFields);
    fputs(curMsgText.str(), handle);
    if(flushes)
        fflush(handle);
}

void FileLogMsgHandlerJSON::addToPTree(IPropertyTree * tree) const
{
    IPropertyTree * handlerTree = createPTree(ipt_caseInsensitive);
    handlerTree->setProp("@type", "file");
    handlerTree->setProp("@filename", filename.get());
    if(headerText)
        handlerTree->setProp("@headertext", headerText.get());
    handlerTree->setPropInt("@fields", messageFields);
    handlerTree->setProp("@writeJSON", "true");
    if(append)
        handlerTree->setProp("@append", "true");
    if(flushes)
        handlerTree->setProp("@flushes", "true");
    tree->addPropTree("handler", handlerTree);
}

// PostMortemLogMsgHandler

PostMortemLogMsgHandler::PostMortemLogMsgHandler(const char * _filebase, unsigned _maxLinesToKeep, unsigned _messageFields)
  : filebase(_filebase), maxLinesToKeep(_maxLinesToKeep), messageFields(_messageFields)
{
    openFile();
}

PostMortemLogMsgHandler::~PostMortemLogMsgHandler()
{
    closeAndDeleteEmpty(filename.length()?filename.str():nullptr, handle);
}

void PostMortemLogMsgHandler::handleMessage(const LogMsg & msg)
{
    CriticalBlock block(crit);
    if (handle)
    {
        checkRollover();
        msg.toStringTable(curMsgText.clear(), messageFields);
        fputs(curMsgText.str(), handle);
        if (flushes)
            fflush(handle);
        if (filename.length()) // don't track if writing to null (and hence no rollover)
            linesInCurrent++;
    }
}

void PostMortemLogMsgHandler::addToPTree(IPropertyTree * tree) const
{
}

void PostMortemLogMsgHandler::checkRollover()
{
    if (linesInCurrent>=maxLinesToKeep)
    {
        doRollover();
    }
}

void PostMortemLogMsgHandler::doRollover()
{
    dbgassertex(filename.length());
    closeAndDeleteEmpty(filename, handle);
    handle = 0;
    if (sequence > 0)
    {
        StringBuffer agedName;
        agedName.append(filebase).append('.').append(sequence-1);
        remove(agedName);
    }
    sequence++;
    openFile();
}

void PostMortemLogMsgHandler::openFile()
{
    filename.clear().append(filebase).append('.').append(sequence);
    recursiveCreateDirectoryForFile(filename.str());
    handle = fopen(filename.str(), "wt");
    if (!handle)
    {
        filename.clear();
        handle = getNullHandle();   // If we can't write where we expected, write to /dev/null instead
    }
    linesInCurrent = 0;
}

void PostMortemLogMsgHandler::copyTo(const char *target, bool clear)
{
    CriticalBlock block(crit);
    if (handle)
    {
        fflush(handle);
        if (clear)
        {
            fclose(handle);
            handle = 0;
        }
    }
    try
    {
        if (sequence) // meaning there is a prior file
        {
            StringBuffer priorName;
            priorName.append(filebase).append('.').append(sequence-1);
            recursiveCreateDirectoryForFile(target);
            copyFile(target, priorName);
            if (clear)
                remove(priorName);
        }
        if (filename.length() && linesInCurrent)
        {
            Owned<IFile> targetIFile = createIFile(target);
            Owned<IFileIO> targetIO;
            if (sequence)
                targetIO.setown(targetIFile->open(IFOreadwrite));
            else
            {
                recursiveCreateDirectoryForFile(target);
                targetIO.setown(targetIFile->open(IFOcreate));
            }
            if (!targetIO)
                throwStringExceptionV(-1, "postmortem copyTo - Failed to open target file %s", target);
            Owned<IFile> currentIFile = createIFile(filename);
            appendFile(targetIO, currentIFile, 0, currentIFile->size());
            if (clear)
                remove(filename);
        }
        if (clear)
            sequence = 0;
    }
    catch (IException *e)
    {
        EXCLOG(e);
        e->Release();
    }
    if (clear)
        openFile();
}

bool PostMortemLogMsgHandler::copyPIDFiles(const char *filebase, const char *target, bool clear)
{
    try
    {
        recursiveCreateDirectoryForFile(target);
        StringBuffer path, tail;
        splitFilename(filebase, nullptr, &path, &tail, &tail);
        tail.append("*");
        Owned<IDirectoryIterator> dirIter = createDirectoryIterator(path, tail);
        StringBuffer fileName;
        Owned<IFile> targetIFile = createIFile(target);
        Owned<IFileIO> targetIO = targetIFile->open(IFOcreate);
        // there should be at most two files to copy (postmortem has current and 1 previous max)
        ForEach(*dirIter)
        {
            dirIter->getName(fileName.clear());
            if (!targetIO)
                throwStringExceptionV(-1, "postmortem copyPIDFiles - Failed to open target file %s", target);
            Owned<IFile> currentIFile = createIFile(fileName);
            appendFile(targetIO, currentIFile, 0, currentIFile->size());
            if (clear)
                remove(fileName);
        }
    }
    catch (IException *e)
    {
        EXCLOG(e);
        e->Release();
        return false;
    }
    return true;
}

bool copyPostMortemLogging(const char *target, bool clear)
{
    if (!thePostMortemHandler)
        return false;
    thePostMortemHandler->copyTo(target, clear);
    return true;
}

StringBuffer &getPostMortemPidFilename(StringBuffer &out, unsigned pid)
{
    if (0 == pid)
        pid = GetCurrentProcessId();
    out.appendf("/tmp/postmortem.%u.log", pid);
    return out;
}

bool getDebugInstanceDir(StringBuffer &dir, const char *componentType, const char *instance)
{
    Owned<IPropertyTree> config = getComponentConfig();
    if (!getConfigurationDirectory(config->queryPropTree("Directories"), "debug", componentType, config->queryProp("@name"), dir))
    {
        if (!isContainerized())
        {
            appendCurrentDirectory(dir, false);
            addPathSepChar(dir);
            dir.append("debuginfo"); // use ./debuginfo in non-containerized mode
        }
        else
        {
            IWARNLOG("Failed to get debug directory");
            return false;
        }
    }
    addPathSepChar(dir);
    if (!isEmptyString(instance))
    {
        dir.append(instance);
        addPathSepChar(dir);
    }
    CDateTime now;
    timestamp_type nowTime = getTimeStampNowValue();
    now.setTimeStamp(nowTime);
    unsigned year, month, day, hour, minute, second, nano;
    now.getDate(year, month, day);
    now.getTime(hour, minute, second, nano);
    unsigned hundredths = ((unsigned __int64)nano) * 100 / 1000000000;
    VStringBuffer dateStr("%04u%02u%02u-%02u%02u%02u.%02u", year, month, day, hour, minute, second, hundredths);
    dir.append(dateStr);
    return true;
}

bool copyPortMortemPIDFiles(HANDLE pid, const char *target, bool clear)
{
    StringBuffer pidFilenameBase;
    getPostMortemPidFilename(pidFilenameBase, (unsigned)pid);
    return PostMortemLogMsgHandler::copyPIDFiles(pidFilenameBase, target, clear);
}


// RollingFileLogMsgHandler
#define MIN_LOGFILE_SIZE_LIMIT 10000
#define LOG_LINE_SIZE_ESTIMATE 80

RollingFileLogMsgHandler::RollingFileLogMsgHandler(const char * _filebase, const char * _fileextn, unsigned _fields, bool _append, bool _flushes, const char *initialName, const char *_alias, bool daily, long _maxLogFileSize)
  : handle(0), messageFields(_fields), alias(_alias), filebase(_filebase), fileextn(_fileextn), append(_append), flushes(_flushes), maxLogFileSize(_maxLogFileSize)
{
    if (_maxLogFileSize)
    {
        if (_maxLogFileSize < MIN_LOGFILE_SIZE_LIMIT)                 // Setting the cap too low, doesn't work well
            maxLogFileSize = MIN_LOGFILE_SIZE_LIMIT;
        maxLogFileSize = _maxLogFileSize - (LOG_LINE_SIZE_ESTIMATE*2); // Trying to keep log file size below capped
    };

    time_t tNow;
    time(&tNow);
    localtime_r(&tNow, &startTime);
    doRollover(daily, initialName);
    checkRollover();
}

RollingFileLogMsgHandler::~RollingFileLogMsgHandler()
{
    closeAndDeleteEmpty(filename,handle);
}

void RollingFileLogMsgHandler::addToPTree(IPropertyTree * tree) const
{
    IPropertyTree * handlerTree = createPTree(ipt_caseInsensitive);
    handlerTree->setProp("@type", "rollingfile");
    handlerTree->setProp("@filebase", filebase.get());
    handlerTree->setProp("@fileextn", fileextn.get());
    handlerTree->setPropInt("@fields", messageFields);
    if(append) handlerTree->setProp("@append", "true");
    if(flushes) handlerTree->setProp("@flushes", "true");
    tree->addPropTree("handler", handlerTree);
}

void RollingFileLogMsgHandler::checkRollover()
{
    time_t tNow;
    time(&tNow);
    struct tm ltNow;
    localtime_r(&tNow, &ltNow);
    if(ltNow.tm_year != startTime.tm_year || ltNow.tm_yday != startTime.tm_yday)
    {
        localtime_r(&tNow, &startTime);  // reset the start time for next rollover check
        doRollover(true);
    }
    else if (maxLogFileSize)
    {
        linesSinceSizeChecked++;
        if (linesSinceSizeChecked > sizeCheckNext)
        {
            long fsize = ftell(handle);
            if ((fsize==-1 && errno==EOVERFLOW) || (fsize >= maxLogFileSize))
            {
                localtime_r(&tNow, &startTime);
                doRollover(false);
            }
            else
                // Calc how many lines to skip before next log file size check
                //  - using (LOG_LINE_SIZE_ESTIMATE*2) to ensure the size check is done well before limit
                sizeCheckNext = (maxLogFileSize - fsize) / (LOG_LINE_SIZE_ESTIMATE*2);
            linesSinceSizeChecked = 0;
        }
    }
}

void RollingFileLogMsgHandler::doRollover(bool daily, const char *forceName)
{
    CriticalBlock block(crit);
    closeAndDeleteEmpty(filename,handle);
    handle = 0;
    filename.clear();
    if (forceName)
        filename.append(forceName);
    else
    {
        filename.clear().append(filebase.get());
        addFileTimestamp(filename, daily);
        filename.append(fileextn.get());
    }
    recursiveCreateDirectoryForFile(filename.str());
    handle = fopen(filename.str(), append ? "a+" : "w");
    printHeader = true;
    currentLogFields = 0;
    if (handle)
    {
        if (append)
        {
            fseek(handle, 0, SEEK_END);
            long pos = ftell(handle);
            if (pos > 0 || (pos==-1 && errno==EOVERFLOW)) // If current file is not empty
            {
                printHeader = false;
                unsigned logfields = getMessageFieldsFromHeader(handle);
                if (logfields == 0) // No header file so write log lines legacy field format
                    currentLogFields = MSGFIELD_LEGACY;
                else if (logfields != messageFields) // Different log format from format in current log file
                    currentLogFields = logfields;
            }
        }
        if (alias && alias.length())
        {
            fclose(handle);
            handle = 0;
            remove(alias);
            try
            {
                createHardLink(alias, filename.str());
            }
            catch (IException *E)
            {
                recursiveCreateDirectoryForFile(filename.str());
                handle = fopen(filename.str(), append ? "a" : "w");
                EXCLOG(E);  // Log the fact that we could not create the alias - probably it is locked (tail a bit unfortunate on windows).
                E->Release();
            }
            if (!handle)
            {
                recursiveCreateDirectoryForFile(filename.str());
                handle = fopen(filename.str(), append ? "a" : "w");
            }
        }
    }
    if(!handle)
    {
        handle = getNullHandle();
        OWARNLOG("RollingFileLogMsgHandler::doRollover : could not open log file %s for output", filename.str());
        // actually this is pretty fatal
    }
}

// BinLogMsgHandler

BinLogMsgHandler::BinLogMsgHandler(const char * _filename, bool _append) : filename(_filename), append(_append)
{
    file.setown(createIFile(filename.get()));
    if(!file) assertex(!"BinLogMsgHandler::BinLogMsgHandler : Could not create IFile");
    if(append)
        fio.setown(file->open(IFOwrite));
    else
        fio.setown(file->open(IFOcreate));
    if(!fio) assertex(!"BinLogMsgHandler::BinLogMsgHandler : Could not create IFileIO");
    fstr.setown(createIOStream(fio));
    if(!fstr) assertex(!"BinLogMsgHandler::BinLogMsgHandler : Could not create IFileIOStream");
    if(append)
        fstr->seek(0, IFSend);
}

BinLogMsgHandler::~BinLogMsgHandler()
{
    fstr.clear();
    fio.clear();
    file.clear();
}

void BinLogMsgHandler::handleMessage(const LogMsg & msg)
{
    CriticalBlock block(crit);
    mbuff.clear();
    msg.serialize(mbuff);
    size32_t msglen = mbuff.length();
    fstr->write(sizeof(msglen), &msglen);
    fstr->write(msglen, mbuff.toByteArray());
}

void BinLogMsgHandler::addToPTree(IPropertyTree * tree) const
{
    IPropertyTree * handlerTree = createPTree(ipt_caseInsensitive);
    handlerTree->setProp("@type", "binary");
    handlerTree->setProp("@filename", filename.get());
    if(append) handlerTree->setProp("@append", "true");
    tree->addPropTree("handler", handlerTree);
}

// LogMsgPrepender

void LogMsgPrepender::report(const LogMsgCategory & cat, const char * format, ...)
{
    StringBuffer buff;
    buff.append(file).append("(").append(line).append(") : ").append(format);
    va_list args;
    va_start(args, format);
    queryLogMsgManager()->report_va(cat, buff.str(), args);
    va_end(args);
}

void LogMsgPrepender::report_va(const LogMsgCategory & cat, const char * format, va_list args)
{
    StringBuffer buff;
    buff.append(file).append("(").append(line).append(") : ").append(format);
    queryLogMsgManager()->report_va(cat, buff.str(), args);
}

void LogMsgPrepender::report(const LogMsgCategory & cat, LogMsgCode code, const char * format, ...)
{
    StringBuffer buff;
    buff.append(file).append("(").append(line).append(") : ").append(format);
    va_list args;
    va_start(args, format);
    queryLogMsgManager()->report_va(cat, buff.str(), args);
    va_end(args);
}

void LogMsgPrepender::report_va(const LogMsgCategory & cat, LogMsgCode code, const char * format, va_list args)
{
    StringBuffer buff;
    buff.append(file).append("(").append(line).append(") : ").append(format);
    queryLogMsgManager()->report_va(cat, buff.str(), args);
}

void LogMsgPrepender::report(const LogMsgCategory & cat, const IException * exception, const char * prefix)
{
    StringBuffer buff;
    buff.append(file).append("(").append(line).append(") : ");
    if(prefix) buff.append(prefix).append(" : ");
    exception->errorMessage(buff);
    queryLogMsgManager()->report(cat, exception->errorCode(), "%s", buff.str());
}

IException * LogMsgPrepender::report(IException * e, const char * prefix, LogMsgClass cls)
{
    report(MCexception(e, cls), e, prefix);
    return e;
}

// LogMsgMonitor

void LogMsgMonitor::addToPTree(IPropertyTree * tree) const
{
    IPropertyTree * monitorTree = createPTree(ipt_caseInsensitive);
    handler->addToPTree(monitorTree);
    filter->addToPTree(monitorTree);
    tree->addPropTree("monitor", monitorTree);
}

// CLogMsgManager

void CLogMsgManager::MsgProcessor::push(LogMsg * msg)
{
    //assertex(more); an assertex will just recurse here
    if (!more) // we are effective stopped so don't bother even dropping (and leak parameter) as drop will involve
               // interaction with the base class which is stopped and could easily crash (as this condition
               // is expected not to occur - typically occurs if the user has incorrectly called exit on one thread
               // while still in the process of logging on another)
               // cf Bug #53695 for more discussion of the issue
        return;
    else if(droppingLimit && (q.ordinality() >= droppingLimit))
        drop();
    q.enqueue(msg);
}

int CLogMsgManager::MsgProcessor::run()
{
    Owned<LogMsg> msg;
    while(more)
    {
        msg.setown(q.dequeueAndNotify(this)); // notify locks mutex on non-null return
        if(!msg)
            break;
        owner->doReport(*msg);
        pullCycleMutex.unlock();
    }
    while(true)
    {
        msg.setown(q.dequeueNowAndNotify(this)); // notify locks mutex on non-null return
        if(!msg)
            break;
        owner->doReport(*msg);
        pullCycleMutex.unlock();
    }
    return 0;
}

void CLogMsgManager::MsgProcessor::notify(LogMsg *)
{
    pullCycleMutex.lock();
}

void CLogMsgManager::MsgProcessor::setBlockingLimit(unsigned lim)
{
    q.setLimit(lim);
    droppingLimit = 0;
}

void CLogMsgManager::MsgProcessor::setDroppingLimit(unsigned lim, unsigned num)
{
    numToDrop = num;
    droppingLimit = lim;
    q.setLimit(0);
}

void CLogMsgManager::MsgProcessor::resetLimit()
{
    droppingLimit = 0;
    q.setLimit(0);
}

void CLogMsgManager::MsgProcessor::stop()
{
    more = false;
    q.stop();
}

void CLogMsgManager::MsgProcessor::drop()
{
    Owned<LogMsg> msg, lastMsg;
    unsigned count;
    unsigned prev = 0;
    for(count = 0; count < numToDrop; count++)
    {
        msg.setown(q.dequeueTail(0));
        if(!msg) break;
        DropLogMsg * dmsg = dynamic_cast<DropLogMsg *>(msg.get());
        if(dmsg) prev += dmsg->queryCount()-1;
        lastMsg.setown(msg.getClear());
    }
    if(lastMsg)
        q.enqueue(new DropLogMsg(owner, lastMsg->querySysInfo().queryMsgID(), count+prev));
}

bool CLogMsgManager::MsgProcessor::flush(unsigned timeout)
{
    unsigned start = msTick();
    if(!q.waitMaxOrdinality(0, timeout))
        return false;
    unsigned now = msTick();
    if(now - start >= timeout)
        return false;
    try
    {
        TimedMutexBlock block(pullCycleMutex, timeout+start-now);
    }
    catch(IException * e)
    {
        e->Release();
        return false;
    }
    return true;
}

CLogMsgManager::~CLogMsgManager()
{
    CriticalBlock crit(modeLock);
    if(processor)
    {
        processor->stop();
        processor->join();
    }
}

LogMsgJobId CLogMsgManager::addJobId(const char *job)
{
    LogMsgJobId ret = ++nextJobId;
    pushMsg(new LogMsg(ret, job));
    return ret;
}

void CLogMsgManager::removeJobId(LogMsgJobId id)
{
    pushMsg(new LogMsg(id, nullptr));
}

const char * CLogMsgManager::queryJobId(LogMsgJobId id) const
{
    // NOTE - thread safety is important here. We have to consider two things:
    // 1. Whether an id (and therefore an entry in this table) can be invalidated between the return statement and someone using the result
    //    It is up to the calling application to ensure that it does not call removeJobId() on an ID that may still be being used for logging by another thread.
    // 2. Whether the table lookup may coincide with a table add, and crash in getValue/setValue
    //    This is a non-issue in queueing mode as all gets/sets happen on a single thread, but we lock to be on the safe side

    CriticalBlock b(jobIdLock);
    StringAttr *found = jobIds.getValue(id);
    return found ? found->get() : "invalid";
}

void CLogMsgManager::doAddJobId(LogMsgJobId id, const char *text) const
{
    CriticalBlock b(jobIdLock);
    jobIds.setValue(id, text);
}

void CLogMsgManager::doRemoveJobId(LogMsgJobId id) const
{
    CriticalBlock b(jobIdLock);
    jobIds.remove(id);
}

void CLogMsgManager::enterQueueingMode()
{
    CriticalBlock crit(modeLock);
    if(processor) return;
    processor.setown(new MsgProcessor(this));
    processor->setBlockingLimit(defaultMsgQueueLimit);
    processor->start(false);
}

void CLogMsgManager::setQueueBlockingLimit(unsigned lim)
{
    CriticalBlock crit(modeLock);
    if(processor)
        processor->setBlockingLimit(lim);
}

void CLogMsgManager::setQueueDroppingLimit(unsigned lim, unsigned numToDrop)
{
    CriticalBlock crit(modeLock);
    if(processor)
        processor->setDroppingLimit(lim, numToDrop);
}

void CLogMsgManager::resetQueueLimit()
{
    CriticalBlock crit(modeLock);
    if(processor)
        processor->resetLimit();
}

void CLogMsgManager::report(const LogMsgCategory & cat, const char * format, ...)
{
    if(rejectsCategory(cat)) return;
    va_list args;
    va_start(args, format);
    pushMsg(new LogMsg(cat, getNextID(), NoLogMsgCode, format, args, port, session));
    va_end(args);
}

void CLogMsgManager::report_va(const LogMsgCategory & cat, const char * format, va_list args)
{
    if(rejectsCategory(cat)) return;
    pushMsg(new LogMsg(cat, getNextID(), NoLogMsgCode, format, args, port, session));
}

void CLogMsgManager::report(const LogMsgCategory & cat, LogMsgCode code, const char * format, ...)
{
    if(rejectsCategory(cat)) return;
    va_list args;
    va_start(args, format);
    pushMsg(new LogMsg(cat, getNextID(), code, format, args, port, session));
    va_end(args);
}

void CLogMsgManager::report_va(const LogMsgCategory & cat, LogMsgCode code, const char * format, va_list args)
{
    if(rejectsCategory(cat)) return;
    pushMsg(new LogMsg(cat, getNextID(), code, format, args, port, session));
}

void CLogMsgManager::mreport_direct(const LogMsgCategory & cat, const char * msg)
{
    if(rejectsCategory(cat)) return;
    const char *cursor = msg;
    const char *lineStart = cursor;
    while (true)
    {
        switch (*cursor)
        {
            case '\0':
                if (cursor != lineStart || cursor==msg)
                    pushMsg(new LogMsg(cat, getNextID(), NoLogMsgCode, (int)(cursor-lineStart), lineStart, port, session));
                return;
            case '\r':
                // NB: \r or \r\n translated into newline
                pushMsg(new LogMsg(cat, getNextID(), NoLogMsgCode, (int)(cursor-lineStart), lineStart, port, session));
                if ('\n' == *(cursor+1))
                    cursor++;
                lineStart = cursor+1;
                break;
            case '\n':
                pushMsg(new LogMsg(cat, getNextID(), NoLogMsgCode, (int)(cursor-lineStart), lineStart, port, session));
                lineStart = cursor+1;
                break;
        }
        ++cursor;
    }
}

void CLogMsgManager::mreport_va(const LogMsgCategory & cat, const char * format, va_list args)
{
    if(rejectsCategory(cat)) return;
    StringBuffer log;
    log.limited_valist_appendf(1024*1024, format, args);
    mreport_direct(cat, log);
}

void CLogMsgManager::report(const LogMsgCategory & cat, const IException * exception, const char * prefix)
{
    if(rejectsCategory(cat)) return;
    StringBuffer buff;
    if(prefix) buff.append(prefix).append(" : ");
    exception->errorMessage(buff);
    pushMsg(new LogMsg(cat, getNextID(), exception->errorCode(), buff.str(), port, session));
}

void CLogMsgManager::pushMsg(LogMsg * _msg)
{
    Owned<LogMsg> msg(_msg);
    if(processor)
        processor->push(msg.getLink());
    else
        doReport(*msg);
}

void CLogMsgManager::doReport(const LogMsg & msg) const
{
    try
    {
        switch (msg.queryCategory().queryClass())
        {
        case MSGCLS_addid:
            doAddJobId(msg.queryJobInfo().queryJobID(), msg.queryText());
            break;
        case MSGCLS_removeid:
            doRemoveJobId(msg.queryJobInfo().queryJobID());
            break;
        default:
            ReadLockBlock block(monitorLock);
            ForEachItemIn(i, monitors)
                monitors.item(i).processMessage(msg);
            break;
        }
    }
    catch(IException * e)
    {
        StringBuffer err("exception reporting log message: ");
        err.append(e->errorCode());
        e->errorMessage(err);
        panic(err.str());
        e->Release();
    }
    catch(...)
    {
        panic("unknown exception reporting log message");
    }
}

void CLogMsgManager::panic(char const * reason) const
{
    fprintf(stderr, "%s", reason); // not sure there's anything more useful we can do here
}

offset_t CLogMsgManager::getLogPosition(StringBuffer &logFileName, const ILogMsgHandler * handler) const
{
    if (processor)
        processor->flush(10*1000);
    WriteLockBlock block(monitorLock);  // Prevents any incoming messages as we are doing this
    return handler->getLogPosition(logFileName);
}

aindex_t CLogMsgManager::find(const ILogMsgHandler * handler) const
{
    // N.B. Should be used inside critical block
    ForEachItemIn(i, monitors)
        if(monitors.item(i).queryHandler()==handler) return i;
    return NotFound;
}

bool CLogMsgManager::addMonitor(ILogMsgHandler * handler, ILogMsgFilter * filter)
{
    flushQueue(10*1000);
    WriteLockBlock block(monitorLock);
    if(find(handler) != NotFound) return false;
    monitors.append(*(new LogMsgMonitor(filter, handler)));
    prefilter.orWithFilter(filter);
    sendFilterToChildren(true);
    return true;
}

bool CLogMsgManager::addMonitorOwn(ILogMsgHandler * handler, ILogMsgFilter * filter)
{
    bool ret = addMonitor(handler, filter);
    filter->Release();
    handler->Release();
    return ret;
}

void CLogMsgManager::buildPrefilter()
{
    // N.B. Should be used inside critical block
    prefilter.reset();
    ForEachItemIn(i, monitors)
        prefilter.orWithFilter(monitors.item(i).queryFilter());
}

bool CLogMsgManager::removeMonitor(ILogMsgHandler * handler)
{
    Linked<LogMsgMonitor> todelete;
    {
        WriteLockBlock block(monitorLock);
        aindex_t pos = find(handler);
        if(pos == NotFound) return false;
        todelete.set(&monitors.item(pos));
        monitors.remove(pos);
        buildPrefilter();
        sendFilterToChildren(true);
        return true;
    }
}

unsigned CLogMsgManager::removeMonitorsMatching(HandlerTest & test)
{
    CIArrayOf<LogMsgMonitor>  todelete; // delete outside monitorLock
    unsigned count = 0;
    {
        WriteLockBlock block(monitorLock);
        ForEachItemInRev(i, monitors)
            if(test(monitors.item(i).queryHandler()))
            {
                LogMsgMonitor &it = monitors.item(i);
                it.Link();
                todelete.append(it);
                monitors.remove(i);
                ++count;
            }
        buildPrefilter();
        sendFilterToChildren(true);
    }
    return count;
}

void CLogMsgManager::removeAllMonitors()
{
    CIArrayOf<LogMsgMonitor>  todelete; // delete outside monitorLock
    {
        WriteLockBlock block(monitorLock);
        ForEachItemInRev(i, monitors) {
            LogMsgMonitor &it = monitors.item(i);
            it.Link();
            todelete.append(it);
            monitors.remove(i);
        }
        prefilter.reset();
        sendFilterToChildren(true);
    }
}

void CLogMsgManager::resetMonitors()
{
    suspendChildren();
    removeAllMonitors();
    Owned<ILogMsgFilter> defaultFilter = getDefaultLogMsgFilter();

    // dont add stderr log handler if it has been redirected to /dev/null ...

    if (!stdErrIsDevNull())
        addMonitor(theStderrHandler, defaultFilter);
    unsuspendChildren();
}

ILogMsgFilter * CLogMsgManager::queryMonitorFilter(const ILogMsgHandler * handler) const
{
    ReadLockBlock block(monitorLock);
    aindex_t pos = find(handler);
    if(pos == NotFound) return 0;
    return monitors.item(pos).queryFilter();
}

bool CLogMsgManager::changeMonitorFilter(const ILogMsgHandler * handler, ILogMsgFilter * newFilter)
{
    WriteLockBlock block(monitorLock);
    aindex_t pos = find(handler);
    if(pos == NotFound) return 0;
    monitors.item(pos).setFilter(newFilter);
    buildPrefilter();
    sendFilterToChildren(true);
    return true;
}

void CLogMsgManager::prepAllHandlers() const
{
    ReadLockBlock block(monitorLock);
    ForEachItemIn(i, monitors)
        if(monitors.item(i).queryHandler()->needsPrep()) monitors.item(i).queryHandler()->prep();
}

aindex_t CLogMsgManager::findChild(ILogMsgLinkToChild * child) const
{
    ForEachItemIn(i, children)
        if(&(children.item(i)) == child ) return i;
    return NotFound;
}

ILogMsgFilter * CLogMsgManager::getCompoundFilter(bool locked) const
{
    if(!locked) monitorLock.lockRead();
    Owned<CategoryLogMsgFilter> categoryFilter = new CategoryLogMsgFilter(0, 0, 0, false);
    Owned<ILogMsgFilter> otherFilters;
    ILogMsgFilter * ifilter;
    bool hadCat = false;
    ForEachItemIn(i, monitors)
    {
        ifilter = monitors.item(i).queryFilter();
        if(ifilter->queryLocalFlag()) continue;
        if(ifilter->isCategoryFilter())
        {
            categoryFilter->orWithFilter(ifilter);
            hadCat = true;
        }
        else
        {
            if(otherFilters)
                otherFilters.setown(getOrLogMsgFilter(otherFilters, ifilter));
            else
                otherFilters.set(ifilter);
        }
    }
    if(hadCat)
    {
        if(otherFilters)
            otherFilters.setown(getOrLogMsgFilter(otherFilters, categoryFilter));
        else
            otherFilters.set(categoryFilter);
    }
    if(!locked) monitorLock.unlock();
    if(!otherFilters)
        return getPassNoneLogMsgFilter();
    return otherFilters.getLink();
}

void CLogMsgManager::sendFilterToChildren(bool locked) const
{
    if(suspendedChildren) return;
    ReadLockBlock block(childLock);
    if(children.length()==0) return;
    ILogMsgFilter * filter = getCompoundFilter(locked);
    ForEachItemIn(i, children)
        children.item(i).sendFilter(filter);
    filter->Release();
}

bool CLogMsgManager::addMonitorToPTree(const ILogMsgHandler * handler, IPropertyTree * tree) const
{
    ReadLockBlock block(monitorLock);
    aindex_t pos = find(handler);
    if(pos == NotFound) return false;
    monitors.item(pos).addToPTree(tree);
    return true;
}

void CLogMsgManager::addAllMonitorsToPTree(IPropertyTree * tree) const
{
    ReadLockBlock block(monitorLock);
    ForEachItemIn(i, monitors)
        monitors.item(i).addToPTree(tree);
}

bool CLogMsgManager::rejectsCategory(const LogMsgCategory & cat) const
{
    if (!prefilter.includeCategory(cat))
        return true;

    ReadLockBlock block(monitorLock);
    ForEachItemIn(i, monitors)
    {
        if (monitors.item(i).queryFilter()->mayIncludeCategory(cat))
            return false;
    }
    return true;
}

// Helper functions

ILogMsgFilter * getDeserializedLogMsgFilter(MemoryBuffer & in)
{
    unsigned type;
    in.read(type);
    switch(type)
    {
    case MSGFILTER_passall : return LINK(thePassAllFilter);
    case MSGFILTER_passlocal : return LINK(thePassLocalFilter);
    case MSGFILTER_passnone : return LINK(thePassNoneFilter);
    case MSGFILTER_category : return new CategoryLogMsgFilter(in);
    case MSGFILTER_pid : return new PIDLogMsgFilter(in);
    case MSGFILTER_tid : return new TIDLogMsgFilter(in);
    case MSGFILTER_node : return new NodeLogMsgFilter(in);
    case MSGFILTER_ip : return new IpLogMsgFilter(in);
    case MSGFILTER_session : return new SessionLogMsgFilter(in);
    case MSGFILTER_regex : return new RegexLogMsgFilter(in);
    case MSGFILTER_not : return new NotLogMsgFilter(in);
    case MSGFILTER_and : return new AndLogMsgFilter(in);
    case MSGFILTER_or : return new OrLogMsgFilter(in);
    case MSGFILTER_switch : return new SwitchLogMsgFilter(in);
    default: assertex(!"getDeserializedLogMsgFilter: unrecognized LogMsgFilterType");
    }
    return 0;
}

ILogMsgFilter * getLogMsgFilterFromPTree(IPropertyTree * xml)
{
    /* Note that several of these constructors use GetPropInt and GetPropInt64 to get unsigneds. I think this is OK? (all int64 internally)*/
    StringBuffer type;
    xml->getProp("@type", type);
    if(strcmp(type.str(), "all")==0) return LINK(thePassAllFilter);
    else if(strcmp(type.str(), "local")==0) return LINK(thePassLocalFilter);
    else if(strcmp(type.str(), "none")==0) return LINK(thePassNoneFilter);
    else if(strcmp(type.str(), "category")==0) return new CategoryLogMsgFilter(xml);
    else if(strcmp(type.str(), "pid")==0) return new PIDLogMsgFilter(xml);
    else if(strcmp(type.str(), "tid")==0) return new TIDLogMsgFilter(xml);
    else if(strcmp(type.str(), "node")==0) return new NodeLogMsgFilter(xml);
    else if(strcmp(type.str(), "ip")==0) return new IpLogMsgFilter(xml);
    else if(strcmp(type.str(), "session")==0) return new SessionLogMsgFilter(xml);
    else if(strcmp(type.str(), "regex")==0) return new RegexLogMsgFilter(xml);
    else if(strcmp(type.str(), "not")==0) return new NotLogMsgFilter(xml);
    else if(strcmp(type.str(), "and")==0) return new AndLogMsgFilter(xml);
    else if(strcmp(type.str(), "or")==0) return new OrLogMsgFilter(xml);
    else if(strcmp(type.str(), "filter")==0) return new SwitchLogMsgFilter(xml);
    else assertex(!"getLogMsgFilterFromPTree : unrecognized LogMsgFilter type");
    return getPassAllLogMsgFilter();
}

ILogMsgFilter * getDefaultLogMsgFilter()
{
    return new CategoryLogMsgFilter(MSGAUD_all, MSGCLS_all, DefaultDetail, true);
}

ILogMsgFilter * getPassAllLogMsgFilter()
{
    return LINK(thePassAllFilter);
}

ILogMsgFilter * getLocalLogMsgFilter()
{
    return LINK(thePassLocalFilter);
}

ILogMsgFilter * getPassNoneLogMsgFilter()
{
    return LINK(thePassNoneFilter);
}

ILogMsgFilter * queryPassAllLogMsgFilter()
{
    return thePassAllFilter;
}

ILogMsgFilter * queryLocalLogMsgFilter()
{
    return thePassLocalFilter;
}

ILogMsgFilter * queryPassNoneLogMsgFilter()
{
    return thePassNoneFilter;
}

ILogMsgFilter * getCategoryLogMsgFilter(unsigned audiences, unsigned classes, LogMsgDetail maxDetail, bool local)
{
    if((audiences==MSGAUD_all) && (classes==MSGCLS_all) && (maxDetail==TopDetail))
    {
        if(local)
            return LINK(thePassLocalFilter);
        else
            return LINK(thePassAllFilter);
    }
    return new CategoryLogMsgFilter(audiences, classes, maxDetail, local);
}

ILogMsgFilter * getPIDLogMsgFilter(unsigned pid, bool local)
{
    return new PIDLogMsgFilter(pid, local);
}

ILogMsgFilter * getTIDLogMsgFilter(unsigned tid, bool local)
{
    return new TIDLogMsgFilter(tid, local);
}

ILogMsgFilter * getNodeLogMsgFilter(const char * name, unsigned port, bool local)
{
    return new NodeLogMsgFilter(name, port, local);
}

ILogMsgFilter * getNodeLogMsgFilter(const IpAddress & ip, unsigned port, bool local)
{
    return new NodeLogMsgFilter(ip, port, local);
}

ILogMsgFilter * getNodeLogMsgFilter(unsigned port, bool local)
{
    return new NodeLogMsgFilter(port, local);
}

ILogMsgFilter * getIpLogMsgFilter(const char * name, bool local)
{
    return new IpLogMsgFilter(name, local);
}

ILogMsgFilter * getIpLogMsgFilter(const IpAddress & ip, bool local)
{
    return new IpLogMsgFilter(ip, local);
}

ILogMsgFilter * getIpLogMsgFilter(bool local)
{
    return new IpLogMsgFilter(local);
}

ILogMsgFilter * getSessionLogMsgFilter(LogMsgSessionId session, bool local)
{
    return new SessionLogMsgFilter(session, local);
}

ILogMsgFilter * getRegexLogMsgFilter(const char *regex, bool local)
{
    return new RegexLogMsgFilter(regex, local);
}

ILogMsgFilter * getNotLogMsgFilter(ILogMsgFilter * arg)
{
    return new NotLogMsgFilter(arg);
}

ILogMsgFilter * getNotLogMsgFilterOwn(ILogMsgFilter * arg)
{
    ILogMsgFilter * ret = new NotLogMsgFilter(arg);
    arg->Release();
    return ret;
}

ILogMsgFilter * getAndLogMsgFilter(ILogMsgFilter * arg1, ILogMsgFilter * arg2)
{
    return new AndLogMsgFilter(arg1, arg2);
}

ILogMsgFilter * getAndLogMsgFilterOwn(ILogMsgFilter * arg1, ILogMsgFilter * arg2)
{
    ILogMsgFilter * ret = new AndLogMsgFilter(arg1, arg2);
    arg1->Release();
    arg2->Release();
    return ret;
}

ILogMsgFilter * getOrLogMsgFilter(ILogMsgFilter * arg1, ILogMsgFilter * arg2)
{
    return new OrLogMsgFilter(arg1, arg2);
}

ILogMsgFilter * getOrLogMsgFilterOwn(ILogMsgFilter * arg1, ILogMsgFilter * arg2)
{
    ILogMsgFilter * ret = new OrLogMsgFilter(arg1, arg2);
    arg1->Release();
    arg2->Release();
    return ret;
}

ILogMsgFilter * getSwitchLogMsgFilterOwn(ILogMsgFilter * switchFilter, ILogMsgFilter * yesFilter, ILogMsgFilter * noFilter)
{
    ILogMsgFilter * ret = new SwitchLogMsgFilter(switchFilter, yesFilter, noFilter);
    switchFilter->Release();
    yesFilter->Release();
    noFilter->Release();
    return ret;
}

ILogMsgHandler * getHandleLogMsgHandler(FILE * handle, unsigned fields, LogHandlerFormat logFormat)
{
    switch (logFormat)
    {
    case LOGFORMAT_xml:
        return new HandleLogMsgHandlerXML(handle, fields);
    case LOGFORMAT_json:
        return new HandleLogMsgHandlerJSON(handle, fields);
    case LOGFORMAT_table:
    default:
        return new HandleLogMsgHandlerTable(handle, fields);
    }
}

ILogMsgHandler * getFileLogMsgHandler(const char * filename, const char * headertext, unsigned fields, LogHandlerFormat logFormat, bool append, bool flushes)
{
    switch (logFormat)
    {
    case LOGFORMAT_xml:
        return new FileLogMsgHandlerXML(filename, headertext, fields, append, flushes);
    case LOGFORMAT_json:
        return new FileLogMsgHandlerJSON(filename, headertext, fields, append, flushes);
    case LOGFORMAT_table:
    default:
        return new FileLogMsgHandlerTable(filename, headertext, fields, append, flushes);
    }
}

ILogMsgHandler * getRollingFileLogMsgHandler(const char * filebase, const char * fileextn, unsigned fields, bool append, bool flushes, const char *initialName, const char *alias, bool daily, long maxLogSize)
{
    return new RollingFileLogMsgHandler(filebase, fileextn, fields, append, flushes, initialName, alias, daily, maxLogSize);
}

ILogMsgHandler * getBinLogMsgHandler(const char * filename, bool append)
{
    return new BinLogMsgHandler(filename, append);
}

ILogMsgHandler * getPostMortemLogMsgHandler(const char * filebase, unsigned maxLinesToKeep, unsigned messageFields)
{
    return new PostMortemLogMsgHandler(filebase, maxLinesToKeep, messageFields);
}

void installLogMsgFilterSwitch(ILogMsgHandler * handler, ILogMsgFilter * switchFilter, ILogMsgFilter * newFilter)
{
    queryLogMsgManager()->changeMonitorFilterOwn(handler, getSwitchLogMsgFilterOwn(switchFilter, newFilter, queryLogMsgManager()->getMonitorFilter(handler)));
}

ILogMsgHandler * getLogMsgHandlerFromPTree(IPropertyTree * tree)
{
    StringBuffer type;
    tree->getProp("@type", type);
    unsigned fields = MSGFIELD_all;
    char const * fstr = tree->queryProp("@fields");
    if(fstr)
    {
        if(isdigit(fstr[0]))
            fields = atoi(fstr);
        else
            fields = logMsgFieldsFromAbbrevs(fstr);
    }
    if(strcmp(type.str(), "stderr")==0)
        return getHandleLogMsgHandler(stderr, fields, tree->hasProp("@writeXML") ? LOGFORMAT_xml : LOGFORMAT_table);
    else if(strcmp(type.str(), "file")==0)
    {
        StringBuffer filename;
        tree->getProp("@filename", filename);
        if(tree->hasProp("@headertext"))
        {
            StringBuffer headertext;
            tree->getProp("@headertext", headertext);
            //JSON format now available, but currently not an option for file bound logs
            return getFileLogMsgHandler(filename.str(), headertext.str(), fields, !(tree->hasProp("@writeTable")) ? LOGFORMAT_xml : LOGFORMAT_table, tree->hasProp("@append"), tree->hasProp("@flushes"));
        }
        else
        {
            //JSON format now available, but currently not an option for file bound logs
            return getFileLogMsgHandler(filename.str(), 0, fields, !(tree->hasProp("@writeTable")) ? LOGFORMAT_xml : LOGFORMAT_table, tree->hasProp("@append"), tree->hasProp("@flushes"));
        }
    }
    else if(strcmp(type.str(), "binary")==0)
    {
        StringBuffer filename;
        tree->getProp("@filename", filename);
        return getBinLogMsgHandler(filename.str(), tree->hasProp("@append"));
    }
    else assertex(!"getLogMsgFilterFromPTree : unrecognized LogMsgHandler type");
    return LINK(theStderrHandler);
}

ILogMsgHandler * attachStandardFileLogMsgMonitor(const char * filename, const char * headertext, unsigned fields, unsigned audiences, unsigned classes, LogMsgDetail detail, LogHandlerFormat logFormat, bool append, bool flushes, bool local)
{
#ifdef FILE_LOG_ENABLES_QUEUEUING
    queryLogMsgManager()->enterQueueingMode();
#endif
    ILogMsgFilter * filter = getCategoryLogMsgFilter(audiences, classes, detail, local);
    ILogMsgHandler * handler = getFileLogMsgHandler(filename, headertext, fields, logFormat, append, flushes);
    queryLogMsgManager()->addMonitorOwn(handler, filter);
    return handler;
}

ILogMsgHandler * attachStandardBinLogMsgMonitor(const char * filename, unsigned audiences, unsigned classes, LogMsgDetail detail, bool append, bool local)
{
#ifdef FILE_LOG_ENABLES_QUEUEUING
    queryLogMsgManager()->enterQueueingMode();
#endif
    ILogMsgFilter * filter = getCategoryLogMsgFilter(audiences, classes, detail, local);
    ILogMsgHandler * handler = getBinLogMsgHandler(filename, append);
    queryLogMsgManager()->addMonitorOwn(handler, filter);
    return handler;
}

ILogMsgHandler * attachStandardHandleLogMsgMonitor(FILE * handle, unsigned fields, unsigned audiences, unsigned classes, LogMsgDetail detail, LogHandlerFormat logFormat, bool local)
{
    ILogMsgFilter * filter = getCategoryLogMsgFilter(audiences, classes, detail, local);
    ILogMsgHandler * handler = getHandleLogMsgHandler(handle, fields, logFormat);
    queryLogMsgManager()->addMonitorOwn(handler, filter);
    return handler;
}

ILogMsgHandler * attachLogMsgMonitorFromPTree(IPropertyTree * tree)
{
    Owned<IPropertyTree> handlertree = tree->getPropTree("handler");
    Owned<IPropertyTree> filtertree = tree->getPropTree("filter");
    ILogMsgHandler * handler = getLogMsgHandlerFromPTree(handlertree);
    ILogMsgFilter * filter = getLogMsgFilterFromPTree(filtertree);
    queryLogMsgManager()->addMonitorOwn(handler, filter);
    return handler;
}

void attachManyLogMsgMonitorsFromPTree(IPropertyTree * tree)
{
    Owned<IPropertyTreeIterator> iter = tree->getElements("monitor");
    ForEach(*iter)
        attachLogMsgMonitorFromPTree(&(iter->query()));
}

// Calls to make, remove, and return the manager, standard handler, pass all/none filters, reporter array

class CNullManager : implements ILogMsgManager
{
public:
// IInterface impl.
    virtual void Link() const override {}
    virtual bool Release() const override { return false; }

// ILogMsgListener impl.
    virtual bool              addMonitor(ILogMsgHandler * handler, ILogMsgFilter * filter) override { return true; }
    virtual bool              addMonitorOwn(ILogMsgHandler * handler, ILogMsgFilter * filter) override { return true; }
    virtual bool              removeMonitor(ILogMsgHandler * handler)  override { return true; }
    virtual unsigned          removeMonitorsMatching(HandlerTest & test) override { return 0; }
    virtual void              removeAllMonitors()  override {}
    virtual bool              isActiveMonitor(const ILogMsgHandler * handler) const override { return false; }
    virtual ILogMsgFilter *   queryMonitorFilter(const ILogMsgHandler * handler) const override { return nullptr; }
    virtual ILogMsgFilter *   getMonitorFilter(const ILogMsgHandler * handler) const override { return nullptr; }
    virtual bool              changeMonitorFilter(const ILogMsgHandler * handler, ILogMsgFilter * newFilter) override { return true; }
    virtual bool              changeMonitorFilterOwn(const ILogMsgHandler * handler, ILogMsgFilter * newFilter) override { return true; }
    virtual void              prepAllHandlers() const override {}
    virtual void              addChildOwn(ILogMsgLinkToChild * child) override {}
    virtual void              removeChild(ILogMsgLinkToChild * child) override {}
    virtual void              removeAllChildren() override {}
    virtual ILogMsgFilter *   getCompoundFilter(bool locked = false) const override { return nullptr; }
    virtual void              suspendChildren() override {}
    virtual void              unsuspendChildren() override {}
    virtual bool              addMonitorToPTree(const ILogMsgHandler * handler, IPropertyTree * tree) const override { return true; }
    virtual void              addAllMonitorsToPTree(IPropertyTree * tree) const override {}
    virtual void              setPort(unsigned _port) override {}
    virtual unsigned          queryPort() const override { return 0; }
    virtual void              setSession(LogMsgSessionId _session) override {}
    virtual LogMsgSessionId   querySession() const override { return 0; }

// ILogMsgManager impl.
    virtual void              enterQueueingMode() override {}
    virtual void              setQueueBlockingLimit(unsigned lim) override {}
    virtual void              setQueueDroppingLimit(unsigned lim, unsigned numToDrop) override {}
    virtual void              resetQueueLimit() override {}
    virtual bool              flushQueue(unsigned timeout) override { return true; }
    virtual void              resetMonitors() override {}
    virtual void              report(const LogMsgCategory & cat, const char * format, ...) override {}
    virtual void              report_va(const LogMsgCategory & cat, const char * format, va_list args) override {}
    virtual void              report(const LogMsgCategory & cat, LogMsgCode code , const char * format, ...) override {}
    virtual void              report_va(const LogMsgCategory & cat, LogMsgCode code , const char * format, va_list args) override {}
    virtual void              report(const LogMsgCategory & cat, const IException * e, const char * prefix = NULL) override {}
    virtual void              mreport_direct(const LogMsgCategory & cat, const char * msg) override {}
    virtual void              mreport_va(const LogMsgCategory & cat, const char * format, va_list args) override {}
    virtual void              report(const LogMsg & msg) const override {}
    virtual LogMsgId          getNextID() override { return 0; }
    virtual bool              rejectsCategory(const LogMsgCategory & cat) const override { return true; }
    virtual offset_t          getLogPosition(StringBuffer &logFileName, const ILogMsgHandler * handler) const override { return 0; }
    virtual LogMsgJobId       addJobId(const char *job) override { return 0; }
    virtual void              removeJobId(LogMsgJobId) override {}
    virtual const char *      queryJobId(LogMsgJobId id) const override { return ""; }
};

static CNullManager nullManager;
static Singleton<IRemoteLogAccess> logAccessor;


MODULE_INIT(INIT_PRIORITY_JLOG)
{
    thePassAllFilter = new PassAllLogMsgFilter();
    thePassLocalFilter = new PassLocalLogMsgFilter();
    thePassNoneFilter = new PassNoneLogMsgFilter();
    theStderrHandler = new HandleLogMsgHandlerTable(stderr, MSGFIELD_STANDARD);

    theSysLogEventLogger = new CSysLogEventLogger;
    theManager = new CLogMsgManager();
    theManager->resetMonitors();
    return true;
}
MODULE_EXIT()
{
    ::Release(theManager);
    theManager = &nullManager;
    delete theSysLogEventLogger;
    delete theStderrHandler;
    delete thePostMortemHandler;
    delete thePassNoneFilter;
    delete thePassLocalFilter;
    delete thePassAllFilter;
    theSysLogEventLogger = nullptr;
    theStderrHandler = nullptr;
    thePostMortemHandler = nullptr;
    thePassNoneFilter = nullptr;
    thePassLocalFilter = nullptr;
    thePassAllFilter = nullptr;
    logAccessor.destroy([](IRemoteLogAccess * _logAccessor) { ::Release(_logAccessor); });
}

static constexpr const char * logFieldsAtt = "@fields";
static constexpr const char * logMsgDetailAtt = "@detail";
static constexpr const char * logMsgAudiencesAtt = "@audiences";
static constexpr const char * logMsgClassesAtt = "@classes";
static constexpr const char * logQueueLenAtt = "@queueLen";
static constexpr const char * logQueueDropAtt = "@queueDrop";
static constexpr const char * logDisabledAtt = "@disabled";
static constexpr const char * useSysLogpAtt ="@enableSysLog";
static constexpr const char * capturePostMortemAtt ="@postMortem";
static constexpr const char * logFormatAtt ="@format";

static constexpr unsigned queueLenDefault = 512;
static constexpr unsigned queueDropDefault = 0; // disabled by default
static constexpr bool useSysLogDefault = false;

// returns LOGFORMAT_undefined if format has not changed
// NB: returns LOGFORMAT_table if no format specified (i.e. this is the default)
LogHandlerFormat getConfigHandlerFormat(const IPropertyTree *logConfig)
{
    LogHandlerFormat currentFormat = theStderrHandler->queryFormatType();
    LogHandlerFormat newFormat{LOGFORMAT_undefined};
    const char *newFormatString = logConfig ? logConfig->queryProp(logFormatAtt) : nullptr;
    if (!newFormatString) // absent, defaults to "table"
        newFormat = LOGFORMAT_table;
    else
    {
        if (streq(newFormatString, "xml"))
            newFormat = LOGFORMAT_xml;
        else if (streq(newFormatString, "json"))
            newFormat = LOGFORMAT_json;
        else if (streq(newFormatString, "table"))
            newFormat = LOGFORMAT_table;
        else
            OWARNLOG("JLog: Invalid log format configuration detected '%s'!", newFormatString);
    }
    return currentFormat != newFormat ? newFormat : LOGFORMAT_undefined;
}

void updateStdErrLogHandler(const IPropertyTree *logConfig)
{
    if (logConfig->hasProp(logFieldsAtt))
    {
        // Supported logging fields: TRC,SPN,AUD,CLS,DET,MID,TIM,DAT,PID,TID,NOD,JOB,USE,SES,COD,MLT,MCT,NNT,COM,QUO,PFX,ALL,STD
        const char *logFields = logConfig->queryProp(logFieldsAtt);
        if (!isEmptyString(logFields))
            theStderrHandler->setMessageFields(logMsgFieldsFromAbbrevs(logFields));
    }

    // Only recreate filter if at least one filter attribute configured
    if (logConfig->hasProp(logMsgDetailAtt) || logConfig->hasProp(logMsgAudiencesAtt) || logConfig->hasProp(logMsgClassesAtt))
    {
        LogMsgDetail logDetail = logConfig->getPropInt(logMsgDetailAtt, DefaultDetail);

        unsigned msgClasses = MSGCLS_all;
        const char *logClasses = logConfig->queryProp(logMsgClassesAtt);
        if (!isEmptyString(logClasses))
            msgClasses = logMsgClassesFromAbbrevs(logClasses);

        unsigned msgAudiences = MSGAUD_all;
        const char *logAudiences = logConfig->queryProp(logMsgAudiencesAtt);
        if (!isEmptyString(logAudiences))
            msgAudiences = logMsgAudsFromAbbrevs(logAudiences);

        const bool local = true; // Do not include remote messages from other components
        Owned<ILogMsgFilter> filter = getCategoryLogMsgFilter(msgAudiences, msgClasses, logDetail, local);
        theManager->changeMonitorFilter(theStderrHandler, filter);
    }
}

static void loggingSetupUpdate(const IPropertyTree *oldComponentConfiguration, const IPropertyTree *oldGlobalConfiguration)
{
    Owned<IPropertyTree> logConfig = getComponentConfigSP()->getPropTree("logging");
    if (!logConfig)
        return;
    if (logConfig->getPropBool(logDisabledAtt, false))
    {
        OWARNLOG("JLog: Ignoring can't be disabled dynamically via an update");
        return;
    }

    LogHandlerFormat newFormat = getConfigHandlerFormat(logConfig);
    if (newFormat != LOGFORMAT_undefined)
    {
        OWARNLOG("JLog: Ignoring log format configuration change on the fly, as it is not supported in a containerized environment");
    }

    updateStdErrLogHandler(logConfig);
}

static CConfigUpdateHook configUpdateHook;

// NB: it is not thread-safe to change the handler whilst other threads are logging.
// This should only be called at startup.
void setupContainerizedLogMsgHandler()
{
    Owned<IPropertyTree> logConfig = getComponentConfigSP()->getPropTree("logging");
    if (logConfig && logConfig->getPropBool(logDisabledAtt, false))
    {
        removeLog();
        return; // NB: can't be reenabled dynamically via an update at present
    }
    LogHandlerFormat newFormat = getConfigHandlerFormat(logConfig); // NB: theStderrHandler is initially setup in MODULE_INIT
    if (newFormat != LOGFORMAT_undefined)
    {
        // NB: old theStderrHandler leaks, should be fixed by separate PR.
        switch (newFormat)
        {
            case LOGFORMAT_xml:
                theStderrHandler = new HandleLogMsgHandlerXML(stderr, MSGFIELD_STANDARD);
                break;
            case LOGFORMAT_json:
                theStderrHandler = new HandleLogMsgHandlerJSON(stderr, MSGFIELD_STANDARD);
                break;
            case LOGFORMAT_table:
                theStderrHandler = new HandleLogMsgHandlerTable(stderr, MSGFIELD_STANDARD);
                break;
            default:
                throwUnexpected(); // should never reach here
        }
        theManager->resetMonitors();
    }
    if (logConfig)
    {
        unsigned queueLen = logConfig->getPropInt(logQueueLenAtt, queueLenDefault);
        if (queueLen)
        {
            queryLogMsgManager()->enterQueueingMode();
            unsigned queueDrop = logConfig->getPropInt(logQueueDropAtt, queueDropDefault);
            if (queueDrop)
            {
                queryLogMsgManager()->setQueueDroppingLimit(queueLen, queueDrop);
                PROGLOG("JLog: queuing enabled with dropping: queueLen=%u, queueDrop=%u", queueLen, queueDrop);
            }
            else
            {
                queryLogMsgManager()->setQueueBlockingLimit(queueLen);
                PROGLOG("JLog: queuing enabled: queueLen=%u", queueLen);
            }
        }

        if (logConfig->getPropBool(useSysLogpAtt, useSysLogDefault))
            UseSysLogForOperatorMessages();

        unsigned postMortemLines = logConfig->getPropInt(capturePostMortemAtt, 0);
        if (postMortemLines)
        {
            // augment postmortem files with <pid> to avoid clashes where multiple processes are running within
            // same process space, e.g. hthor processes running in same k8s container
            StringBuffer portMortemFileBase;
            getPostMortemPidFilename(portMortemFileBase, 0);

            thePostMortemHandler = new PostMortemLogMsgHandler(portMortemFileBase, postMortemLines, MSGFIELD_STANDARD);
            queryLogMsgManager()->addMonitor(thePostMortemHandler, getCategoryLogMsgFilter(MSGAUD_all, MSGCLS_all, TopDetail));
        }

        updateStdErrLogHandler(logConfig);
    }
    configUpdateHook.installOnce(loggingSetupUpdate, false);
}

ILogMsgManager * queryLogMsgManager()
{
    return theManager;
}

ILogMsgHandler * queryStderrLogMsgHandler()
{
    return theStderrHandler;
}

ILogMsgHandler * queryPostMortemLogMsgHandler()
{
    return thePostMortemHandler;
}

ILogMsgManager * createLogMsgManager() // use with care! (needed by mplog listener facility)
{
    return new CLogMsgManager();
}

// Event Logging

ISysLogEventLogger * querySysLogEventLogger()
{
    return theSysLogEventLogger;
}

ILogMsgHandler * getSysLogMsgHandler(unsigned fields)
{
    return new SysLogMsgHandler(theSysLogEventLogger, fields);
}

#ifdef _WIN32

#include <WINNT.H>
#include "jelog.h"

struct AuditTypeWin32Data
{
public:
    unsigned eventtype;
    unsigned categoryid;
    unsigned eventid;
};

#define CATEGORY_AUDIT_FUNCTION_REQUIRED
#define AUDIT_TYPES_BEGIN AuditTypeWin32Data auditTypeDataMap[NUM_AUDIT_TYPES+1] = {
#define MAKE_AUDIT_TYPE(name, type, categoryid, eventid, level) {type, categoryid, eventid},
#define AUDIT_TYPES_END {0, 0, 0} };
#include "jelogtype.hpp"
#undef CATEGORY_AUDIT_FUNCTION_REQUIRED
#undef AUDIT_TYPES_BEGIN
#undef MAKE_AUDIT_TYPE
#undef AUDIT_TYPES_END

CSysLogEventLogger::CSysLogEventLogger() : hEventLog(0)
{
}

bool CSysLogEventLogger::log(AuditType auditType, char const * msg, size32_t datasize, void const * data)
{
    assertex(auditType < NUM_AUDIT_TYPES);
    AuditTypeWin32Data const & typeData = auditTypeDataMap[auditType];
    return win32Report(typeData.eventtype, typeData.categoryid, typeData.eventid, msg, datasize, data);
}

bool CSysLogEventLogger::win32Report(unsigned eventtype, unsigned category, unsigned eventid, const char * msg, size32_t datasize, const void * data)
{
    if (hEventLog==0) {
        // MORE - this doesn't work on Vista/Win7 as can't copy to system32...
        // Perhaps we should just kill this code
        char path[_MAX_PATH+1];
        GetEnvironmentVariable("SystemRoot",path,sizeof(path));
        strcat(path,"\\System32\\JELOG.dll");
        Owned<IFile> file = createIFile(path);
        try {
            if (!file->exists()) {
                char src[_MAX_PATH+1];
                LPTSTR tail;
                DWORD res = SearchPath(NULL,"JELOG.DLL",NULL,sizeof(src),src,&tail);
                if (res>0)
                    copyFile(path,src);
                else
                    throw makeOsException(GetLastError());
            }
        }
        catch (IException *e)
        {
            EXCLOG(e, "reportEventLog: Could not install JELOG.DLL");
            hEventLog=(HANDLE)-1;
            e->Release();
            return false;

        }
        HKEY hk;
        if (RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\Seisint",
                           NULL, NULL, 0, KEY_ALL_ACCESS, NULL, &hk, NULL)==0) {
            DWORD sizedata = 0;
            DWORD type = REG_EXPAND_SZ;
            if ((RegQueryValueEx(hk,"EventMessageFile",NULL, &type, NULL, &sizedata)!=0)||!sizedata) {
                StringAttr str("%SystemRoot%\\System32\\JELOG.dll");
                RegSetValueEx(hk,"EventMessageFile", 0, REG_EXPAND_SZ, (LPBYTE) str.get(), (DWORD)str.length() + 1);
                RegSetValueEx(hk,"CategoryMessageFile", 0, REG_EXPAND_SZ, (LPBYTE) str.get(), (DWORD)str.length() + 1);
                DWORD dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE | EVENTLOG_AUDIT_SUCCESS | EVENTLOG_AUDIT_FAILURE;
                RegSetValueEx(hk, "TypesSupported", 0, REG_DWORD,  (LPBYTE) &dwData,  sizeof(DWORD));
                dwData = 16;
                RegSetValueEx(hk, "CategoryCount", 0, REG_DWORD,  (LPBYTE) &dwData,  sizeof(DWORD));
            }
            RegCloseKey(hk);
        }
        hEventLog = RegisterEventSource(NULL,"Seisint");
        if (!hEventLog) {
            OERRLOG("reportEventLog: Could not register Seisint event source");
            hEventLog=(HANDLE)-1;
            return false;
        }
    }
    if (hEventLog==(HANDLE)-1)
        return false;
    assertex((unsigned)eventtype<=16);
    if (!data)
        datasize = 0;
    else if (!datasize)
        data = NULL;
#if 1 //useful for debugging...
    ReportEvent(hEventLog, eventtype, category, eventid, NULL, 1, datasize, &msg, (LPVOID)data);
#else
    if(datasize)
    {
        char * buff = (char *)malloc(datasize*3+1);
        unsigned char const * cdata = (unsigned char *)data;
        unsigned i;
        for(i=0; i<datasize; i++)
            sprintf(buff+i*3, "%02X ", cdata[i]);
        buff[datasize*3-1] = 0;
        DBGLOG("ReportEvent: type=%X categoryid=%X eventid=%X msg='%s' data=[%s]", eventtype, category, eventid, msg, buff);
        free(buff);
    }
    else
        DBGLOG("ReportEvent: type=%X categoryid=%X eventid=%X msg='%s'", eventtype, category, eventid, msg);
#endif
    return true;
}

CSysLogEventLogger::~CSysLogEventLogger()
{
    if (hEventLog!=0)
        DeregisterEventSource(hEventLog);
}

#else

#include <syslog.h>

#define CATEGORY_AUDIT_FUNCTION_REQUIRED
#define AUDIT_TYPES_BEGIN int auditTypeDataMap[NUM_AUDIT_TYPES+1] = {
#define MAKE_AUDIT_TYPE(name, type, categoryid, eventid, level) level,
#define AUDIT_TYPES_END 0 };
#include "jelogtype.hpp"
#undef CATEGORY_AUDIT_FUNCTION_REQUIRED
#undef AUDIT_TYPES_BEGIN
#undef MAKE_AUDIT_TYPE
#undef AUDIT_TYPES_END

CSysLogEventLogger::CSysLogEventLogger() : dataLogUsed(false), dataLogName(0), dataLogFile(-1)
{
    StringBuffer folder;
    const char * processName = splitDirTail(queryCurrentProcessPath(), folder);
    if (!processName||!*processName)
        processName = "hpcc";
    openlog(processName, LOG_PID, LOG_USER);
}

CSysLogEventLogger::~CSysLogEventLogger()
{
    if(dataLogFile != -1)
        close(dataLogFile);
    if(dataLogName)
        delete [] dataLogName;
    closelog();
}

bool CSysLogEventLogger::log(AuditType auditType, const char *msg, size32_t datasize, const void * data)
{
    assertex(auditType < NUM_AUDIT_TYPES);
    int level = auditTypeDataMap[auditType];
    return linuxReport(level, msg, datasize, data);
}

bool CSysLogEventLogger::linuxReport(int level, const char * msg, size32_t datasize, const void * data)
{
    if (!data)
        datasize = 0;
    else if (!datasize)
        data = NULL;
    bool ret = true;
#if 1 //useful for debugging...
    if(data)
    {
        if(!dataLogUsed)
            openDataLog();
        if(dataLogFile != -1)
        {
            int fpos = writeDataLog(datasize, (byte const *)data);
            if(fpos != -1)
                syslog(level, "%s [0x%X bytes of data at %s byte 0x%X]", msg, datasize, dataLogName, fpos);
            else
                syslog(level, "%s [could not write 0x%X bytes of data to %s]", msg, datasize, dataLogName);
        }
        else
        {
            ret = false;
            syslog(level, "%s [could not open file of form %s to write data]", msg, AUDIT_DATA_LOG_TEMPLATE);
        }
    }
    else
    {
        syslog(level, "%s", msg);
    }
#else
    if(datasize)
    {
        char * buff = (char *)malloc(datasize*3+1);
        unsigned char const * cdata = (unsigned char *)data;
        unsigned i;
        for(i=0; i<datasize; i++)
            sprintf(buff+i*3, "%02X ", cdata[i]);
        buff[datasize*3-1] = 0;
        DBGLOG("syslog: priority=%X msg='%s' data=[%s]", level, msg, buff);
        free(buff);
    }
    else
        DBGLOG("syslog: priority=%X msg='%s'", level, msg);
#endif
    return ret;
}

void CSysLogEventLogger::openDataLog()
{
    CriticalBlock block(dataLogLock);
    dataLogUsed = true;
    unsigned len = strlen(AUDIT_DATA_LOG_TEMPLATE);
    dataLogName = new char[len+1];
    strcpy(dataLogName, AUDIT_DATA_LOG_TEMPLATE);
    dataLogFile = mkstemp(dataLogName);
}

int CSysLogEventLogger::writeDataLog(size32_t datasize, byte const * data)
{
    CriticalBlock block(dataLogLock);
    off_t fpos = lseek(dataLogFile, 0, SEEK_CUR);
    while(datasize > 0)
    {
        ssize_t written = write(dataLogFile, data, datasize);
        if (written == -1)
            return -1;
        data += written;
        datasize -= written;
    }
#ifndef _WIN32
#ifdef F_FULLFSYNC
    fcntl(dataLogFile, F_FULLFSYNC);
#else
    fdatasync(dataLogFile);
#endif
#ifdef POSIX_FADV_DONTNEED
    posix_fadvise(dataLogFile, 0, 0, POSIX_FADV_DONTNEED);
#endif
#endif
    return fpos;
}

#endif

void SysLogMsgHandler::handleMessage(const LogMsg & msg)
{
    AuditType type = categoryToAuditType(msg.queryCategory());
    StringBuffer text;
    msg.toStringPlain(text, fields);
    logger->log(type, text.str());
}

void SysLogMsgHandler::addToPTree(IPropertyTree * tree) const
{
    IPropertyTree * handlerTree = createPTree(ipt_caseInsensitive);
    handlerTree->setProp("@type", "audit");
    tree->addPropTree("handler", handlerTree);
}

// Default implementations of the functions in IContextLogger interface

void IContextLogger::CTXLOG(const char *format, ...) const
{
    va_list args;
    va_start(args, format);
    CTXLOGva(MCdebugInfo, NoLogMsgCode, format, args);
    va_end(args);
}

void IContextLogger::mCTXLOG(const char *format, ...) const
{
    va_list args;
    va_start(args, format);
    StringBuffer log;
    log.limited_valist_appendf(1024*1024, format, args);
    va_end(args);

    const char *cursor = log;
    const char *lineStart = cursor;
    while (true)
    {
        switch (*cursor)
        {
            case '\0':
                CTXLOG("%.*s", (int)(cursor-lineStart), lineStart);
                return;
            case '\r':
                // NB: \r or \r\n translated into newline
                CTXLOG("%.*s", (int)(cursor-lineStart), lineStart);
                if ('\n' == *(cursor+1))
                    cursor++;
                lineStart = cursor+1;
                break;
            case '\n':
                CTXLOG("%.*s", (int)(cursor-lineStart), lineStart);
                lineStart = cursor+1;
                break;
        }
        ++cursor;
    }
}

void IContextLogger::logOperatorException(IException *E, const char *file, unsigned line, const char *format, ...) const
{
    va_list args;
    va_start(args, format);
    logOperatorExceptionVA(E, file, line, format, args);
    va_end(args);
}
class CRuntimeStatisticCollection;

/*
  This class is used to implement the default log context - especially used for engines that only support a single query at a time
  */

class DummyLogCtx : implements IContextLogger
{
private:
    Owned<ISpan> activeSpan = getNullSpan();

public:
    DummyLogCtx() {}
    // It's a static object - we don't want to actually link-count it...
    virtual void Link() const {}
    virtual bool Release() const { return false; }

    virtual void CTXLOGva(const LogMsgCategory & cat, LogMsgCode code, const char *format, va_list args) const override  __attribute__((format(printf,4,0)))
    {
        VALOG(cat, code, format, args);
    }
    virtual void logOperatorExceptionVA(IException *E, const char *file, unsigned line, const char *format, va_list args) const __attribute__((format(printf,5,0)))
    {
        StringBuffer ss;
        ss.append("ERROR");
        if (E)
            ss.append(": ").append(E->errorCode());
        if (file)
            ss.appendf(": %s(%d) ", sanitizeSourceFile(file), line);
        if (E)
            E->errorMessage(ss.append(": "));
        if (format)
            ss.append(": ").valist_appendf(format, args);
        LOG(MCoperatorProgress, "%s", ss.str());
    }
    virtual void noteStatistic(StatisticKind kind, unsigned __int64 value) const
    {
    }
    virtual void setStatistic(StatisticKind kind, unsigned __int64 value) const
    {
    }
    virtual void mergeStats(unsigned activityId, const CRuntimeStatisticCollection &from) const
    {
    }
    virtual unsigned queryTraceLevel() const
    {
        return 0;
    }
    virtual ISpan * queryActiveSpan() const override
    {
        return activeSpan;
    }
    virtual void setActiveSpan(ISpan * span) override
    {
        activeSpan.set(span);
    }
    virtual IProperties * getClientHeaders() const override
    {
        return ::getClientHeaders(activeSpan);
    }
    virtual IProperties * getSpanContext() const override
    {
        return ::getSpanContext(activeSpan);
    }
    virtual void setSpanAttribute(const char *name, const char *value) const override
    {
        activeSpan->setSpanAttribute(name, value);
    }
    virtual void setSpanAttribute(const char *name, __uint64 value) const override
    {
        activeSpan->setSpanAttribute(name, value);
    }
    virtual const char *queryGlobalId() const override
    {
        return activeSpan->queryGlobalId();
    }
    virtual const char *queryCallerId() const override
    {
        return activeSpan->queryCallerId();
    }
    virtual const char *queryLocalId() const override
    {
        return activeSpan->queryLocalId();
    }
    virtual const CRuntimeStatisticCollection &queryStats() const override
    {
        throwUnexpected();
    }
    virtual void recordStatistics(IStatisticGatherer &progress) const override
    {
        throwUnexpected();
    }
} dummyContextLogger;

extern jlib_decl const IContextLogger &queryDummyContextLogger()
{
    return dummyContextLogger;
}

extern jlib_decl IContextLogger &updateDummyContextLogger()
{
    return dummyContextLogger;
}


extern jlib_decl StringBuffer &appendGloballyUniqueId(StringBuffer &s)
{
    std::string uid = createUniqueIdString();
    return s.append(uid.c_str());
}

extern jlib_decl void UseSysLogForOperatorMessages(bool use)
{
    static ILogMsgHandler *msgHandler=NULL;
    if (use==(msgHandler!=NULL))
        return;
    if (use) {
        msgHandler = getSysLogMsgHandler();
        ILogMsgFilter * operatorFilter = getCategoryLogMsgFilter(MSGAUD_operator, MSGCLS_all, DefaultDetail, true);
        queryLogMsgManager()->addMonitorOwn(msgHandler, operatorFilter);
    }
    else {
        queryLogMsgManager()->removeMonitor(msgHandler);
        msgHandler = NULL;
    }
}

extern jlib_decl void AuditSystemAccess(const char *userid, bool success, char const * msg,...)
{
    va_list args;
    va_start(args, msg);
    VStringBuffer s("User %s: ", userid);
    SYSLOG((success) ? AUDIT_TYPE_ACCESS_SUCCESS : AUDIT_TYPE_ACCESS_FAILURE, s.valist_appendf(msg, args).str());
    va_end(args);
}

//--------------------------------------------------------------

class jlib_decl CComponentLogFileCreator : implements IComponentLogFileCreator, public CInterface
{
private:
    StringBuffer component;

    //filename parts
    StringBuffer prefix;
    StringBuffer name;
    StringBuffer postfix;
    StringBuffer extension;
    StringBuffer fullFileSpec;

    bool         createAlias;
    StringBuffer aliasName;

    StringBuffer logDirSubdir;

    bool         rolling;

    //ILogMsgHandler fields
    bool         append;
    bool         flushes;
    unsigned     msgFields;

    //ILogMsgFilter fields
    unsigned     msgAudiences;
    unsigned     msgClasses;
    LogMsgDetail maxDetail;
    bool         local;

    //available after logging started
    StringBuffer logDir;        //access via queryLogDir()
    StringBuffer aliasFileSpec; //access via queryAliasFileSpec()
    StringBuffer expandedLogSpec;//access via queryLogFileSpec()
    long         maxLogFileSize = 0;

private:
    void setDefaults()
    {
        rolling = true;
        append = true;
        flushes = true;
#ifdef _CONTAINERIZED
        const char *logFields = nullptr;
#else
        const char *logFields = queryEnvironmentConf().queryProp("logfields");
#endif
        if (!isEmptyString(logFields))
            msgFields = logMsgFieldsFromAbbrevs(logFields);
        else
            msgFields = MSGFIELD_STANDARD;
        msgAudiences = MSGAUD_all;
        msgClasses = MSGCLS_all;
        maxDetail = DefaultDetail;
        name.set(component); //logfile defaults to component name. Change via setName(), setPrefix() and setPostfix()
        extension.set(".log");
        local = true;
        createAlias = true;
    }

public:
    IMPLEMENT_IINTERFACE;
    CComponentLogFileCreator(IPropertyTree * _properties, const char *_component) : component(_component)
    {
        setDefaults();
        if (_properties && !getConfigurationDirectory(_properties->queryPropTree("Directories"), "log", _component, _properties->queryProp("@name"), logDir))
            _properties->getProp("@logDir", logDir);
    }

    CComponentLogFileCreator(const char *_logDir, const char *_component) : component(_component), logDir(_logDir)
    {
        setDefaults();
    }

    CComponentLogFileCreator(const char *_component) : component(_component)
    {
        setDefaults();
        if (!getConfigurationDirectory(NULL, "log", _component, _component, logDir))
        {
            appendCurrentDirectory(logDir,false);
        }
    }

    //set methods
    void setExtension(const char * _ext)     { extension.set(_ext); }
    void setPrefix(const char * _prefix)     { prefix.set(_prefix); }
    void setName(const char * _name)         { name.set(_name); }
    void setCompleteFilespec(const char * _fs){fullFileSpec.set(_fs); setExtension(NULL); setRolling(false);}
    void setPostfix(const char * _postfix)   { postfix.set(_postfix); }
    void setCreateAliasFile(bool _create)    { createAlias = _create; }
    void setAliasName(const char * _aliasName)   { aliasName.set(_aliasName); }
    void setLogDirSubdir(const char * _subdir)   { logDirSubdir.set(_subdir); }
    void setRolling(const bool _rolls)       { rolling = _rolls; }
    void setMaxLogFileSize( const long _size)    { maxLogFileSize = _size; }
    //ILogMsgHandler fields
    void setAppend(const bool _append)       { append = _append; }
    void setFlushes(const bool _flushes)     { flushes = _flushes; }
    void setMsgFields(const unsigned _fields){ msgFields = _fields; }

    //ILogMsgFilter fields
    void setMsgAudiences(const unsigned _audiences){ msgAudiences = _audiences; }
    void setMsgClasses(const unsigned _classes)    { msgClasses = _classes; }
    void setMaxDetail(const LogMsgDetail _maxDetail)  { maxDetail = _maxDetail; }
    void setLocal(const bool _local)               { local = _local; }

    //query methods (not valid until logging started)
    const char * queryLogDir() const        { return logDir.str(); }
    const char * queryLogFileSpec() const   { return expandedLogSpec.str(); }
    const char * queryAliasFileSpec() const { return aliasFileSpec.str(); }

    ILogMsgHandler * beginLogging()
    {
        //build directory path
        StringBuffer logFileSpec;
        if (!fullFileSpec.length())//user specify complete logfile specification?
        {
            if (!logDir.length())
            {
                appendCurrentDirectory(logDir,false).append(PATHSEPSTR).append("logs");
                OWARNLOG("No logfile directory specified - logs will be written locally to %s", logDir.str());
            }

            makeAbsolutePath(logDir);

            //build log file name (without date string or extension)
            StringBuffer logFileName;
            if (prefix.length())
                logFileName.append(prefix).append(".");
            logFileName.append(name);
            if (postfix.length())
                logFileName.append(".").append(postfix);

            //build log file spec
            if (logDirSubdir.length())
                logDir.append(PATHSEPCHAR).append(logDirSubdir);//user specified subfolder
            logFileSpec.append(logDir).append(PATHSEPCHAR).append(logFileName);

            //build alias file spec
            if (createAlias)
            {
                if (aliasName.length()==0)
                    aliasName.set(logFileName);
                aliasFileSpec.append(logDir).append(PATHSEPCHAR).append(aliasName).append(extension);
            }
        }
        else
            makeAbsolutePath(fullFileSpec);

        ILogMsgHandler * lmh;
        if (rolling)
        {
            lmh = getRollingFileLogMsgHandler(logFileSpec.str(), extension, msgFields, append, flushes, NULL, aliasFileSpec.str(), true, maxLogFileSize);
        }
        else
        {
            StringBuffer lfs;
            if (fullFileSpec.length())
                lfs.set(fullFileSpec);
            else
                lfs.set(logFileSpec.append(extension).str());
            lmh = getFileLogMsgHandler(lfs.str(), NULL, msgFields);
        }
        lmh->getLogName(expandedLogSpec);
        queryLogMsgManager()->addMonitorOwn( lmh, getCategoryLogMsgFilter(msgAudiences, msgClasses, maxDetail, local));
        return lmh;
    }
};

IComponentLogFileCreator * createComponentLogFileCreator(IPropertyTree * _properties, const char *_component)
{
    return new CComponentLogFileCreator(_properties, _component);
}

IComponentLogFileCreator * createComponentLogFileCreator(const char *_logDir, const char *_component)
{
    return new CComponentLogFileCreator(_logDir, _component);
}

IComponentLogFileCreator * createComponentLogFileCreator(const char *_component)
{
    return new CComponentLogFileCreator(_component);
}

ILogAccessFilter * getLogAccessFilterFromPTree(IPropertyTree * xml)
{
    if (xml == nullptr)
        throw makeStringException(-2,"getLogAccessFilterFromPTree: input tree cannot be null");

    StringBuffer type;
    xml->getProp("@type", type);
    if (streq(type.str(), "jobid"))
        return new FieldLogAccessFilter(xml, LOGACCESS_FILTER_jobid);
    else if (streq(type.str(), "audience"))
        return new FieldLogAccessFilter(xml, LOGACCESS_FILTER_audience);
    else if (streq(type.str(), "class"))
        return new FieldLogAccessFilter(xml, LOGACCESS_FILTER_class);
    else if (streq(type.str(), "component"))
        return new FieldLogAccessFilter(xml, LOGACCESS_FILTER_component);
    else if (streq(type.str(), "and"))
        return new BinaryLogAccessFilter(xml, LOGACCESS_FILTER_and);
    else if (streq(type.str(), "or"))
        return new BinaryLogAccessFilter(xml, LOGACCESS_FILTER_or);
    else
        throwUnexpectedX("getLogAccessFilterFromPTree : unrecognized LogAccessFilter type");
}

ILogAccessFilter * getWildCardLogAccessFilter(const char * wildcardfilter)
{
    return new FieldLogAccessFilter(wildcardfilter, LOGACCESS_FILTER_wildcard);
}

ILogAccessFilter * getWildCardLogAccessFilter()
{
    return new FieldLogAccessFilter("", LOGACCESS_FILTER_wildcard);
}

ILogAccessFilter * getHostLogAccessFilter(const char * host)
{
    return new FieldLogAccessFilter(host, LOGACCESS_FILTER_host);
}

ILogAccessFilter * getInstanceLogAccessFilter(const char * instancename)
{
    return new FieldLogAccessFilter(instancename, LOGACCESS_FILTER_instance);
}

ILogAccessFilter * getJobIDLogAccessFilter(const char * jobId)
{
    return new FieldLogAccessFilter(jobId, LOGACCESS_FILTER_jobid);
}

ILogAccessFilter * getTraceIDLogAccessFilter(const char * traceId)
{
    return new FieldLogAccessFilter(traceId, LOGACCESS_FILTER_trace);
}

ILogAccessFilter * getSpanIDLogAccessFilter(const char * spanId)
{
    return new FieldLogAccessFilter(spanId, LOGACCESS_FILTER_span);
}

ILogAccessFilter * getColumnLogAccessFilter(const char * columnName, const char * value)
{
    return new ColumnLogAccessFilter(columnName, value, LOGACCESS_FILTER_column);
}

ILogAccessFilter * getComponentLogAccessFilter(const char * component)
{
    return new FieldLogAccessFilter(component, LOGACCESS_FILTER_component);
}

ILogAccessFilter * getPodLogAccessFilter(const char * podName)
{
    return new FieldLogAccessFilter(podName, LOGACCESS_FILTER_pod);
}

ILogAccessFilter * getAudienceLogAccessFilter(MessageAudience audience)
{
    return new FieldLogAccessFilter(LogMsgAudienceToFixString(audience), LOGACCESS_FILTER_audience);
}

ILogAccessFilter * getClassLogAccessFilter(LogMsgClass logclass)
{
    return new FieldLogAccessFilter(LogMsgClassToFixString(logclass), LOGACCESS_FILTER_class);
}

ILogAccessFilter * getBinaryLogAccessFilter(ILogAccessFilter * arg1, ILogAccessFilter * arg2, LogAccessFilterType type)
{
    return new BinaryLogAccessFilter(arg1, arg2, type);
}

ILogAccessFilter * getBinaryLogAccessFilterOwn(ILogAccessFilter * arg1, ILogAccessFilter * arg2, LogAccessFilterType type)
{
    ILogAccessFilter * ret = new BinaryLogAccessFilter(arg1, arg2, type);
    if (arg1)
        arg1->Release();
    if (arg2)
        arg2->Release();
    return ret;
}

ILogAccessFilter* getCompoundLogAccessFilter(ILogAccessFilter* arg1, ILogAccessFilter* arg2, LogAccessFilterType type)
{
    if (!arg1)
        return LINK(arg2);
    if (!arg2)
        return LINK(arg1);
    return getBinaryLogAccessFilter(arg1, arg2, type);
}


// LOG ACCESS HELPER METHODS

// Fetches log entries - based on provided filter, via provided IRemoteLogAccess instance
bool fetchLog(LogQueryResultDetails & resultDetails, StringBuffer & returnbuf, IRemoteLogAccess & logAccess, ILogAccessFilter * filter, LogAccessTimeRange timeRange, const StringArray & cols, LogAccessLogFormat format)
{
    LogAccessConditions logFetchOptions;
    logFetchOptions.setTimeRange(timeRange);
    logFetchOptions.setFilter(filter);
    logFetchOptions.copyLogFieldNames(cols); //ensure these fields are declared in m_logMapping->queryProp("WorkUnits/@contentcolumn")? or in LogMap/Fields?"
    return logAccess.fetchLog(resultDetails, logFetchOptions, returnbuf, format);
}

// Fetches log entries based on provided JobID, via provided IRemoteLogAccess instance
bool fetchJobIDLog(LogQueryResultDetails & resultDetails,StringBuffer & returnbuf, IRemoteLogAccess & logAccess, const char *jobid, LogAccessTimeRange timeRange, StringArray & cols, LogAccessLogFormat format = LOGACCESS_LOGFORMAT_json)
{
    return fetchLog(resultDetails, returnbuf, logAccess, getJobIDLogAccessFilter(jobid), timeRange, cols, format);
}

// Fetches log entries based on provided component name, via provided IRemoteLogAccess instance
bool fetchComponentLog(LogQueryResultDetails & resultDetails, StringBuffer & returnbuf, IRemoteLogAccess & logAccess, const char * component, LogAccessTimeRange timeRange, StringArray & cols, LogAccessLogFormat format = LOGACCESS_LOGFORMAT_json)
{
    return fetchLog(resultDetails, returnbuf, logAccess, getComponentLogAccessFilter(component), timeRange, cols, format);
}

// Fetches log entries based on provided pod name, via provided IRemoteLogAccess instance
bool fetchPodLog(LogQueryResultDetails & resultDetails, StringBuffer & returnbuf, IRemoteLogAccess & logAccess, const char * podName, LogAccessTimeRange timeRange, StringArray & cols, LogAccessLogFormat format = LOGACCESS_LOGFORMAT_json)
{
    return fetchLog(resultDetails, returnbuf, logAccess, getPodLogAccessFilter(podName), timeRange, cols, format);
}

// Fetches log entries based on provided audience, via provided IRemoteLogAccess instance
bool fetchLogByAudience(LogQueryResultDetails & resultDetails, StringBuffer & returnbuf, IRemoteLogAccess & logAccess, MessageAudience audience, LogAccessTimeRange timeRange, StringArray & cols, LogAccessLogFormat format = LOGACCESS_LOGFORMAT_json)
{
    return fetchLog(resultDetails, returnbuf, logAccess, getAudienceLogAccessFilter(audience), timeRange, cols, format);
}

// Fetches log entries based on provided log message class, via provided IRemoteLogAccess instance
bool fetchLogByClass(LogQueryResultDetails & resultDetails, StringBuffer & returnbuf, IRemoteLogAccess & logAccess, LogMsgClass logclass, LogAccessTimeRange timeRange, StringArray & cols, LogAccessLogFormat format = LOGACCESS_LOGFORMAT_json)
{
    return fetchLog(resultDetails, returnbuf, logAccess, getClassLogAccessFilter(logclass), timeRange, cols, format);
}

//logAccessPluginConfig expected to contain connectivity and log mapping information
typedef IRemoteLogAccess * (*newLogAccessPluginMethod_t_)(IPropertyTree & logAccessPluginConfig);

IRemoteLogAccess *queryRemoteLogAccessor()
{
    return logAccessor.query([]
        {
            PROGLOG("Loading remote log access plug-in.");

            IRemoteLogAccess *remoteLogAccessor = nullptr;
            try
            {
                Owned<IPropertyTree> logAccessPluginConfig = getGlobalConfigSP()->getPropTree("logAccess");
#ifdef LOGACCESSDEBUG
                if (!logAccessPluginConfig)
                {
                    const char * simulatedGlobalYaml = R"!!(global:
  logAccess:
    name: "Grafana/loki stack log access"
    type: "GrafanaCurl"
    connection:
      #workspaceID: "ef060646-ef24-48a5-b88c-b1f3fbe40271"
      #workspaceID: "XYZ"      #ID of the Azure LogAnalytics workspace to query logs from
      #tenantID: "ABC"         #The Tenant ID, required for KQL API access
      #clientID: "DEF"         #ID of Azure Active Directory registered application with api.loganalytics.io access
      protocol: "http"
      host: "localhost"
      port: "3000"
    datasource:
      id: "1"
      name: "Loki"
    namespace:
      name: "hpcc"
                    )!!";
                    Owned<IPropertyTree> testTree = createPTreeFromYAMLString(simulatedGlobalYaml, ipt_none, ptr_ignoreWhiteSpace, nullptr);
                    logAccessPluginConfig.setown(testTree->getPropTree("global/logAccess"));
                }
#endif

                if (!logAccessPluginConfig)
                    throw makeStringException(-1, "RemoteLogAccessLoader: logaccess configuration not available!");

                constexpr const char * methodName = "queryRemoteLogAccessor";
                constexpr const char * instFactoryName = "createInstance";

                StringBuffer libName; //lib<type>logaccess.so
                StringBuffer type;
                logAccessPluginConfig->getProp("@type", type);
                if (type.isEmpty())
                    throw makeStringExceptionV(-1, "%s RemoteLogAccess plugin kind not specified.", methodName);
                libName.append("lib").append(type.str()).append("logaccess");

                //Load the DLL/SO
                HINSTANCE logAccessPluginLib = LoadSharedObject(libName.str(), false, true);

                newLogAccessPluginMethod_t_ xproc = (newLogAccessPluginMethod_t_)GetSharedProcedure(logAccessPluginLib, instFactoryName);
                if (xproc == nullptr)
                    throw makeStringExceptionV(-1, "%s cannot locate procedure %s in library '%s'", methodName, instFactoryName, libName.str());

                //Call logaccessplugin instance factory and return the new instance
                DBGLOG("Calling '%s' in log access plugin '%s'", instFactoryName, libName.str());
                remoteLogAccessor = xproc(*logAccessPluginConfig);
            }
            catch (IException *e)
            {
                EXCLOG(e, "Could not load remote log access plug-in: ");
                e->Release();
            }
            return remoteLogAccessor;
        }
    );
}

void setDefaultJobName(const char * name)
{
    setDefaultJobId(theManager->addJobId(name));
}


JobNameScope::JobNameScope(const char * name)
{
    set(name);
}

void JobNameScope::clear()
{
    if (id != UnknownJob)
    {
        theManager->removeJobId(id);
        id = prevId;
        setDefaultJobId(prevId);
    }
}

void JobNameScope::set(const char * name)
{
    clear();
    id = theManager->addJobId(name);
    prevId = queryThreadedJobId();
    setDefaultJobId(id);
}

//---------------------------------------------------------------------------------------------------------------------

TraceFlags loadTraceFlags(const IPropertyTree *ptree, const std::initializer_list<TraceOption> &optNames, TraceFlags dft)
{
    for (const TraceOption& option: optNames)
    {
        VStringBuffer attrName("@%s", option.name);
        const char* value = ptree->queryProp(attrName);
        if (!value)
        {
            attrName.setCharAt(0, '_');
            value = ptree->queryProp(attrName);
            if (!value)
                continue;
        }
        if (strieq(value, "default")) // allow a configuration to explicitly request a default value
            continue;
        if (option.value == traceDetail) // non-Boolean traceDetail
        {
            dft &= ~TraceFlags::LevelMask;
            if (strieq(value, "standard"))
                dft |= traceStandard;
            else if (strieq(value, "detailed"))
                dft |= traceDetailed;
            else if (strieq(value, "max"))
                dft |= traceMax;
            else
            {
                char* endptr = nullptr;
                unsigned tmp = strtoul(value, &endptr, 10);
                if (endptr && !*endptr && TraceFlags(tmp) <= TraceFlags::LevelMask)
                    dft |= TraceFlags(tmp);
            }
        }
        else if (option.value <= TraceFlags::LevelMask) // block individual trace level names
            continue;
        else // Boolean trace options
        {
            bool flag = strToBool(value);
            if (flag)
                dft |= option.value;
            else
                dft &= ~option.value;
        }
    }
    return dft;
}

void ctxlogReport(const LogMsgCategory & cat, const char * format, ...)
{
    va_list args;
    va_start(args, format);
    ctxlogReportVA(cat, NoLogMsgCode, format, args);
    va_end(args);
}

void ctxlogReportVA(const LogMsgCategory & cat, const char * format, va_list args)
{
    ctxlogReportVA(cat, NoLogMsgCode, format, args);
}
void ctxlogReport(const LogMsgCategory & cat, LogMsgCode code, const char * format, ...)
{
    va_list args;
    va_start(args, format);
    ctxlogReportVA(cat, code, format, args);
    va_end(args);
}
void ctxlogReportVA(const LogMsgCategory & cat, LogMsgCode code, const char * format, va_list args)
{
    if (queryThreadedContextLogger())
    {
        LogContextScope ls(nullptr);
        ls.prev->CTXLOGva(cat, code, format, args);
    }
    else
        queryLogMsgManager()->report_va(cat, code, format, args);
}
void ctxlogReport(const LogMsgCategory & cat, const IException * e, const char * prefix)
{
    StringBuffer buff;
    e->errorMessage(buff);
    ctxlogReport(cat, e->errorCode(), "%s%s%s", prefix ? prefix : "", prefix ? " : " : "", buff.str());
}
IException * ctxlogReport(IException * e, const char * prefix, LogMsgClass cls)
{
    ctxlogReport(MCexception(e, cls), e, prefix);
    return e;
}

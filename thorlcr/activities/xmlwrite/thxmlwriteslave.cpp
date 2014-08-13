/*##############################################################################

    HPCC SYSTEMS software Copyright (C) 2012 HPCC Systems.

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


#include "jio.hpp"
#include "jtime.hpp"
#include "jfile.ipp"
#include "thbuf.hpp"
#include "thexception.hpp"
#include "thbufdef.hpp"
#include "thmfilemanager.hpp"
#include "slave.ipp"
#include "thactivityutil.ipp"
#include "thorxmlwrite.hpp"
#include "thxmlwriteslave.ipp"

class CXmlWriteSlaveActivity : public CDiskWriteSlaveActivityBase
{
    IHThorXmlWriteArg *helper;
    ThorActivityKind kind;

public:
    CXmlWriteSlaveActivity(CGraphElementBase *container, ThorActivityKind _kind) : CDiskWriteSlaveActivityBase(container), kind(_kind)
    {
        helper = static_cast <IHThorXmlWriteArg *> (queryHelper());
    }
    virtual void write()
    {
        StringBuffer rowTag;
        OwnedRoxieString xmlpath(helper->getXmlIteratorPath());
        if (!xmlpath)
        {
            rowTag.append("Row");
        }
        else
        {
            const char *path = xmlpath;
            if (*path == '/') path++;
            if (strchr(path, '/')) UNIMPLEMENTED;
            rowTag.append(path);
        }

        StringBuffer out;
        Owned<IXmlWriterExt> writer = createIXmlWriterExt(helper->getXmlFlags(), 0, NULL, (kind==TAKjsonwrite) ? WTJSON : WTStandard);
        writer->outputBeginArray(rowTag); //need this to format rows, even if not outputting it below
        if (!dlfn.isExternal() || firstNode()) // if external, 1 header,footer
        {
            OwnedRoxieString header(helper->getHeader());
            if (header)
                out.set(header.get());
            else
            {
                if (kind==TAKjsonwrite)
                {
                    out.set("{");
                    appendJSONName(out, "Dataset").append('{');
                    appendJSONName(out, rowTag).append('[').newline();
                }
                else
                    out.set("<Dataset>").newline();
            }
            outraw->write(out.length(), out.toCharArray());
            if (writer->length())
            {
                outraw->write(writer->length(), writer->str());
                if (calcFileCrc)
                    fileCRC.tally(writer->length(), writer->str());
            }
            if (calcFileCrc)
                fileCRC.tally(out.length(), out.toCharArray());
        }
        while(!abortSoon)
        {
            OwnedConstThorRow row = input->ungroupedNextRow();
            if (!row)
                break;
            writer->clear().outputBeginNested(rowTag, false);
            helper->toXML((const byte *)row.get(), *writer);
            writer->outputEndNested(rowTag);
            outraw->write(writer->length(), writer->str());
            if (calcFileCrc)
                fileCRC.tally(writer->length(), writer->str());
            processed++;
        }
        writer->outputEndArray(rowTag); //set this, but don't output it
        writer->clear();
        if (!dlfn.isExternal() || lastNode()) // if external, 1 header,footer
        {
            OwnedRoxieString footer(helper->getFooter());
            if (footer)
                out.clear().append(footer);
            else
                out.clear().append((kind==TAKjsonwrite) ? "]}}" : "</Dataset>").newline();
            outraw->write(out.length(), out.toCharArray());
            if (calcFileCrc)
                fileCRC.tally(out.length(), out.toCharArray());
        }
    }
    virtual bool wantRaw() { return true; }
};

CActivityBase *createXmlWriteSlave(CGraphElementBase *container, ThorActivityKind kind)
{
    return new CXmlWriteSlaveActivity(container, kind);
}



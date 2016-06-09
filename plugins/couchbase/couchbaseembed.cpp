/*##############################################################################

    HPCC SYSTEMS software Copyright (C) 2016 HPCC Systems®.

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

#include "couchbaseembed.hpp"

#include "platform.h"
#include "jexcept.hpp"
#include "jlog.hpp"
//#include "jthread.hpp"
#include "hqlplugins.hpp"
#include "deftype.hpp"
#include "eclhelper.hpp"
#include "eclrtl.hpp"
#include "eclrtl_imp.hpp"
#include "rtlds_imp.hpp"
#include "rtlfield_imp.hpp"
//#include "rtlembed.hpp"
//#include "roxiemem.hpp"
//#include "nbcd.hpp"

//#include <event2/event.h>

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

static const char *g_moduleName = "couchbase";
static const char *g_moduleDescription = "Couchbase Embed Helper";
static const char *g_version = "Couchbase Embed Helper 1.0.0";
static const char *g_compatibleVersions[] = { g_version, nullptr };

COUCHBASEEMBED_PLUGIN_API bool getECLPluginDefinition(ECLPluginDefinitionBlock *pb)
{
    if (pb->size == sizeof(ECLPluginDefinitionBlockEx))
    {
        ECLPluginDefinitionBlockEx * pbx = (ECLPluginDefinitionBlockEx *) pb;
        pbx->compatibleVersions = g_compatibleVersions;
    }
    else if (pb->size != sizeof(ECLPluginDefinitionBlock))
        return false;

    pb->magicVersion = PLUGIN_VERSION;
    pb->version = g_version;
    pb->moduleName = g_moduleName;
    pb->ECL = NULL;
    pb->flags = PLUGIN_IMPLICIT_MODULE;
    pb->description = g_moduleDescription;
    return true;
}

namespace couchbaseembed
{
//--------------------------------------------------------------------------
   // Plugin Classes
   //--------------------------------------------------------------------------

    CouchbaseRowStream::CouchbaseRowStream(IEngineRowAllocator* resultAllocator, Couchbase::Query * cbaseQuery)
       :   m_CouchBaseQuery(cbaseQuery),
           m_resultAllocator(resultAllocator)
    {
        m_currentRow = 0;
        m_shouldRead = true;

        //iterating over result rows and copying them to stringarray
        //is there a way to independently step through original result rows?
        for (auto cbrow : *m_CouchBaseQuery)
            m_Rows.append(cbrow.json().data());

        if (m_CouchBaseQuery->meta().status().errcode() != LCB_SUCCESS )//rows.length() == 0)
            failx("Embedded couchbase error: %s", m_CouchBaseQuery->meta().body().data());
        else if (m_Rows.length() == 0) // Query errors not reported in meta.status, lets check for errors in meta body
        {
            if (strstr(m_CouchBaseQuery->meta().body().data(), "\"status\": \"errors\""))
                failx("Err: %s", m_CouchBaseQuery->meta().body().data());
        }
    }

     CouchbaseRowStream::~CouchbaseRowStream()
     {}

    const void * CouchbaseRowStream::nextRow()
    {
        const void * result = NULL;
        if (m_shouldRead && m_currentRow < m_Rows.length())
        {
            CouchbaseRowBuilder * cbRowBuilder = new CouchbaseRowBuilder(m_Rows.item(m_currentRow++));
            RtlDynamicRowBuilder rowBuilder(m_resultAllocator);

            const RtlTypeInfo *typeInfo = m_resultAllocator->queryOutputMeta()->queryTypeInfo();
            assertex(typeInfo);
            RtlFieldStrInfo dummyField("<row>", NULL, typeInfo);
            size32_t len = typeInfo->build(rowBuilder, 0, &dummyField, *cbRowBuilder);
            return rowBuilder.finalizeRowClear(len);
        }
        return result;
    }

   void CouchbaseRowStream::stop()
   {
       m_resultAllocator.clear();
       m_shouldRead = false;
   }

   CouchbaseRowStream * CouchbaseConnection::query(IEngineRowAllocator * allocator, const char * query)
   {
       Owned<CouchbaseRowStream> cbaseRowStream;
       if (m_connectionStatus.success())
       {
            if (query && *query)
            {
                Couchbase::Status queryStatus;
                Couchbase::QueryCommand qcmd(query);

                m_pQuery = new Couchbase::Query(*m_pCouchbaseClient, qcmd, queryStatus);

                if (!queryStatus)
                    failx("Couldn't issue query to: %s Reason: %s", m_connectionString.str(), queryStatus.description());

                if (!m_pQuery->status())
                    failx("Couldn't execute query, reason: %s\nBody is: ", m_pQuery->meta().body().data());

                cbaseRowStream.set(new CouchbaseRowStream(allocator, m_pQuery));
            }
            else
                failx("Empty N1QL query detected");
        }
        else
            failx("No Couchbase connection available");

       return cbaseRowStream.getLink();
   }

    COUCHBASEEMBED_PLUGIN_API IRowStream * COUCHBASEEMBED_PLUGIN_CALL executen1ql(ICodeContext * ctx, IEngineRowAllocator* allocator, const char * host, const char * bucket, const char * query)
    {
        Owned<CouchbaseConnection> connection = new CouchbaseConnection(host, bucket);

        connection->connect();

        return connection->query(allocator, query);
    }

    extern void UNSUPPORTED(const char *feature)
    {
        throw MakeStringException(-1, "UNSUPPORTED feature: %s not supported in %s", feature, g_version);
    }

    extern void failx(const char *message, ...)
    {
        va_list args;
        va_start(args,message);
        StringBuffer msg;
        msg.appendf("%s: ", g_moduleName).valist_appendf(message,args);
        va_end(args);
        rtlFail(0, msg.str());
    }

    extern void fail(const char *message)
    {
        StringBuffer msg;
        msg.appendf("%s: ", g_moduleName).append(message);
        rtlFail(0, msg.str());
    }

} // namespace

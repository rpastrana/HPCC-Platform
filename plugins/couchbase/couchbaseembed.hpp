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

#ifndef _COUCHBASEEMBED_INCL
#define _COUCHBASEEMBED_INCL

#ifdef _WIN32
#define COUCHBASEEMBED_PLUGIN_CALL _cdecl
#ifdef COUCHBASEEMBED_PLUGIN_EXPORTS
#define COUCHBASEEMBED_PLUGIN_API __declspec(dllexport)
#else
#define COUCHBASEEMBED_PLUGIN_API __declspec(dllimport)
#endif
#else
#define COUCHBASEEMBED_PLUGIN_CALL
#define COUCHBASEEMBED_PLUGIN_API
#endif

//Using cpp wrapper from https://github.com/couchbaselabs/libcouchbase-cxx
#include <libcouchbase/couchbase++.h>
#include <libcouchbase/couchbase++/views.h>
#include <libcouchbase/couchbase++/query.h>
#include <libcouchbase/couchbase++/endure.h>
#include <libcouchbase/couchbase++/logging.h>

#include "platform.h"
#include "jthread.hpp"
#include "hqlplugins.hpp"
#include "eclrtl_imp.hpp"
#include "eclhelper.hpp"
#include "rtlembed.hpp"

namespace couchbaseembed
{
    extern void UNSUPPORTED(const char *feature) __attribute__((noreturn));
    extern void failx(const char *msg, ...) __attribute__((noreturn))  __attribute__((format(printf, 1, 2)));
    extern void fail(const char *msg) __attribute__((noreturn));

    extern "C++"
    {
        COUCHBASEEMBED_PLUGIN_API IRowStream* COUCHBASEEMBED_PLUGIN_CALL executen1ql  (ICodeContext * _ctx, IEngineRowAllocator* allocator, const char * param1, const char * param2, const char * query);
    }

    class CouchbaseRowStream : public RtlCInterface, implements IRowStream
    {
    public:
        CouchbaseRowStream(IEngineRowAllocator* _resultAllocator, Couchbase::Query * cbaseQuery);
        virtual ~CouchbaseRowStream();

        RTLIMPLEMENT_IINTERFACE
        virtual const void* nextRow();
        virtual void stop();
    private:
        Couchbase::Query *              m_CouchBaseQuery;   //!< pointer to couchbase query (holds results and metadata)
        Linked<IEngineRowAllocator>     m_resultAllocator;    //!< Pointer to allocator used when building result rows
        bool                            m_shouldRead;         //!< If true, we should continue trying to read more messages
        StringArray                     m_Rows;                //!< Local copy of result rows
        __int64                            m_currentRow;       //!< Current result row
    };

    class CouchbaseConnection : public CInterface
    {
    public:
        inline CouchbaseConnection(const char * host, const char * bucket)
        {
            m_connectionString.setf("couchbase://%s/%s", (!host || !*host) ? "localhost" : host, (!bucket || !*bucket) ? "default" : bucket);
            m_pCouchbaseClient = new Couchbase::Client(m_connectionString.str());//USER/PASS still needed
            m_pQuery = nullptr;
        }

        inline ~CouchbaseConnection()
        {
        }

        inline void connect()
        {
            m_connectionStatus = m_pCouchbaseClient->connect();
            if (!m_connectionStatus.success())
                failx("Failed to connect to couchbase instance: %s Reason: %s", m_connectionString.str(), m_connectionStatus.description());
        }

        CouchbaseRowStream* query(IEngineRowAllocator* allocator, const char * query);

    private:
        StringBuffer m_connectionString;
        Couchbase::Client * m_pCouchbaseClient;
        Couchbase::Status  m_connectionStatus;
        Couchbase::Query  * m_pQuery;

        CouchbaseConnection(const CouchbaseConnection &);
    };

    /*Future
    class CouchbasePrepared : public CInterfaceOf<IInterface>
    {
        public:
            inline CouchbasePrepared(const char *_queryString) : queryString(_queryString)
            {
            }
            inline ~CouchbasePrepared()
            {
            }

            inline const char *queryQueryString() const
            {
                return queryString;
            }
        private:
            CouchbasePrepared(const CouchbasePrepared &);
            StringAttr queryString;
    };
*/
    class CouchbaseRowBuilder : public CInterfaceOf<IFieldSource>
    {
    public:
        CouchbaseRowBuilder(const char * row)
        {
            m_ResultRow.set(row);
        }

        virtual bool getBooleanResult(const RtlFieldInfo *field)
        {
            UNSUPPORTED("Embedded Couchbase support error: Boolean result encountered, only String based results supported");
        }
        virtual void getDataResult(const RtlFieldInfo *field, size32_t &len, void * &result)
        {
            UNSUPPORTED("Embedded Couchbase support error: Data result encountered, only String based results supported");
        }
        virtual double getRealResult(const RtlFieldInfo *field)
        {
            UNSUPPORTED("Embedded Couchbase support error: Real result encountered, only String based results supported");
        }
        virtual __int64 getSignedResult(const RtlFieldInfo *field)
        {
            UNSUPPORTED("Embedded Couchbase support error: Signed result encountered, only String based results supported");
        }
        virtual unsigned __int64 getUnsignedResult(const RtlFieldInfo *field)
        {
            UNSUPPORTED("getUnsignedResult");
            UNSUPPORTED("Embedded Couchbase support error: Boolean result encountered, only String based results supported");
        }
        virtual void getStringResult(const RtlFieldInfo *field, size32_t &chars, char * &result)
        {
            if (m_ResultRow.length() == 0)
            {
                NullFieldProcessor p(field);
                rtlStrToStrX(chars, result, p.resultChars, p.stringResult);
                return;
            }
            unsigned long bytes = m_ResultRow.length();
            unsigned numchars = rtlUtf8Length(bytes, m_ResultRow.str());  // MORE - is it a good assumption that it is utf8 ? Depends how the database is configured I think

            rtlUtf8ToStrX(chars, result, numchars, m_ResultRow.str());
            return;
        }
        virtual void getUTF8Result(const RtlFieldInfo *field, size32_t &chars, char * &result)
        {
            UNSUPPORTED("Embedded Couchbase support error: UTF8 result encountered, only String based results supported");
        }
        virtual void getUnicodeResult(const RtlFieldInfo *field, size32_t &chars, UChar * &result)
        {
            UNSUPPORTED("Embedded Couchbase support error: Unicode result encountered, only String based results supported");
        }
        virtual void getDecimalResult(const RtlFieldInfo *field, Decimal &value)
        {
            UNSUPPORTED("Embedded Couchbase support error: Decimal result encountered, only String based results supported");
        }
        virtual void processBeginSet(const RtlFieldInfo * field, bool &isAll)
        {
            UNSUPPORTED("Embedded Couchbase support error: processBeginSet() not supported");
        }
        virtual bool processNextSet(const RtlFieldInfo * field)
        {
            UNSUPPORTED("Embedded Couchbase support error: processNextSet() not supported");
            return false;
        }
        virtual void processBeginDataset(const RtlFieldInfo * field)
        {
            UNSUPPORTED("Embedded Couchbase support error: processBeginDataset() not supported");
        }
        virtual void processBeginRow(const RtlFieldInfo * field)
        {
        }
        virtual bool processNextRow(const RtlFieldInfo * field)
        {
            UNSUPPORTED("Embedded Couchbase support error: processNextRow() not supported");
            return false;
        }
        virtual void processEndSet(const RtlFieldInfo * field)
        {
            UNSUPPORTED("Embedded Couchbase support error: processEndSet() not supported");
        }
        virtual void processEndDataset(const RtlFieldInfo * field)
        {
            UNSUPPORTED("Embedded Couchbase support error: processEndDataset() not supported");
        }
        virtual void processEndRow(const RtlFieldInfo * field)
        {
        }
    private:
        StringBuffer m_ResultRow;
    };
} // couchbaseembed namespace
#endif

/*##############################################################################

    HPCC SYSTEMS software Copyright (C) 2016 HPCC Systems.

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


#ifndef _CASSANDRAESPCACHE_HPP_
#define _CASSANDRAESPCACHE_HPP_

#include "jmisc.hpp"
#include "eclhelper.hpp"
#include "cassandra.h"
#include "cassandraembed.hpp"
#include "espcacheclient.hpp"


using namespace cassandraembed;

#ifdef WIN32
    #ifdef CASSANDRAESPCACHE_EXPORTS
        #define CASSANDRAESPCACHE_API __declspec(dllexport)
    #else
        #define CASSANDRAESPCACHE_API __declspec(dllimport)
    #endif
#else
    #define CASSANDRAESPCACHE_API
#endif

class CCassandraEspCacheClient : public CEspCacheClientBase
{
    StringBuffer server, userID, password;
    Owned<CassandraClusterSession> session;
    CriticalSection transactionSeedCrit;

    void initKeySpace();
    void ensureDefaultKeySpace();
    void setSessionOptions(const char *keyspace);
    //void ensureTransSeedTable();
    void createTable(const char *dbName, const char *tableName, StringArray& columnNames, StringArray& columnTypes, const char* keys);
    void executeSimpleStatement(const char *st);
    //void executeSimpleStatement(StringBuffer& st);
    //bool executeSimpleSelectStatement(const char* st, unsigned& resultValue);
    void readDBCfg(IPropertyTree* cfg, StringBuffer& server, StringBuffer& dbUser, StringBuffer& dbPassword);

public:
    IMPLEMENT_IINTERFACE;

    virtual bool init(const char* name, const char* type, IPropertyTree* cfg, const char* process);
    virtual bool contains(const char * key);
    virtual void add(const char * key,  IEspCacheEntry * entry);
    virtual void remove(const char * key);
    virtual IEspCacheEntry * fetch (const char * key);

    CCassandraEspCacheClient() {};
    virtual ~CCassandraEspCacheClient()
    {
    //    logGroups.kill(); logSources.kill();
    };

};


extern "C"
{
    CASSANDRAESPCACHE_API IEspCacheClient * createEspCache()
    {
        return new CCassandraEspCacheClient();
    }
}

#endif /* _CASSANDRAESPCACHE_HPP_ */

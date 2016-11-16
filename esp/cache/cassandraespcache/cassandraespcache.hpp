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
    StringBuffer m_cassandraHost;
    StringBuffer m_cassandraUserID;
    StringBuffer m_cassandraUserPass;

    Owned<CassandraClusterSession> m_cassandraSession;
    //CriticalSection transactionSeedCrit;

    void initCassandraSession();
    void initCachTable(IPropertyTree * tableCfg);
    void initSession();
    void ensureDefaultKeySpace();
    void setSessionOptions();
    void ensureCacheTable();
    void createTable(const char *dbName, const char *tableName, StringArray& columnNames, StringArray& columnTypes, const char* keys, unsigned timeToLive);
    void executeSimpleStatement(const char *st);
    bool executeSimpleSelectStatement(const char* st);
    bool executeSimpleSelectStatement(const char* st, StringBuffer & resultValue);
    bool executeSimpleSelectStatement(const char* st, unsigned& resultValue);
    void readConnectionCfg(IPropertyTree* cfg);

public:
    IMPLEMENT_IINTERFACE;

    virtual bool init(const char* name, const char* type, IPropertyTree* cfg) override;
    virtual bool contains(IEspCacheSerializable & key) override;
    virtual void add(IEspCacheSerializable & key, IEspCacheSerializable & entry, unsigned int ttl=0) override;
    virtual void add(IEspCacheSerializable & key, IArrayOf<IEspCacheSerializable> & entries, unsigned int ttl=0);
    virtual void remove(IEspCacheSerializable & key) override;
    virtual IEspCacheSerializable * fetch (IEspCacheSerializable & key) override;
    //virtual unsigned getCacheTimeToLive() { return 0;}
    //virtual unsigned getEntryTimeToLive(IEspCacheSerializable & key) {return 0;}
    //virtual bool setEntryTileToLive(IEspCacheSerializable & key, unsigned ttl) {return false;}

    CCassandraEspCacheClient() {};
    virtual ~CCassandraEspCacheClient(){};
};

extern "C"
{
    CASSANDRAESPCACHE_API IEspCacheClient * createEspCache()
    {
        return new CCassandraEspCacheClient();
    }
}

#endif /* _CASSANDRAESPCACHE_HPP_ */

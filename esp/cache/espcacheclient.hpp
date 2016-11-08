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

#ifndef _ESPCACHECLIENT_HPP_
#define _ESPCACHECLIENT_HPP_

#include "jiface.hpp"
#include "tokenserialization.hpp"

interface IEspCacheEntry : extends IInterface
{
     //TokenDeserializer m_tokenDeserializer;
     //TokenSerializer m_tokenSerializer;
    template <typename TValue>
    StringBuffer& serialize(const TValue& value, StringBuffer& buffer);
    virtual DeserializationResult deserialize(const char* buffer, bool& value) const =0;
};

interface IEspCacheClient : extends IInterface
{
    virtual bool init(const char * name, const char * type, IPropertyTree * cfg, const char * process) = 0;
    virtual bool contains(const char * key) = 0;
    virtual void add(const char * key,  IEspCacheEntry * entry) = 0;
    virtual void remove(const char * key) = 0;
    virtual IEspCacheEntry * fetch (const char * key) = 0;
};

//class LOGGINGCOMMON_API CDBLogAgentBase : public CInterface, implements IEspLogAgent
class CEspCacheClientBase : public CInterface, implements IEspCacheClient
{
protected:
    StringBuffer cacheName, defaultTransactionTable, transactionTable, loggingTransactionSeed;
    StringAttr defaultLogGroup, defaultTransactionApp, loggingTransactionApp, logSourcePath;

    unsigned logSourceCount, loggingTransactionCount, maxTriesGTS;
    //MapStringToMyClass<CLogGroup> logGroups;
    //MapStringToMyClass<CLogSource> logSources;

public:
    IMPLEMENT_IINTERFACE;

    virtual bool init(const char * name, const char * type, IPropertyTree * cfg, const char * process){return false;}
    virtual bool contains(const char * key){return false;}
    virtual void add(const char * key,  IEspCacheEntry * entry){return;}
    virtual void remove(const char * key){return;}
    virtual IEspCacheEntry * fetch (const char * key){ return nullptr;}

    CEspCacheClientBase() {};
    virtual ~CEspCacheClientBase() {};
};
#endif /* _ESPCACHECLIENT_HPP_ */

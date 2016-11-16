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

interface IEspCacheSerializable : extends IInterface
{
	virtual const char * str() = 0;
};

class CEspCacheKey : public CInterface, implements IEspCacheSerializable
{
private:
	StringBuffer m_buffer;

public:
    IMPLEMENT_IINTERFACE;

    CEspCacheKey() {}
    CEspCacheKey(const char * key)
	{
		set(key);
	}

    virtual ~CEspCacheKey(){}

    void set(const char * value)
	{
		m_buffer.set(value);
	}

	const char * str()
	{
		return m_buffer.str();
	}
};

class CEspCacheValue : public CInterface, implements IEspCacheSerializable
{
private:
	StringBuffer m_buffer;

public:
    IMPLEMENT_IINTERFACE;

    CEspCacheValue() {}
    CEspCacheValue(const char * entry)
    {
    	set(entry);
    }

    virtual ~CEspCacheValue(){}

	void set(const char * value)
    {
		m_buffer.set(value);
    }

    const char * str()
	{
		return m_buffer.str();
	}
};

interface IEspCacheClient : extends IInterface
{
    virtual bool init(const char * name, const char * type, IPropertyTree * cfg) = 0;
    virtual bool contains(IEspCacheSerializable & key) = 0;
    //virtual void add(const char * key,  IEspCacheEntry * entry) = 0;
    virtual void add(IEspCacheSerializable & key, IEspCacheSerializable & entry, unsigned int ttl=0)=0;
    virtual void add(IEspCacheSerializable & key, IArrayOf<IEspCacheSerializable> & entries, unsigned int ttl=0)=0;
    virtual void remove(IEspCacheSerializable & key) = 0;
    virtual IEspCacheSerializable * fetch (IEspCacheSerializable & key) = 0;
    virtual unsigned getCacheTimeToLive() = 0;
    virtual unsigned getEntryTimeToLive(IEspCacheSerializable & key) = 0;
    virtual bool setEntryTileToLive(IEspCacheSerializable & key, unsigned ttl) = 0;
};

class CEspCacheClientBase : public CInterface, implements IEspCacheClient
{
protected:
    StringBuffer m_cacheName;
    StringBuffer m_tableName;
    unsigned m_cacheWideTimeToLive;

public:
    IMPLEMENT_IINTERFACE;

    virtual bool init(const char * name, const char * type, IPropertyTree * cfg){return false;}
    virtual bool contains(IEspCacheSerializable & key){return false;}
    //virtual void add(IEspCacheKey & key, IEspCacheEntry & entry);
    virtual void add(IEspCacheSerializable & key, IEspCacheSerializable & entry, unsigned int ttl=0){return;}
    virtual void add(IEspCacheSerializable & key, IArrayOf<IEspCacheSerializable> & entries, unsigned int ttl=0){return;}
    virtual void remove(IEspCacheSerializable & key){return;}
    virtual IEspCacheSerializable * fetch (IEspCacheSerializable & key){ return nullptr;}
    virtual unsigned getCacheTimeToLive() { return 0;}
    virtual unsigned getEntryTimeToLive(IEspCacheSerializable & key) {return 0;}
    virtual bool setEntryTileToLive(IEspCacheSerializable & key, unsigned ttl) {return false;}

    CEspCacheClientBase() {};
    virtual ~CEspCacheClientBase() {};
};

typedef IEspCacheClient * (*newCacheClient_t_)();
class EspCacheLoader
{
public:
	static IEspCacheClient* loadCacheClient(const char* name, const char* dll, const char* service)
	{
	    StringBuffer plugin;
	    plugin.append(SharedObjectPrefix).append(dll).append(SharedObjectExtension);
	    HINSTANCE cacheClientLib = LoadSharedObject("/opt/HPCCSystems/lib/libcassandraespcache.so", true, false);
	    if(!cacheClientLib)
	        throw MakeStringException(-1, "can't load library %s", plugin.str());

	    newCacheClient_t_ xproc = (newCacheClient_t_)GetSharedProcedure(cacheClientLib, "createEspCache");
	    if (!xproc)
	        throw MakeStringException(-1, "procedure createEspCache of %s can't be loaded", plugin.str());

	    return (IEspCacheClient*) xproc();
	}
};
#endif /* _ESPCACHECLIENT_HPP_ */

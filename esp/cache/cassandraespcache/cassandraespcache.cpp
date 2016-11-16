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

#include "cassandraespcache.hpp"

#include "esp.hpp"
#include "espcontext.hpp"
#include "eclhelper.hpp"
#include "deftype.hpp"
#include "eclhelper.hpp"
#include "eclrtl.hpp"
#include "eclrtl_imp.hpp"

static void setOption(StringArray& opts, const char* opt, const char* val)
{
    if (opt && *opt && val)
    {
        VStringBuffer optstr("%s=%s", opt, val);
        opts.append(optstr);
    }
}

static const CassValue* getSingleResult(const CassResult* result)
{
    const CassRow* row = cass_result_first_row(result);
    return row ? cass_row_get_column(row, 0) : NULL;
}

static void ensureInputString(const char* input, bool lowerCase, StringBuffer& outputStr, int code, const char* msg)
{
    outputStr.set(input).trim();
    if (outputStr.isEmpty())
        throw MakeStringException(code, "%s", msg);
    if (lowerCase)
        outputStr.toLowerCase();
}

void CCassandraEspCacheClient::readConnectionCfg(IPropertyTree* cfg)
{
	if(cfg)
	{
		m_cassandraHost.set(cfg->queryProp("server"));
		if (m_cassandraHost.length() == 0)
		{
			m_cassandraHost.set("localhost");
			ESPLOG(LogNormal, "ESP Cache: Target Cassandra server not specified, defaulting to  LOCALHOST!");
		}

		int cassandraServerPort = cfg->getPropInt("port", 9042);

		m_cassandraUserID.set(cfg->queryProp("@user"));
		if (m_cassandraUserID.length() == 0)
		{
			ESPLOG(LogNormal, "ESP Cache: Target Cassandra USER not specified!");
		}

		m_cassandraUserPass.set(cfg->queryProp("@password"));
		if (m_cassandraUserPass.length() == 0)
		{
			ESPLOG(LogNormal, "ESP Cache: Target Cassandra PASSWORD not specified!");
		}
	}
	else
		throw MakeStringException(-1, "Unable to find configuration for Cassandra based ESP Cache client");


}

/*void CCassandraEspCacheClient::readCacheLayoutCfg(IPropertyTree* cfg)
{
}*/

bool CCassandraEspCacheClient::init(const char* name, const char* type, IPropertyTree* cfg)
{
    if (!name || !*name || !type || !*type)
        throw MakeStringException(-1, "Name or type not specified for Cassandra based ESP Cache client");

    readConnectionCfg(cfg);

    //readCacheLayoutCfg(cfg->queryBranch("cache"));
    IPropertyTree* cachecfg = cfg->queryBranch("cache");
    m_cacheName.set(cachecfg->queryProp("@name"));


    //IPropertyTree* tablecfg = cachecfg->queryBranch("table"); //RODRIGO could we support multiple tables??? nah
    //Init Cassandra session and connect
    initSession();

    //ensure defaultDB
    ensureDefaultKeySpace();

    //cachecfg->queryBranch("table")
    initCachTable(cachecfg->queryBranch("table"));
    return true;
}

void CCassandraEspCacheClient::initCachTable(IPropertyTree * tablecfg)
{
	StringBuffer queryCacheTableKeys;
	StringArray queryCacheTableColumnNames, queryCacheTableColumnTypes;

	queryCacheTableColumnNames.append("wuid");
	queryCacheTableColumnTypes.append("varchar");
	queryCacheTableColumnNames.append("sqlquery");
	queryCacheTableColumnTypes.append("varchar");
	queryCacheTableKeys.set("sqlquery"); //primary keys

	m_tableName.set(tablecfg->queryProp("@name"));
	m_cacheWideTimeToLive = tablecfg->getPropInt("@ttl", 0);

	createTable(m_cacheName.str(), m_tableName.str(), queryCacheTableColumnNames, queryCacheTableColumnTypes, queryCacheTableKeys.str(), m_cacheWideTimeToLive);


	VStringBuffer st("SELECT %s FROM %s.%s LIMIT 1;", queryCacheTableKeys.str(), m_cacheName.str(), m_tableName.str());
	if (! executeSimpleSelectStatement(st.str()))
	{
		st.setf("INSERT INTO %s (sqlquery, wuid) values ( 'select * from myhpcctable', 'W20160101-010101');", m_tableName.str());
		executeSimpleStatement(st.str());
	}
	//m_cassandraSession->disconnect();
}


bool CCassandraEspCacheClient::contains(IEspCacheSerializable & key)
{
	//m_cassandraSession->connect();
	VStringBuffer st("SELECT sqlquery FROM %s.%s where sqlquery = '%s' LIMIT 1;", m_cacheName.str(), m_tableName.str(), key.str());
	bool success = executeSimpleSelectStatement(st.str());
	//m_cassandraSession->disconnect();
    return success;
}

void CCassandraEspCacheClient::add(IEspCacheSerializable & key, IEspCacheSerializable & entry, unsigned int ttl)
{
	bool success = false;
	const char * entryvalue = entry.str();
	if (entryvalue && *entryvalue)
	{
		StringBuffer usingoption;
		if (ttl > 0)
			usingoption.setf("USING TTL %d", ttl);

		VStringBuffer statement("INSERT INTO %s.%s (sqlquery, wuid) values ('%s', '%s') %s;", m_cacheName.str(), m_tableName.str(), key.str(), entryvalue, usingoption.str());
		//m_cassandraSession->connect();
		success = executeSimpleSelectStatement(statement.str());
		//m_cassandraSession->disconnect();
	}
}

void CCassandraEspCacheClient::add(IEspCacheSerializable & key, IArrayOf<IEspCacheSerializable> & entries, unsigned int ttl)
{
	bool success = false;
	if (entries.length())
	{
		StringBuffer usingoption;
		if (ttl > 0)
			usingoption.setf("USING TTL %d", ttl);

		const char * entryvalue = entries.item(0).str();
		VStringBuffer statement("INSERT INTO %s.%s (sqlquery, wuid) values (%s, %s)) %s ;", m_cacheName.str(), m_tableName.str(), key.str(), entryvalue, usingoption.str());
		//m_cassandraSession->connect();
		success = executeSimpleSelectStatement(statement.str());
		//m_cassandraSession->disconnect();
	}
    //return success;
}

void CCassandraEspCacheClient::remove(IEspCacheSerializable & key)
{
	bool success = false;
	VStringBuffer statement("DELETE from %s.%s where sqlquery = '%s';", m_cacheName.str(), m_tableName.str(), key.str());
	//m_cassandraSession->connect();
	success = executeSimpleSelectStatement(statement.str());
	//m_cassandraSession->disconnect();
    return;
}

IEspCacheSerializable * CCassandraEspCacheClient::fetch (IEspCacheSerializable & key)
{
	bool success = false;
	VStringBuffer statement("SELECT wuid FROM %s.%s where sqlquery = '%s' LIMIT 1;", m_cacheName.str(), m_tableName.str(), key.str());
	StringBuffer response;
	//m_cassandraSession->connect();
	success = executeSimpleSelectStatement(statement.str(), response);
	//m_cassandraSession->disconnect();

	if (success)
	{
		Owned<CEspCacheValue> resp = new CEspCacheValue(response.str());
		return resp.getLink();
	}

	return nullptr;
}

void CCassandraEspCacheClient::ensureCacheTable()
{
	CassandraSession s(cass_session_new());
	CassandraFuture future1(cass_session_connect(s, m_cassandraSession->queryCluster()));
	future1.wait("connect without keyspace");

	VStringBuffer st("CREATE KEYSPACE IF NOT EXISTS %s WITH replication "
					 "= { 'class': 'SimpleStrategy', 'replication_factor': '1' };",
					 m_cacheName.str());

	CassandraStatement statement(cass_statement_new(st.str(), 0));
	CassandraFuture future2(cass_session_execute(s, statement));
	future2.wait("execute");

	s.set(NULL);
}

void CCassandraEspCacheClient::initSession()
{
    //Initialize Cassandra Cluster Session
    m_cassandraSession.setown(new CassandraClusterSession(cass_cluster_new()));
    if (!m_cassandraSession)
        throw MakeStringException(-1,"Unable to create cassandra cassSession session");

    setSessionOptions();
    m_cassandraSession->connect();
}

void CCassandraEspCacheClient::ensureDefaultKeySpace()
{
	CassandraSession s(cass_session_new());
	CassandraFuture future1(cass_session_connect(s, m_cassandraSession->queryCluster()));
	future1.wait("connect without keyspace");

	VStringBuffer st("CREATE KEYSPACE IF NOT EXISTS %s WITH replication "
					 "= { 'class': 'SimpleStrategy', 'replication_factor': '1' };",
					 m_cacheName.str());

	CassandraStatement statement(cass_statement_new(st.str(), 0));
	CassandraFuture future2(cass_session_execute(s, statement));
	future2.wait("execute");

	s.set(NULL);
}

void CCassandraEspCacheClient::setSessionOptions()
{
    StringArray opts;
    setOption(opts, "contact_points", m_cassandraHost.str());
    if (!m_cassandraUserID.isEmpty())
    {
        setOption(opts, "user", m_cassandraUserID.str());
        if (!m_cassandraUserPass.isEmpty())
            setOption(opts, "password", m_cassandraUserPass.str());
    }
    if (m_cacheName.length())
        setOption(opts, "keyspace", m_cacheName.str());
    m_cassandraSession->setOptions(opts);
}

void CCassandraEspCacheClient::createTable(const char *dbName, const char *tableName, StringArray& columnNames, StringArray& columnTypes, const char* keys, unsigned timeToLive)
{
    StringBuffer fields;
    ForEachItemIn(i, columnNames)
        fields.appendf("%s %s,", columnNames.item(i), columnTypes.item(i));

    VStringBuffer createTableSt("CREATE TABLE IF NOT EXISTS %s.%s (%s PRIMARY KEY (%s)) WITH default_time_to_live = %d;", dbName, tableName, fields.str(), keys, timeToLive);
    executeSimpleStatement(createTableSt.str());
}

void CCassandraEspCacheClient::executeSimpleStatement(const char* st)
{
    CassandraStatement statement(m_cassandraSession->prepareStatement(st, getEspLogLevel()>LogNormal));
    CassandraFuture future(cass_session_execute(m_cassandraSession->querySession(), statement));
    future.wait("execute");
}

bool CCassandraEspCacheClient::executeSimpleSelectStatement(const char* st)
{
	//m_cassandraSession->connect();

    CassandraStatement statement(m_cassandraSession->prepareStatement(st, getEspLogLevel()>LogNormal));
    CassandraFuture future(cass_session_execute(m_cassandraSession->querySession(), statement));
    future.wait("execute");
    CassandraResult result(cass_future_get_result(future));

	//m_cassandraSession->disconnect();

    if (cass_result_row_count(result) == 0)
        return false;

    return true;
}

bool CCassandraEspCacheClient::executeSimpleSelectStatement(const char* st, StringBuffer & resultValue)
{
	//m_cassandraSession->connect();

    CassandraStatement statement(m_cassandraSession->prepareStatement(st, getEspLogLevel()>LogNormal));
    CassandraFuture future(cass_session_execute(m_cassandraSession->querySession(), statement));
    future.wait("execute");
    CassandraResult result(cass_future_get_result(future));
    if (cass_result_row_count(result) == 0)
        return false;

    size32_t chars;
    rtlDataAttr tempStr;
        //cassandraembed::getStringResult(NULL, getSingleResult(result), chars, tempStr.refstr());
        //resultValue.setString(chars, tempStr.getstr());

    getStringResult(NULL, getSingleResult(result), chars, tempStr.refstr());
    resultValue.set(tempStr.getstr());
    //resultValue = getUnsignedResult(NULL, getSingleResult(result));
    //m_cassandraSession->disconnect();
    return true;
}

bool CCassandraEspCacheClient::executeSimpleSelectStatement(const char* st, unsigned& resultValue)
{

	//m_cassandraSession->connect();

    CassandraStatement statement(m_cassandraSession->prepareStatement(st, getEspLogLevel()>LogNormal));
    CassandraFuture future(cass_session_execute(m_cassandraSession->querySession(), statement));
    future.wait("execute");
    CassandraResult result(cass_future_get_result(future));
    //m_cassandraSession->disconnect();

    if (cass_result_row_count(result) == 0)
        return false;

    resultValue = getUnsignedResult(NULL, getSingleResult(result));
    return true;
}

/*##############################################################################

    HPCC SYSTEMS software Copyright (C) 2023 HPCC Systems®.

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

#include "GrafanaCurlClient.hpp"

#include "platform.h"
#include <curl/curl.h>

static constexpr const char * defaultTSName = "TimeGenerated";
static constexpr const char * defaultIndexPattern = "ContainerLog";

static constexpr const char * defaultHPCCLogSeqCol         = "hpcc_log_sequence";
static constexpr const char * defaultHPCCLogTimeStampCol   = "hpcc_log_timestamp";
static constexpr const char * defaultHPCCLogProcIDCol      = "hpcc_log_procid";
static constexpr const char * defaultHPCCLogThreadIDCol    = "hpcc_log_threadid";
static constexpr const char * defaultHPCCLogMessageCol     = "hpcc_log_message";
static constexpr const char * defaultHPCCLogJobIDCol       = "hpcc_log_jobid";
static constexpr const char * defaultHPCCLogComponentCol   = "hpcc_log_component";
static constexpr const char * defaultHPCCLogTypeCol        = "hpcc_log_class";
static constexpr const char * defaultHPCCLogAudCol         = "hpcc_log_audience";
static constexpr const char * defaultHPCCLogComponentTSCol = "TimeGenerated";

static constexpr const char * logMapIndexPatternAtt = "@storeName";
static constexpr const char * logMapSearchColAtt = "@searchColumn";
static constexpr const char * logMapTimeStampColAtt = "@timeStampColumn";
static constexpr const char * logMapKeyColAtt = "@keyColumn";
static constexpr const char * logMapDisableJoinsAtt = "@disableJoins";

class GrafanaStream : public CInterfaceOf<IRemoteLogAccessStream>
{
public:
    virtual bool readLogEntries(StringBuffer & record, unsigned & recsRead) override
    {
        LogQueryResultDetails  resultDetails;
        m_remoteLogAccessor->fetchLog(resultDetails, m_options, record, m_outputFormat);
        recsRead = resultDetails.totalReceived;

        return false;
    }

    GrafanaStream(IRemoteLogAccess * azureQueryClient, const LogAccessConditions & options, LogAccessLogFormat format, unsigned int pageSize)
    {
        m_remoteLogAccessor.set(azureQueryClient);
        m_outputFormat = format;
        m_pageSize = pageSize;
        m_options = options;
    }

private:
    unsigned int m_pageSize;
    bool m_hasBeenScrolled = false;
    LogAccessLogFormat m_outputFormat;
    LogAccessConditions m_options;
    Owned<IRemoteLogAccess> m_remoteLogAccessor;
};

size_t stringCallback(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

static void issueQuery(std::string & readBuffer, const char * query, const char * connectStr, const char * user, const char * pass)
{
    if (isEmptyString(query))
        throw makeStringExceptionV(-1, "%s issueQuery: Empty query string detected!", COMPONENT_NAME);

    if (isEmptyString(connectStr))
        throw makeStringExceptionV(-1, "%s issueQuery: Empty connection string detected!", COMPONENT_NAME);

    if (isEmptyString(user))
        throw makeStringExceptionV(-1, "%s issueQuery: Empty user detected!", COMPONENT_NAME);

    if (isEmptyString(pass))
        throw makeStringExceptionV(-1, "%s issueQuery: Empty password detected!", COMPONENT_NAME);

    OwnedPtrCustomFree<CURL, curl_easy_cleanup> curlHandle = curl_easy_init();
    if (curlHandle)
    {
        CURLcode                curlResponseCode;
        OwnedPtrCustomFree<curl_slist, curl_slist_free_all> headers = nullptr;
        char                    curlErrBuffer[CURL_ERROR_SIZE];
        curlErrBuffer[0] = '\0';

        //char * encodedKQL = curl_easy_escape(curlHandle, kql, strlen(kql));
        //VStringBuffer tokenRequestURL("https://api.loganalytics.io/v1/workspaces/%s/query?query=%s", workspaceID, encodedKQL);
        VStringBuffer dsRequestURL("%s/api/datasources/proxy/1/loki/api/v1/query_range?%s", connectStr, "direction%3DBACKWARD%26limit%3D1");
  //curl -G -H "Content-Type: application/json" 
  //api/datasources/proxy/1/loki/api/v1/query_range?direction%3DBACKWARD%26limit%3D1

        VStringBuffer bAuth("%s:%s", user, pass);

        curl_easy_setopt(curlHandle, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(curlHandle, CURLOPT_USERPWD, bAuth.str());
        //headers = curl_slist_append(headers, bearerHeader.str());


        char * encodedKQL = curl_easy_escape(curlHandle, query, strlen(query));
        //VStringBuffer tokenRequestURL("https://api.loganalytics.io/v1/workspaces/%s/query?query=%s", workspaceID, encodedKQL);
        curl_free(encodedKQL);
        //if (curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, headers.getClear()) != CURLE_OK)
        //    throw makeStringExceptionV(-1, "%s: Log query request: Could not set 'CURLOPT_HTTPHEADER'", COMPONENT_NAME);

        if (curl_easy_setopt(curlHandle, CURLOPT_URL, dsRequestURL.str()) != CURLE_OK)
            throw makeStringExceptionV(-1, "%s: datasources request: Could not set 'CURLOPT_URL' (%s)!", COMPONENT_NAME, dsRequestURL.str());

        if (curl_easy_setopt(curlHandle, CURLOPT_POST, 0) != CURLE_OK)
            throw makeStringExceptionV(-1, "%s: Log query request: Could not disable 'CURLOPT_POST' option!", COMPONENT_NAME);

        if (curl_easy_setopt(curlHandle, CURLOPT_HTTPGET, 1) != CURLE_OK)
            throw makeStringExceptionV(-1, "%s: Log query request: Could not set 'CURLOPT_HTTPGET' option!", COMPONENT_NAME);

        if (curl_easy_setopt(curlHandle, CURLOPT_NOPROGRESS, 1) != CURLE_OK)
            throw makeStringExceptionV(-1, "%s: Log query request: Could not set 'CURLOPT_NOPROGRESS' option!", COMPONENT_NAME);

        if (curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, stringCallback) != CURLE_OK)
            throw makeStringExceptionV(-1, "%s: Log query request: Could not set 'CURLOPT_WRITEFUNCTION' option!", COMPONENT_NAME);

        if (curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &readBuffer) != CURLE_OK)
            throw makeStringExceptionV(-1, "%s: Log query request: Could not set 'CURLOPT_WRITEDATA' option!", COMPONENT_NAME);

        if (curl_easy_setopt(curlHandle, CURLOPT_USERAGENT, "HPCC Systems Log Access client") != CURLE_OK)
            throw makeStringExceptionV(-1, "%s: Log query request: Could not set 'CURLOPT_USERAGENT' option!", COMPONENT_NAME);

        if (curl_easy_setopt(curlHandle, CURLOPT_ERRORBUFFER, curlErrBuffer) != CURLE_OK)
            throw makeStringExceptionV(-1, "%s: Log query request: Could not set 'CURLOPT_ERRORBUFFER' option!", COMPONENT_NAME);

        if (curl_easy_setopt(curlHandle, CURLOPT_FAILONERROR, 1L) != CURLE_OK) // non HTTP Success treated as error
            throw makeStringExceptionV(-1, "%s: Log query request: Could not set 'CURLOPT_FAILONERROR'option!", COMPONENT_NAME);

        try
        {
            curlResponseCode = curl_easy_perform(curlHandle);
        }
        catch (...)
        {
            throw makeStringExceptionV(-1, "%s KQL request: Unknown libcurl error", COMPONENT_NAME);
        }

        if (curlResponseCode != CURLE_OK)
        {
            long response_code;
            curl_easy_getinfo(curlHandle, CURLINFO_RESPONSE_CODE, &response_code);

            StringBuffer message;
            switch (response_code)
            {

            default:
                throw makeStringExceptionV(-1,"%s KQL request: Error (%d): '%s'", COMPONENT_NAME, curlResponseCode, (curlErrBuffer[0] ? curlErrBuffer : "Unknown Error"));
            }
        }
        else if (readBuffer.length() == 0)
            throw makeStringExceptionV(-1, "%s KQL request: Empty response!", COMPONENT_NAME);
    }
}

static void getDataSources(std::string & readBuffer, const char * connectStr, const char * user, const char * pass)
{
    if (isEmptyString(connectStr))
        throw makeStringExceptionV(-1, "%s getDataSources request: Empty connection string detected!", COMPONENT_NAME);

    if (isEmptyString(user))
        throw makeStringExceptionV(-1, "%s getDataSources request: Empty user detected!", COMPONENT_NAME);

    if (isEmptyString(pass))
        throw makeStringExceptionV(-1, "%s getDataSources request: Empty password detected!", COMPONENT_NAME);

    OwnedPtrCustomFree<CURL, curl_easy_cleanup> curlHandle = curl_easy_init();
    if (curlHandle)
    {
        CURLcode                curlResponseCode;
        OwnedPtrCustomFree<curl_slist, curl_slist_free_all> headers = nullptr;
        char                    curlErrBuffer[CURL_ERROR_SIZE];
        curlErrBuffer[0] = '\0';

        //char * encodedKQL = curl_easy_escape(curlHandle, kql, strlen(kql));
        //VStringBuffer tokenRequestURL("https://api.loganalytics.io/v1/workspaces/%s/query?query=%s", workspaceID, encodedKQL);
        VStringBuffer dsRequestURL("%s/api/datasources", connectStr);
  
        VStringBuffer bAuth("%s:%s", user, pass);

        curl_easy_setopt(curlHandle, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(curlHandle, CURLOPT_USERPWD, bAuth.str());
        //headers = curl_slist_append(headers, bearerHeader.str());

        //if (curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, headers.getClear()) != CURLE_OK)
        //    throw makeStringExceptionV(-1, "%s: Log query request: Could not set 'CURLOPT_HTTPHEADER'", COMPONENT_NAME);

        if (curl_easy_setopt(curlHandle, CURLOPT_URL, dsRequestURL.str()) != CURLE_OK)
            throw makeStringExceptionV(-1, "%s: datasources request: Could not set 'CURLOPT_URL' (%s)!", COMPONENT_NAME, dsRequestURL.str());

        if (curl_easy_setopt(curlHandle, CURLOPT_POST, 0) != CURLE_OK)
            throw makeStringExceptionV(-1, "%s: Log query request: Could not disable 'CURLOPT_POST' option!", COMPONENT_NAME);

        if (curl_easy_setopt(curlHandle, CURLOPT_HTTPGET, 1) != CURLE_OK)
            throw makeStringExceptionV(-1, "%s: Log query request: Could not set 'CURLOPT_HTTPGET' option!", COMPONENT_NAME);

        if (curl_easy_setopt(curlHandle, CURLOPT_NOPROGRESS, 1) != CURLE_OK)
            throw makeStringExceptionV(-1, "%s: Log query request: Could not set 'CURLOPT_NOPROGRESS' option!", COMPONENT_NAME);

        if (curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, stringCallback) != CURLE_OK)
            throw makeStringExceptionV(-1, "%s: Log query request: Could not set 'CURLOPT_WRITEFUNCTION' option!", COMPONENT_NAME);

        if (curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &readBuffer) != CURLE_OK)
            throw makeStringExceptionV(-1, "%s: Log query request: Could not set 'CURLOPT_WRITEDATA' option!", COMPONENT_NAME);

        if (curl_easy_setopt(curlHandle, CURLOPT_USERAGENT, "HPCC Systems Log Access client") != CURLE_OK)
            throw makeStringExceptionV(-1, "%s: Log query request: Could not set 'CURLOPT_USERAGENT' option!", COMPONENT_NAME);

        if (curl_easy_setopt(curlHandle, CURLOPT_ERRORBUFFER, curlErrBuffer) != CURLE_OK)
            throw makeStringExceptionV(-1, "%s: Log query request: Could not set 'CURLOPT_ERRORBUFFER' option!", COMPONENT_NAME);

        if (curl_easy_setopt(curlHandle, CURLOPT_FAILONERROR, 1L) != CURLE_OK) // non HTTP Success treated as error
            throw makeStringExceptionV(-1, "%s: Log query request: Could not set 'CURLOPT_FAILONERROR'option!", COMPONENT_NAME);

        try
        {
            curlResponseCode = curl_easy_perform(curlHandle);
        }
        catch (...)
        {
            throw makeStringExceptionV(-1, "%s KQL request: Unknown libcurl error", COMPONENT_NAME);
        }

        if (curlResponseCode != CURLE_OK)
        {
            long response_code;
            curl_easy_getinfo(curlHandle, CURLINFO_RESPONSE_CODE, &response_code);

            StringBuffer message;
            /*switch (response_code)
            {
            case 400L:
                throw makeStringExceptionV(-1,"%s KQL response: Error (400): Request is badly formed and failed (permanently): '%s'", COMPONENT_NAME, curlErrBuffer);
            case 401L:
                throw makeStringExceptionV(-1,"%s KQL response: Error (401): Unauthorized - Client needs to authenticate first: '%s'", COMPONENT_NAME, curlErrBuffer);
            case 403L:
                throw makeStringExceptionV(-1,"%s KQL response: Error (403): Forbidden - Client request is denied: '%s'", COMPONENT_NAME, curlErrBuffer);
            case 404L:
                throw makeStringExceptionV(-1,"%s KQL request: Error (404): NotFound - Request references a non-existing entity. Ensure configured WorkspaceID is valid!: '%s'", COMPONENT_NAME, curlErrBuffer);
            case 413:
                throw makeStringExceptionV(-1,"%s KQL request: Error (413): PayloadTooLarge - Request payload exceeded limits: '%s'", COMPONENT_NAME, curlErrBuffer);
            case 429:
                throw makeStringExceptionV(-1,"%s KQL request: Error (429): TooManyRequests - Request has been denied because of throttling: '%s'", COMPONENT_NAME, curlErrBuffer);
            case 504:
                throw makeStringExceptionV(-1,"%s KQL request: Error (504): Timeout - Request has timed out: '%s'", COMPONENT_NAME, (curlErrBuffer[0] ? curlErrBuffer : "" ));
            case 520:
                throw makeStringExceptionV(-1,"%s KQL request: Error (520): Azure ServiceError - Service found an error while processing the request: '%s'", COMPONENT_NAME, curlErrBuffer);
            default:
                throw makeStringExceptionV(-1,"%s KQL request: Error (%d): '%s'", COMPONENT_NAME, curlResponseCode, (curlErrBuffer[0] ? curlErrBuffer : "Unknown Error"));
            }*/
            throw makeStringExceptionV(-1,"%s KQL request: Error (%d): '%s'", COMPONENT_NAME, curlResponseCode, (curlErrBuffer[0] ? curlErrBuffer : "Unknown Error"));
        }
        else if (readBuffer.length() == 0)
            throw makeStringExceptionV(-1, "%s KQL request: Empty response!", COMPONENT_NAME);
    }
}

bool GrafanaCurlClient::fetchLog(LogQueryResultDetails & resultDetails, const LogAccessConditions & options, StringBuffer & returnbuf, LogAccessLogFormat format)
{

DBGLOG("##GrafanaCurlClient::fetchLog...");
    std::string readBuffer;
    //submitKQLQuery(readBuffer, token.str(), queryString.str(), m_logAnalyticsWorkspaceID.str());
DBGLOG("###GrafanaCurlClient::fetchLog::getDataSources...");
    getDataSources(readBuffer, m_grafanaConnectionStr.str(), m_userName.str(), m_pass.str());
DBGLOG("####Resp:'%s'", readBuffer.c_str());
DBGLOG("###GrafanaCurlClient::fetchLog::issueQuery...");
    readBuffer.clear();
    issueQuery(readBuffer, "query={app=~\"dafilesrv|dali|dfs|dfuserver|eclagent|eclccserver|eclqueries|eclscheduler|eclservices|eclwatch|sasha|thor\"}",  m_grafanaConnectionStr.str(), m_userName.str(), m_pass.str());
return false;
}

static constexpr const char * grafanaLogAccessSecretCategory = "esp";
static constexpr const char * grafanaLogAccessSecretName = "grafana-logaccess";

GrafanaCurlClient::GrafanaCurlClient(IPropertyTree & logAccessPluginConfig)
{
    PROGLOG("%s: Resolving all required configuration values...", COMPONENT_NAME);

    m_pluginCfg.set(&logAccessPluginConfig);

    Owned<IPropertyTree> secretTree = getSecret(grafanaLogAccessSecretCategory, grafanaLogAccessSecretName);
    if (!secretTree)
        throw makeStringExceptionV(-1, "%s: Could not fetch %s information!", COMPONENT_NAME, grafanaLogAccessSecretName);

    getSecretKeyValue(m_userName.clear(), secretTree, "user");
    if (m_userName.isEmpty())
    {
        WARNLOG("%s: Could not find '%s.%s' secret value!", COMPONENT_NAME, grafanaLogAccessSecretName, "user");
        m_userName.set(logAccessPluginConfig.queryProp("connection/@user"));
        if (m_userName.isEmpty())
            throw makeStringExceptionV(-1, "%s: Could not find Grafana user name, provide it as part of '%s.%s' secret, or connection/@user in Grafana LogAccess configuration!", COMPONENT_NAME, grafanaLogAccessSecretName, "user");
    }

    getSecretKeyValue(m_pass.clear(), secretTree, "password");
    if (m_pass.isEmpty())
    {
        WARNLOG("%s: Could not find '%s.%s' secret value!", COMPONENT_NAME, grafanaLogAccessSecretName, "password");
        m_pass.set(logAccessPluginConfig.queryProp("connection/@password"));
        if (m_pass.isEmpty())
            throw makeStringExceptionV(-1, "%s: Could not find Grafana password, provide it as part of '%s.%s' secret, or connection/@password in Grafana LogAccess configuration!", COMPONENT_NAME, grafanaLogAccessSecretName, "password");
    }

    const char * protocol = logAccessPluginConfig.queryProp("connection/@protocol");
    if (isEmptyString(protocol))
        protocol = "http";
    const char * host = logAccessPluginConfig.queryProp("connection/@host");
    if (isEmptyString(protocol))
        host = "localhost";
    const char * port = logAccessPluginConfig.queryProp("connection/@port");
    if (isEmptyString(port))
        port = "80";

    m_grafanaDataSourceName.set(logAccessPluginConfig.queryProp("connection/@dsname"));
    if (isEmptyString(m_grafanaDataSourceName))
        m_grafanaDataSourceName.set("loki");

    m_grafanaDataSourceID.set(logAccessPluginConfig.queryProp("connection/@dsid"));
    if (isEmptyString(m_grafanaDataSourceID))
        m_grafanaDataSourceID.set("1");

    //m_grafanaConnectionStr.setf("%s://%s:%s/api/datasources/proxy/%s/%s/", protocol, host, port, dataSourceID, dataSourceName);
    m_grafanaConnectionStr.setf("%s://%s:%s", protocol, host, port);
    //m_grafanaConnectionStr.setf("http://myloki-grafana.default.svc.cluster.local:3000/api/datasources/proxy/1/loki/");
 //   /api/v1/query_range?direction%3DBACKWARD%26limit%3D1" --data-urlencode 'query={app="eclwatch"}' --user 'admin:qxLGUi343AUoRB1vvbHV41hwoqptBb39H8sv8TAi'

    LOG(MCoperatorProgress, "%s: Connection String: '%s'", COMPONENT_NAME, m_grafanaConnectionStr.str());

    m_globalIndexTimestampField.set(defaultHPCCLogTimeStampCol);
    m_globalIndexSearchPattern.set(defaultIndexPattern);
    m_globalSearchColName.set(defaultHPCCLogMessageCol);

    m_classSearchColName.set(defaultHPCCLogTypeCol);
    m_workunitSearchColName.set(defaultHPCCLogJobIDCol);
    m_componentsSearchColName.set(defaultHPCCLogComponentCol);
    m_audienceSearchColName.set(defaultHPCCLogAudCol);

    Owned<IPropertyTreeIterator> logMapIter = m_pluginCfg->getElements("logMaps");
    ForEach(*logMapIter)
    {
        IPropertyTree & logMap = logMapIter->query();
        const char * logMapType = logMap.queryProp("@type");
        if (streq(logMapType, "global"))
        {
            if (logMap.hasProp(logMapIndexPatternAtt))
                m_globalIndexSearchPattern = logMap.queryProp(logMapIndexPatternAtt);
            if (logMap.hasProp(logMapSearchColAtt))
                m_globalSearchColName = logMap.queryProp(logMapSearchColAtt);
            if (logMap.hasProp(logMapTimeStampColAtt))
                m_globalIndexTimestampField = logMap.queryProp(logMapTimeStampColAtt);
        }
        else if (streq(logMapType, "workunits"))
        {
            if (logMap.hasProp(logMapIndexPatternAtt))
                m_workunitIndexSearchPattern = logMap.queryProp(logMapIndexPatternAtt);
            if (logMap.hasProp(logMapSearchColAtt))
                m_workunitSearchColName = logMap.queryProp(logMapSearchColAtt);
        }
        else if (streq(logMapType, "components"))
        {
            if (logMap.hasProp(logMapIndexPatternAtt))
                m_componentsIndexSearchPattern = logMap.queryProp(logMapIndexPatternAtt);
            if (logMap.hasProp(logMapSearchColAtt))
                m_componentsSearchColName = logMap.queryProp(logMapSearchColAtt);
            if (logMap.hasProp(logMapKeyColAtt))
                m_componentsLookupKeyColumn = logMap.queryProp(logMapKeyColAtt);
            if (logMap.hasProp(logMapTimeStampColAtt))
                m_componentsTimestampField = logMap.queryProp(logMapTimeStampColAtt);
            else
                m_componentsTimestampField = defaultHPCCLogComponentTSCol;

            m_disableComponentNameJoins = logMap.getPropBool(logMapDisableJoinsAtt, false);
        }
        else if (streq(logMapType, "class"))
        {
            if (logMap.hasProp(logMapIndexPatternAtt))
                m_classIndexSearchPattern = logMap.queryProp(logMapIndexPatternAtt);
            if (logMap.hasProp(logMapSearchColAtt))
                m_classSearchColName = logMap.queryProp(logMapSearchColAtt);
        }
        else if (streq(logMapType, "audience"))
        {
            if (logMap.hasProp(logMapIndexPatternAtt))
                m_audienceIndexSearchPattern = logMap.queryProp(logMapIndexPatternAtt);
            if (logMap.hasProp(logMapSearchColAtt))
                m_audienceSearchColName = logMap.queryProp(logMapSearchColAtt);
        }
        else if (streq(logMapType, "instance"))
        {
            if (logMap.hasProp(logMapIndexPatternAtt))
                m_instanceIndexSearchPattern = logMap.queryProp(logMapIndexPatternAtt);
            if (logMap.hasProp(logMapSearchColAtt))
                m_instanceSearchColName = logMap.queryProp(logMapSearchColAtt);
        }
        else if (streq(logMapType, "host") || streq(logMapType, "node"))
        {
            if (logMap.hasProp(logMapIndexPatternAtt))
                m_hostIndexSearchPattern = logMap.queryProp(logMapIndexPatternAtt);
            if (logMap.hasProp(logMapSearchColAtt))
                m_hostSearchColName = logMap.queryProp(logMapSearchColAtt);
        }
        else
        {
            ERRLOG("Encountered invalid LogAccess field map type: '%s'", logMapType);
        }
    }
}

IRemoteLogAccessStream * GrafanaCurlClient::getLogReader(const LogAccessConditions & options, LogAccessLogFormat format)
{
    return getLogReader(options, format, defaultMaxRecordsPerFetch);
}

IRemoteLogAccessStream * GrafanaCurlClient::getLogReader(const LogAccessConditions & options, LogAccessLogFormat format, unsigned int pageSize)
{
    return new GrafanaStream(this, options, format, pageSize);
}

extern "C" IRemoteLogAccess * createInstance(IPropertyTree & logAccessPluginConfig)
{
    return new GrafanaCurlClient(logAccessPluginConfig);
}
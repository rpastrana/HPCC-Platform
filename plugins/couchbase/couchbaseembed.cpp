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

//#include "platform.h"
#include "jexcept.hpp"
//#include "jthread.hpp"
//#include "hqlplugins.hpp"
//#include "deftype.hpp"
//#include "eclhelper.hpp"
//#include "eclrtl.hpp"
//#include "eclrtl_imp.hpp"
//#include "rtlds_imp.hpp"
//#include "rtlfield_imp.hpp"
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
    static void rowCallback(lcb_t instance, int cbtype, const lcb_RESPN1QL *resp)
    {
        if (! (resp->rflags & LCB_RESP_F_FINAL))
        {
            failx("Row: %.*s\n", (int)resp->nrow, resp->row);
            printf("Row: %.*s\n", (int)resp->nrow, resp->row);
        }
        else
        {
            failx("Got metadata: %.*s\n", (int)resp->nrow, resp->row);
            printf("Got metadata: %.*s\n", (int)resp->nrow, resp->row);
        }
}
/*
    static void die(lcb_t instance, const char *msg, lcb_error_t err)
    {
        fprintf(stderr, "%s. Received code 0x%X (%s)\n", msg, err, lcb_strerror(instance, err));
        exit(EXIT_FAILURE);
    }*/

    static void get_callback(lcb_t instance, int cbtype, const lcb_RESPBASE *rb)
    {
        const lcb_RESPGET *rg = (const lcb_RESPGET *)rb;
        if (rg->rc != LCB_SUCCESS)
        {
            fprintf(stderr, "Failed to get key: %s\n", lcb_strerror(instance, rg->rc));
            exit(EXIT_FAILURE);
        }
        fprintf(stdout, "I stored and retrieved the key 'foo'. Value: %.*s. Terminate program\n", (int)rg->nvalue, (const char *)rg->value);
        //event_base_loopbreak((void *)lcb_get_cookie(instance));
        (void)cbtype;
    }

    static void store_callback(lcb_t instance, int cbtype, const lcb_RESPBASE *rb)
    {
        lcb_error_t rc;
        lcb_CMDGET gcmd =  { 0 };
        if (rb->rc != LCB_SUCCESS)
        {
            fprintf(stderr, "Failed to store key: %s\n", lcb_strerror(instance, rb->rc));
            exit(EXIT_FAILURE);
        }
        LCB_CMD_SET_KEY(&gcmd, rb->key, rb->nkey);
        rc = lcb_get3(instance, NULL, &gcmd);
        if (rc != LCB_SUCCESS)
        {
            fprintf(stderr, "Failed to schedule get request: %s\n", lcb_strerror(NULL, rc));
            exit(EXIT_FAILURE);
        }
    }

    static void    op_callback(lcb_t instance, int cbtype, const lcb_RESPBASE *rb)
    {
        fprintf(stderr, "=== %s ===\n", lcb_strcbtype(cbtype));
        if (rb->rc == LCB_SUCCESS)
        {
            fprintf(stderr, "KEY: %.*s\n", (int)rb->nkey, (const char *)rb->key);
            //fprintf(stderr, "CAS: 0x%" PRIx64"\n", rb->cas);
            if (cbtype == LCB_CALLBACK_GET)
            {
                const lcb_RESPGET *rg = (const lcb_RESPGET *)rb;
                fprintf(stderr, "VALUE: %.*s\n", (int)rg->nvalue, (const char *)rg->value);
                failx( "VALUE: %.*s\n", (int)rg->nvalue, (const char *)rg->value);
                fprintf(stderr, "FLAGS: 0x%x\n", rg->itmflags);
            }
        }
        else
        {
            //failx( lcb_strcbtype(rb->rc), rb->rc);
            failx( "%s. Received code 0x%X (%s)\n", lcb_strcbtype(rb->rc), rb->rc, lcb_strerror(instance, rb->rc));
        }
        (void)instance;
    }

    static void    bootstrap_callback(lcb_t instance, lcb_error_t err)
    {
        lcb_CMDSTORE cmd = { 0 };

        if (err != LCB_SUCCESS)
        {
            failx("ERROR: %s\n", lcb_strerror(instance, err));
        }

        /* Since we've got our configuration, let's go ahead and store a value */
        LCB_CMD_SET_KEY(&cmd, "foo", 3);
        LCB_CMD_SET_VALUE(&cmd, "bar", 3);
        cmd.operation = LCB_SET;
        err = lcb_store3(instance, NULL, &cmd);
        if (err != LCB_SUCCESS)
        {
            failx("Failed to set up store request: %s\n", lcb_strerror(instance, err));
        }
    }

    static lcb_t create_libcouchbase_handle()
    {
        lcb_t instance;
        lcb_error_t error;
        struct lcb_create_st copts;
        memset(&copts, 0, sizeof(copts));

        /* If NULL, will default to localhost */
        copts.v.v0.host = getenv("LCB_EVENT_SERVER");
        error = lcb_create(&instance, &copts);

        if (error != LCB_SUCCESS)
            failx("Failed to create a libcouchbase instance: %s\n", lcb_strerror(NULL, error));

        /* Set up the callbacks */
        lcb_set_bootstrap_callback(instance, bootstrap_callback);
        lcb_install_callback3(instance, LCB_CALLBACK_GET, get_callback);
        lcb_install_callback3(instance, LCB_CALLBACK_STORE, store_callback);
        if ((error = lcb_connect(instance)) != LCB_SUCCESS)
        {
            lcb_destroy(instance);
            failx("Failed to connect libcouchbase instance: %s\n", lcb_strerror(NULL, error));
        }
        return instance;
    }

    COUCHBASEEMBED_PLUGIN_API unsigned COUCHBASEEMBED_PLUGIN_CALL func1(ICodeContext * ctx, const char * param1, const char * param2, unsigned param3)
    {
        lcb_error_t err;
        lcb_t instance;
        struct lcb_create_st create_options = { 0 };
        lcb_CMDSTORE scmd = { 0 };
        lcb_CMDGET gcmd = { 0 };
        create_options.version = 3;

        create_options.v.v3.connstr = "couchbase://10.176.152.227/iot";

        err = lcb_create(&instance, &create_options);
        if (err != LCB_SUCCESS)
        {
            failx("Couldn't create couchbase handle, error: %d", err);
        }

        err = lcb_connect(instance);
        if (err != LCB_SUCCESS)
        {
            failx("Couldn't schedule connection, error: %d", err);
        }

        lcb_wait(instance);
        err = lcb_get_bootstrap_status(instance);

        if (err != LCB_SUCCESS)
        {
            failx("Couldn't bootstrap from cluster, error: %d", err);
        }

        // The command structure
        lcb_CMDN1QL cmd = { 0 };
        // Allocate the parameter object
        lcb_N1QLPARAMS *nparams = lcb_n1p_new();

        lcb_n1p_setstmtz(nparams,
           //"SELECT fname || \" \" || lname, age FROM default WHERE age > $age LIMIT 5");
             "select contextualData from  `iot` where contextualData.gps.latitude = $lat");
        // Set the value for '$age'
        lcb_n1p_namedparamz(nparams, "$lat", "44.968046");
        // Now, fill the command structure
        lcb_n1p_mkcmd(nparams, &cmd);
        //#if DEBUG
        // Dump the encoded query
        //printf("Encoded query is: %s\n", lcb_n1p_encode(nparams));
        //#endif
        cmd.callback = rowCallback;

        lcb_error_t rc = lcb_n1ql_query(instance, NULL, &cmd);
        if (rc != LCB_SUCCESS)
        {
            failx("lcb_n1ql_query error: %d", rc);
        }
        // We can release the params object now..
        lcb_n1p_free(nparams);
        lcb_wait(instance);

        /*


        lcb_t instance = create_libcouchbase_handle();

        lcb_error_t err;
        //lcb_t instance;
        struct lcb_create_st create_options = { 0 };
        lcb_CMDSTORE scmd = { 0 };
        lcb_CMDGET gcmd = { 0 };
        create_options.version = 3;

        create_options.v.v3.connstr = "couchbase://10.176.152.227/iot";

        err = lcb_create(&instance, &create_options);
        if (err != LCB_SUCCESS)
        {
            failx("Couldn't create couchbase handle, error: %d", err);
        }

        err = lcb_connect(instance);
        if (err != LCB_SUCCESS)
        {
            failx("Couldn't schedule connection, error: %d", err);
        }

        lcb_wait(instance);
        err = lcb_get_bootstrap_status(instance);

        if (err != LCB_SUCCESS)
        {
            failx("Couldn't bootstrap from cluster, error: %d", err);
        }

        // Assign the handlers to be called for the operation types
        lcb_install_callback3(instance, LCB_CALLBACK_GET, op_callback);
        lcb_install_callback3(instance, LCB_CALLBACK_STORE, op_callback);

        LCB_CMD_SET_KEY(&scmd, "mydoc2", strlen("mydoc2"));
        LCB_CMD_SET_VALUE(&scmd, "{      \"accelx\": 23.323,      \"accely\": 23.323,      \"accelz\": 23.32,         \"contextualData\": {         \"gps\": {           \"latitude\": 44.968046,           \"longitude\": -94.420307         },         \"gyroscope\": {           \"x\": 23.323,           \"y\": 23.323,           \"z\": 23.323         },         \"magneticfield\": {           \"x\": 23.323,           \"y\": 23.323,           \"z\": 23.323         },         \"wifi\": [           {             \"id\": \"c8: d7: 19: a6: 39: b9\",             \"value\": -63           },           {             \"id\": \"9c: 1c: 12: 28: 3a: d22\",             \"value\": -75           },           {             \"id\": \"d8: c7: c8: ec: 2a: 61\",             \"value\": -77           },           {             \"id\": \"d8: c7: c8: ec: 2a: 62\",             \"value\": -78           },           {             \"id\": \"dc: a5: f4: 8d: 5f: 47\",             \"value\": -84           }         ]       },       \"eventId\": \"YYYY\",       \"eventStatus\": false,       \"guid\": \"8f3ba6f4-5c70-46ec-83af-0d5434953e5f\",       \"isStaleData\": false,       \"locationData\": {         \"x\": 23.323,         \"y\": 23.323,         \"z\": 23.323,         \"zoneId\": \"zone1\"       },       \"rawData\": {         \"ambientTemp\": 37.77,         \"barometer\": 15.43,         \"batteryLevelPercentage\": 85.32,         \"bodyTemp\": 30.25,         \"coLevel\": 1.87,         \"forceSensitiveResistance\": 166.46,         \"heartRate\": 90       },       \"sequence\": 100,       \"sourceoffset\": 4,       \"sourcepartition\": \"0\",       \"sourcetopic\": \"test4\",       \"timestamp\": \"2016-01-01 11:36:05.657314-04:00\"     }", strlen("{      \"accelx\": 23.323,      \"accely\": 23.323,      \"accelz\": 23.32,         \"contextualData\": {         \"gps\": {           \"latitude\": 44.968046,           \"longitude\": -94.420307         },         \"gyroscope\": {           \"x\": 23.323,           \"y\": 23.323,           \"z\": 23.323         },         \"magneticfield\": {           \"x\": 23.323,           \"y\": 23.323,           \"z\": 23.323         },         \"wifi\": [           {             \"id\": \"c8: d7: 19: a6: 39: b9\",             \"value\": -63           },           {             \"id\": \"9c: 1c: 12: 28: 3a: d22\",             \"value\": -75           },           {             \"id\": \"d8: c7: c8: ec: 2a: 61\",             \"value\": -77           },           {             \"id\": \"d8: c7: c8: ec: 2a: 62\",             \"value\": -78           },           {             \"id\": \"dc: a5: f4: 8d: 5f: 47\",             \"value\": -84           }         ]       },       \"eventId\": \"YYYY\",       \"eventStatus\": false,       \"guid\": \"8f3ba6f4-5c70-46ec-83af-0d5434953e5f\",       \"isStaleData\": false,       \"locationData\": {         \"x\": 23.323,         \"y\": 23.323,         \"z\": 23.323,         \"zoneId\": \"zone1\"       },       \"rawData\": {         \"ambientTemp\": 37.77,         \"barometer\": 15.43,         \"batteryLevelPercentage\": 85.32,         \"bodyTemp\": 30.25,         \"coLevel\": 1.87,         \"forceSensitiveResistance\": 166.46,         \"heartRate\": 90       },       \"sequence\": 100,       \"sourceoffset\": 4,       \"sourcepartition\": \"0\",       \"sourcetopic\": \"test4\",       \"timestamp\": \"2016-01-01 11:36:05.657314-04:00\"     }"));

        scmd.operation = LCB_SET;
        err = lcb_store3(instance, NULL, &scmd);
        if (err != LCB_SUCCESS)
        {
            failx("Couldn't schedule storage operation error: %d", err);
        }

        //The store_callback is invoked from lcb_wait()
        fprintf(stderr, "Will wait for storage operation to complete..\n");
        lcb_wait(instance);

        // Now fetch the item back
        LCB_CMD_SET_KEY(&gcmd, "mydoc", strlen("mydoc"));
        err = lcb_get3(instance, NULL, &gcmd);
        if (err != LCB_SUCCESS)
        {
            failx("Couldn't schedule retrieval operation error: %d", err);
        }

        //Likewise, the get_callback is invoked from here
        fprintf(stderr, "Will wait to retrieve item..\n");
        lcb_wait(instance);
        // Now that we're all done, close down the connection handle
        lcb_destroy(instance);
*/
        return param3 + 1;
    }

    COUCHBASEEMBED_PLUGIN_API void COUCHBASEEMBED_PLUGIN_CALL func2 (ICodeContext * _ctx, size32_t & returnLength, char * & returnValue, const char * param1, const char * param2, size32_t param3ValueLength, const char * param3Value)
    {
        lcb_error_t err;
        lcb_t instance;
        struct lcb_create_st create_options = { 0 };
        lcb_CMDSTORE scmd = { 0 };
        lcb_CMDGET gcmd = { 0 };
        create_options.version = 3;

        create_options.v.v3.connstr = "couchbase://10.176.152.227/iot";

        err = lcb_create(&instance, &create_options);
        if (err != LCB_SUCCESS)
        {
            failx("Couldn't create couchbase handle, error: %d", err);
        }

        err = lcb_connect(instance);
        if (err != LCB_SUCCESS)
        {
            failx("Couldn't schedule connection, error: %d", err);
        }

        lcb_wait(instance);
        err = lcb_get_bootstrap_status(instance);

        if (err != LCB_SUCCESS)
        {
            failx("Couldn't bootstrap from cluster, error: %d", err);
        }

        /* Assign the handlers to be called for the operation types */
        lcb_install_callback3(instance, LCB_CALLBACK_GET, op_callback);
        lcb_install_callback3(instance, LCB_CALLBACK_STORE, op_callback);

        LCB_CMD_SET_KEY(&scmd, "mydoc1", strlen("mydoc1"));
        LCB_CMD_SET_VALUE(&scmd, "myvalue", strlen("myvalue"));

//                scmd.operation = LCB_SET;
//            err = lcb_store3(instance, NULL, &scmd);
//            if (err != LCB_SUCCESS)
//        {
    //        failx("Couldn't schedule storage operation error: %d", err);
      //  }

        /* The store_callback is invoked from lcb_wait() */
        //fprintf(stderr, "Will wait for storage operation to complete..\n");
        //lcb_wait(instance);

        /* Now fetch the item back */
        LCB_CMD_SET_KEY(&gcmd, "mydoc2", strlen("mydoc2"));
        err = lcb_get3(instance, NULL, &gcmd);
        if (err != LCB_SUCCESS)
        {
            failx("Couldn't schedule retrieval operation error: %d", err);
        }

        /* Likewise, the get_callback is invoked from here */
        fprintf(stderr, "Will wait to retrieve item..\n");
        lcb_wait(instance);
        /* Now that we're all done, close down the connection handle */
        lcb_destroy(instance);

        StringBuffer buffer(param3Value);
        buffer.toLowerCase();
        returnLength = buffer.length();
        returnValue = buffer.detach();
        return;
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
/*
    lcb_error_t err;
    lcb_t instance;
    struct lcb_create_st create_options = { 0 };
    lcb_CMDSTORE scmd = { 0 };
    lcb_CMDGET gcmd = { 0 };
    create_options.version = 3;
    */

//static void logCallBack(const CassLogMessage *message, void *data)
//{
//    DBGLOG("%s: %s - %s", g_moduleName, cass_log_level_string(message->severity), message->message);
//}
//void check(CassError rc)
//{
//    if (rc != CASS_OK)
//    {
//        fail(cass_error_desc(rc));
//    }
//}

} // namespace

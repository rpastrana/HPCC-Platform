#ECL Embedded Couchbase plugin

This is the ECL plugin to access couchbase systems

Client access is based on libcouchbase (c interface) + libcouchbase-cxx
https://github.com/couchbaselabs/libcouchbase-cxx

##Installation and Dependencies

[libcouchbase] is installed via standard package managers


(https://github.com/couchbaselabs/libcouchbase-cxxa) is included as a git
submodule in HPCC-Platform.  It will be built and integrated automatically when
you build the HPCC-Platform project with the couchbase plugin flag turned on.

The recommended method for obtaining Apache Kafka is via
On Ubuntu: sudo apt-get install libcouchbase-dev libcouchbase2-bin build-essential

##ECL Plugin Use
This is a WIP plugin, currently only exposing a general 'executen1ql' interface

Examples:
IMPORT * FROM plugins.couchbase;

OUTPUT(couchbase.couchbaseembed.executen1ql('10.176.152.1', 'iot', 'CREATE PRIMARY INDEX ON `iot`'));
OUTPUT(couchbase.couchbaseembed.executen1ql('10.176.152.1', 'iot', 'INSERT INTO iot (KEY, VALUE) VALUES ("myentry-1", { "accelx": 24.33,  "accely": 2.33,  "accelz": 234.42,  "contextualData": { "gps": { }, "gyroscope": { "x": 23.323, "y": 23.323, "z": 23.323 }, "magneticfield": { "x": 23.323, "y": 23.323, "z": 23.323 }, "wifi": [ {  "id": "c8: d7: 19: a6: 39: b9",  "value": -63 }, {  "id": "9c: 1c: 12: 28: 3a: d22",  "value": -75 }, {  "id": "d8: c7: c8: ec: 2a: 61",  "value": -77 }, {  "id": "d8: c7: c8: ec: 2a: 62",  "value": -78 }, {  "id": "dc: a5: f4: 8d: 5f: 47",  "value": -84 } ]  },  "eventId": "YYYY",  "eventStatus": false,  "guid": "8f3ba6f4-5c70-46ec-83af-0d5434953e5f",  "isStaleData": false,  "locationData": { "x": 23.323, "y": 23.323, "z": 23.323, "zoneId": "zone1"  },  "rawData": { "ambientTemp": 37.77, "barometer": 15.43, "batteryLevelPercentage": 85.32, "bodyTemp": 30.25, "coLevel": 1.87, "forceSensitiveResistance": 166.46, "heartRate": 90  },  "sequence": 100,  "sourceoffset": 4,  "sourcepartition": "0",  "sourcetopic": "test4",  "timestamp": "2016-01-07 11:36:05.657314-04:00" } );'));
OUTPUT(couchbase.couchbaseembed.executen1ql('10.176.152.1', 'iot', 'SELECT * from `iot` where  timestamp = "2016-01-07 11:36:05.657314-04:00"'));
OUTPUT(couchbase.couchbaseembed.executen1ql('10.176.152.1', 'iot', 'DELETE FROM `iot` where timestamp = "2016-01-07 11:36:05.657314-04:00"'));
OUTPUT(couchbase.couchbaseembed.executen1ql('10.176.152.1', 'iot', 'SELECT * from `iot` where timestamp = "2016-01-07 11:36:05.657314-04:00"'));
OUTPUT(couchbase.couchbaseembed.executen1ql('10.176.152.1', 'iot', 'SELECT count(contextualData.gps) count from `iot` '));
OUTPUT(couchbase.couchbaseembed.executen1ql('10.176.152.1', 'iot', 'SELECT contextualData.gps from `iot` where contextualData.gps.latitude = 44.968046'));

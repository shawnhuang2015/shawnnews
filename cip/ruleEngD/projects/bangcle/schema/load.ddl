CLEAR GRAPH STORE -HARD

#
# Create loading job
#

SET sys.data_root="/tmp/bangcle/data/"

DROP JOB load

CREATE LOADING JOB load FOR GRAPH bangcleGraph {

    DEFINE HEADER header_devices = "uuid", "udid", "imei", "android_id", "brand", "model", "manufacturer", "os_name", "os_version", "sdk_version", "run_mode", "is_root", "resolution_h", "resolution_w", "mac", "cpu", "imsi", "created_at", "installed_at_24";
    DEFINE HEADER header_biz = "id", "md5", "ts", "ip", "account", "imei", "mac", "action_type";

    LOAD "$sys.data_root/devices.csv"
    TO VERTEX deviceEvent VALUES ($"uuid", $"created_at"),
    TO VERTEX udid VALUES ($"udid"),
    TO VERTEX imei VALUES ($"imei"),
    TO VERTEX imsi VALUES ($"imsi"),
    TO VERTEX mac VALUES ($"mac"),
    TO EDGE deviceEvent_udid VALUES (
        gsql_concat($"uuid", $"udid"),
    TO EDGE deviceEvent_imei VALUES (
        gsql_concat($"uuid", $"imei"),
    TO EDGE deviceEvent_imsi VALUES (
        gsql_concat($"uuid", $"imsi"),
    TO EDGE deviceEvent_mac VALUES (
        gsql_concat($"uuid", $"mac"),
    TO EDGE udid_imei VALUES ($"udid", $"imei"),
    TO EDGE udid_imsi VALUES ($"udid", $"imsi"),
    TO EDGE udid_mac VALUES ($"udid", $"mac")
    USING user_defined_header="header_devices", separator=",";

    LOAD "$sys.data_root/biz.csv"
    TO VERTEX bizEvent VALUES ($"id", $"ts", $"action_type"),
    TO VERTEX imei VALUES ($"imei"),
    TO VERTEX mac VALUES ($"mac"),
    TO VERTEX ip VALUES ($"ip"),
    TO VERTEX md5 VALUES ($"md5"),
    TO VERTEX account VALUES ($"account"),
    TO EDGE bizEvent_imei VALUES ($"id", $"imei"),
    TO EDGE bizEvent_mac VALUES ($"id", $"mac"),
    TO EDGE bizEvent_mac VALUES ($"id", $"ip"),
    TO EDGE bizEvent_account VALUES ($"id", $"account")
    TO EDGE bizEvent_md5 VALUES ($"id", $"md5")
    USING user_defined_header="header_biz", separator=",";

}	

END

RUN JOB load


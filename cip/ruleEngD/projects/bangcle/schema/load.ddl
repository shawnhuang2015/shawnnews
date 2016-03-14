CLEAR GRAPH STORE -HARD

#
# Create loading job
#

SET sys.data_root="/tmp/data/20160309"

DROP JOB load_guotai

CREATE LOADING JOB load_guotai FOR GRAPH bangcleGraph {

    DEFINE HEADER header_deviceInitEvent = "uuid","ref_id","idfa","ostype","fingerprint","ip","csubnet","timestamp";
    # DEFINE HEADER header_deviceInitEvent = "id", "idfa", "fingerprint", "ip", "c_subnet", "os_type", "udid", "ts";
    DEFINE HEADER header_userActivationEvent = "uuid","ref_id","idfa","ostype","fingerprint","geo_hash","latitude","longitude","ip","csubnet","account","actiontype","signup_duration","account_input_duration","TimeStamp";
    # DEFINE HEADER header_userActivationEvent = "id", "idfa", "fingerprint", "ip", "c_subnet", "os_type", "account", "action_type", "ts"; # lack of: ref_id, geo_hash
    DEFINE HEADER header_bizEvent = "uuid","ref_id","channelid","appid","mac","idfa","openudid","userip","isActive","ostype","phoneType","phoneNum","appVer","xlocation","ylocation","ip","clickTime","clickNumTimeGap","fingerprint","selfidfa","returnStep","createTime","updateTime","activeUpdateTime";

    LOAD "$sys.data_root/DeviceInitEvent.csv"
    TO VERTEX deviceInitEvent VALUES ($"uuid", $"timestamp", $"timestamp"),
    TO VERTEX idfa VALUES ($"idfa", $"timestamp"),
    TO VERTEX fingerprint VALUES ($"fingerprint", $"timestamp"),
    TO VERTEX ip VALUES ($"ip", $"timestamp"),
    TO VERTEX cSubnet VALUES ($"csubnet", $"timestamp"),
    TO EDGE deviceInitEvent_idfa VALUES ($"uuid", $"idfa"),
    TO EDGE deviceInitEvent_fingerprint VALUES ($"uuid", $"fingerprint"),
    TO EDGE deviceInitEvent_ip VALUES ($"uuid", $"ip"),
    TO EDGE ip_cSubnet VALUES ($"ip", $"csubnet")
    USING user_defined_header="header_deviceInitEvent", separator=",";

    LOAD "$sys.data_root/UserActivationEvent.csv"
    TO VERTEX userActivationEvent VALUES ($"uuid", $"TimeStamp", $"TimeStamp", $"ostype", $"ref_id"),
    TO VERTEX idfa VALUES ($"idfa", $"TimeStamp"),
    TO VERTEX fingerprint VALUES ($"fingerprint", $"TimeStamp"),
    TO VERTEX ip VALUES ($"ip", $"TimeStamp"),
    TO VERTEX cSubnet VALUES ($"csubnet", $"TimeStamp"),
    TO VERTEX account VALUES ($"account", $"TimeStamp"),
    TO EDGE userActivationEvent_idfa VALUES ($"uuid", $"idfa"),
    TO EDGE userActivationEvent_fingerprint VALUES ($"uuid", $"fingerprint"),
    TO EDGE userActivationEvent_ip VALUES ($"uuid", $"ip"),
    TO EDGE ip_cSubnet VALUES ($"ip", $"csubnet")
    USING user_defined_header="header_userActivationEvent", separator=",";

    LOAD "$sys.data_root/BizEvent.csv"
    TO VERTEX bizEvent VALUES ($"uuid", $"updateTime", $"isActive", $"ref_id", $"updateTime"),
    TO VERTEX idfa VALUES ($"idfa", $"updateTime"),
    TO VERTEX fingerprint VALUES ($"fingerprint", $"updateTime"),
    TO VERTEX ip VALUES ($"ip", $"updateTime"),
    # TO VERTEX cSubnet VALUES ($"csubnet", $"updateTime"),
    # TO VERTEX account VALUES ($"account", $"updateTime"),
    TO EDGE bizEvent_idfa VALUES ($"uuid", $"idfa"),
    TO EDGE bizEvent_fingerprint VALUES ($"uuid", $"fingerprint"),
    TO EDGE bizEvent_ip VALUES ($"uuid", $"ip")
    # TO EDGE ip_cSubnet VALUES ($"ip", $"csubnet")
    USING user_defined_header="header_bizEvent", separator=",";
}	

END

RUN JOB load_guotai


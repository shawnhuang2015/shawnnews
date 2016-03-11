CLEAR GRAPH STORE -HARD

#
# Create loading job
#

SET sys.data_root="/tmp/bangcle/data/guotai/"

DROP JOB load

CREATE LOADING JOB load FOR GRAPH bangcleGraph {

    DEFINE HEADER header_deviceInitEvent = "id", "idfa", "fingerprint", "ip", "c_subnet", "os_type", "udid", "ts";
    DEFINE HEADER header_userActivationEvent = "id", "idfa", "fingerprint", "ip", "c_subnet", "os_type", "account", "action_type", "ts"; # lack of: ref_id, geo_hash

    LOAD "$sys.data_root/DeviceInitEvent.csv"
    TO VERTEX deviceInitEvent VALUES ($"id", $"ts", $"ts"),
    TO VERTEX idfa VALUES ($"idfa", $"ts"),
    TO VERTEX fingerprint VALUES ($"idfa", $"ts"),
    TO VERTEX ip VALUES ($"ip", $"ts"),
    TO VERTEX cSubnet VALUES ($"c_subnet", $"ts"),
    TO VERTEX udid VALUES ($"udid", $"ts"),
    TO EDGE deviceInitEvent_idfa VALUES ($"id", $"idfa"),
    TO EDGE deviceInitEvent_fingerprint VALUES ($"id", $"fingerprint"),
    TO EDGE deviceInitEvent_ip VALUES ($"id", $"ip"),
    TO EDGE deviceInitEvent_udid VALUES ($"id", $"udid"),
    TO EDGE ip_cSubnet VALUES ($"ip", $"c_subnet")
    USING user_defined_header="header_deviceInitEvent", separator=",";

    LOAD "$sys.data_root/UserActivationEvent.csv"
    TO VERTEX userActivationEvent VALUES ($"id", $"ts", $"ts", $"os_type", _),
    TO VERTEX idfa VALUES ($"idfa", $"ts"),
    TO VERTEX fingerprint VALUES ($"idfa", $"ts"),
    TO VERTEX ip VALUES ($"ip", $"ts"),
    TO VERTEX cSubnet VALUES ($"c_subnet", $"ts"),
    TO VERTEX account VALUES ($"account", $"ts"),
    TO EDGE userActivationEvent_idfa VALUES ($"id", $"idfa"),
    TO EDGE userActivationEvent_fingerprint VALUES ($"id", $"fingerprint"),
    TO EDGE userActivationEvent_ip VALUES ($"id", $"ip"),
    TO EDGE ip_cSubnet VALUES ($"ip", $"c_subnet")
    USING user_defined_header="header_userActivationEvent", separator=",";

}	

END

RUN JOB load


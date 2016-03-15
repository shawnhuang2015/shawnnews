
CREATE online_post JOB load_inc_deviceInitEvent FOR GRAPH bangcleGraph {
    DEFINE HEADER header_deviceInitEvent = "uuid","ref_id","idfa","ostype","fingerprint","ip","csubnet","self_idfa","timestamp";
    LOAD
    TO VERTEX deviceInitEvent VALUES ($"uuid", $"timestamp", $"timestamp"),
    TO VERTEX idfa VALUES ($"idfa", $"timestamp"),
    TO VERTEX fingerprint VALUES ($"fingerprint", $"timestamp"),
    TO VERTEX ip VALUES ($"ip", $"timestamp"),
    TO VERTEX cSubnet VALUES ($"csubnet", $"timestamp"),
    TO EDGE deviceInitEvent_idfa VALUES ($"uuid", $"idfa"),
    TO EDGE deviceInitEvent_fingerprint VALUES ($"uuid", $"fingerprint"),
    TO EDGE deviceInitEvent_ip VALUES ($"uuid", $"ip"),
    TO EDGE ip_cSubnet VALUES ($"ip", $"csubnet")
    USING user_defined_header="header_deviceInitEvent";
}
EXPORT JOB load_inc_deviceInitEvent

CREATE online_post JOB load_inc_userActivationEvent FOR GRAPH bangcleGraph {
    DEFINE HEADER header_userActivationEvent = "uuid","ref_id","idfa","ostype","fingerprint","geo_hash","latitude","longitude","ip","csubnet","account","actiontype","signup_duration","account_input_duration","TimeStamp";
    LOAD
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
    USING user_defined_header="header_userActivationEvent";
}
EXPORT JOB load_inc_userActivationEvent

END


DROP ALL

###############################################################################
#
# Vertices
#
###############################################################################
# 1. Event properties
#  - udid as PK
CREATE VERTEX udid (primary_id id string NOT NULL, gs_touch_time uint)
#  - imei as PK
CREATE VERTEX imei (primary_id id string NOT NULL, gs_touch_time uint)
#  - imsi as PK
CREATE VERTEX imsi (primary_id id string NOT NULL, gs_touch_time uint)
#  - mac as PK
CREATE VERTEX mac (primary_id id string NOT NULL, gs_touch_time uint)
#  - ip as PK
CREATE VERTEX ip (primary_id id string NOT NULL, gs_touch_time uint)
#  - account number as PK
CREATE VERTEX account (primary_id id string NOT NULL, gs_touch_time uint)
#  - csubnet as PK
CREATE VERTEX cSubnet(primary_id id string NOT NULL, gs_touch_time uint)
#  - fingerprint as PK
CREATE VERTEX fingerprint(primary_id id string NOT NULL, gs_touch_time uint)
#  - idfa as PK
CREATE VERTEX idfa(primary_id id string NOT NULL, gs_touch_time uint)

# 2. Events
#  - uuid as PK
CREATE VERTEX deviceInitEvent (primary_id id string NOT NULL, ts uint, gs_touch_time uint)
#  - uuid as PK
CREATE VERTEX userActivationEvent (primary_id id string NOT NULL, ts uint, gs_touch_time uint, os_type string, ref_id string )
#  - uuid as PK
CREATE VERTEX bizEvent (primary_id id string NOT NULL, ts uint, action_type string, ref_id uint, gs_touch_time uint)
#  - geohash value as PK
CREATE VERTEX geoHash(primary_id id string NOT NULL, gs_touch_time uint)

# 3. Time index, event indexed by date for retention
#  - Date as PK
CREATE VERTEX dateIndex (primary_id id string NOT NULL, gs_touch_time uint)
# 4. Stat. 
#  - Date+measurement as PK . eg. "2016-01-01:activationCnt"
CREATE VERTEX stat (primary_id id string NOT NULL, intval uint, gs_touch_time uint)

###############################################################################
#
# Edges
#
###############################################################################
# A. DeviceInitEvent
CREATE UNDIRECTED EDGE deviceInitEvent_udid (FROM deviceInitEvent, TO udid)
CREATE UNDIRECTED EDGE deviceInitEvent_imei (FROM deviceInitEvent, TO imei)
CREATE UNDIRECTED EDGE deviceInitEvent_imsi (FROM deviceInitEvent, TO imsi)
CREATE UNDIRECTED EDGE deviceInitEvent_mac (FROM deviceInitEvent, TO mac)
CREATE UNDIRECTED EDGE deviceInitEvent_fingerprint (FROM deviceInitEvent, TO fingerprint)
CREATE UNDIRECTED EDGE deviceInitEvent_idfa (FROM deviceInitEvent, TO idfa)
CREATE UNDIRECTED EDGE deviceInitEvent_ip (FROM deviceInitEvent, TO ip)
#
# B. bizEvent
CREATE UNDIRECTED EDGE bizEvent_udid (FROM bizEvent, TO udid)
CREATE UNDIRECTED EDGE bizEvent_imsi (FROM bizEvent, TO imsi)
CREATE UNDIRECTED EDGE bizEvent_imei (FROM bizEvent, TO imei)
CREATE UNDIRECTED EDGE bizEvent_mac (FROM bizEvent, TO mac)
CREATE UNDIRECTED EDGE bizEvent_fingerprint (FROM bizEvent, TO fingerprint)
CREATE UNDIRECTED EDGE bizEvent_idfa (FROM bizEvent, TO idfa)
CREATE UNDIRECTED EDGE bizEvent_account (FROM bizEvent, TO account)
CREATE UNDIRECTED EDGE bizEvent_geoHash (FROM bizEvent, TO geoHash)
CREATE UNDIRECTED EDGE bizEvent_ip (FROM bizEvent, TO ip)
CREATE UNDIRECTED EDGE bizEvent_date (FROM bizEvent, TO dateIndex)
#
# C. userActivationEvent
CREATE UNDIRECTED EDGE userActivationEvent_udid(FROM userActivationEvent, TO udid)
CREATE UNDIRECTED EDGE userActivationEvent_imsi (FROM userActivationEvent, TO imsi)
CREATE UNDIRECTED EDGE userActivationEvent_imei (FROM userActivationEvent, TO imei)
CREATE UNDIRECTED EDGE userActivationEvent_mac (FROM userActivationEvent, TO mac)
CREATE UNDIRECTED EDGE userActivationEvent_fingerprint (FROM userActivationEvent, TO fingerprint)
CREATE UNDIRECTED EDGE userActivationEvent_idfa (FROM userActivationEvent, TO idfa)
CREATE UNDIRECTED EDGE userActivationEvent_account (FROM userActivationEvent, TO account)
CREATE UNDIRECTED EDGE userActivationEvent_geoHash (FROM userActivationEvent, TO geoHash)
CREATE UNDIRECTED EDGE userActivationEvent_ip (FROM userActivationEvent, TO ip)
CREATE UNDIRECTED EDGE userActivationEvent_date (FROM userActivationEvent, TO dateIndex)
#
# D. Misc
CREATE UNDIRECTED EDGE ip_cSubnet (FROM ip, TO cSubnet) 
CREATE UNDIRECTED EDGE date_stat (FROM dateIndex, TO stat) 
###############################################################################
#
# Graph
#
###############################################################################
CREATE GRAPH bangcleGraph (udid,imei,imsi,mac,ip,account,cSubnet,fingerprint,idfa,deviceInitEvent,userActivationEvent,bizEvent,geoHash,dateIndex,stat,deviceInitEvent_udid,deviceInitEvent_imei,deviceInitEvent_imsi,deviceInitEvent_mac,deviceInitEvent_fingerprint,deviceInitEvent_idfa,deviceInitEvent_ip,bizEvent_udid,bizEvent_imsi,bizEvent_imei,bizEvent_mac,bizEvent_fingerprint,bizEvent_idfa,bizEvent_account,bizEvent_geoHash,bizEvent_ip,bizEvent_date,userActivationEvent_udid,userActivationEvent_imsi,userActivationEvent_imei,userActivationEvent_mac,userActivationEvent_fingerprint,userActivationEvent_idfa,userActivationEvent_account,userActivationEvent_geoHash,userActivationEvent_ip,userActivationEvent_date)
EXPORT SCHEMA bangcleGraph


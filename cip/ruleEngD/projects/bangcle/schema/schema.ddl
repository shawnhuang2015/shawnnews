DROP ALL

###############################################################################
#
# Vertices
#
###############################################################################

CREATE VERTEX udid (primary_id id string NOT NULL)
CREATE VERTEX imei (primary_id id string NOT NULL)
CREATE VERTEX imsi (primary_id id string NOT NULL)
CREATE VERTEX mac (primary_id id string NOT NULL)
#CREATE VERTEX bmac (primary_id id string NOT NULL)
CREATE VERTEX ip (primary_id id string NOT NULL)
CREATE VERTEX account (primary_id id string NOT NULL)
CREATE VERTEX c_netsegment(primary_id id string NOT NULL)
#
CREATE VERTEX deviceEvent (primary_id id string NOT NULL, ts uint)
CREATE VERTEX bizEvent (primary_id id string NOT NULL, ts uint, action_type string, ref_id uint)
CREATE VERTEX deviceGeo (primary_id id string NOT NULL, region uint)

###############################################################################
#
# Edges
#
###############################################################################
#
CREATE UNDIRECTED EDGE deviceEvent_udid (FROM deviceEvent, TO udid)
CREATE UNDIRECTED EDGE deviceEvent_imei (FROM deviceEvent, TO imei)
CREATE UNDIRECTED EDGE deviceEvent_imsi (FROM deviceEvent, TO imsi)
CREATE UNDIRECTED EDGE deviceEvent_mac (FROM deviceEvent, TO mac)
#
CREATE UNDIRECTED EDGE bizEvent_imei (FROM bizEvent, TO imei)
CREATE UNDIRECTED EDGE bizEvent_mac (FROM bizEvent, TO mac)
CREATE UNDIRECTED EDGE bizEvent_ip (FROM bizEvent, TO ip)
CREATE UNDIRECTED EDGE bizEvent_account (FROM bizEvent, TO account)
CREATE UNDIRECTED EDGE bizEvent_udid (FROM bizEvent, TO udid)
CREATE UNDIRECTED EDGE bizEvent_geo (FROM bizEvent, TO deviceGeo)
#
###############################################################################
#
# Graph
#
###############################################################################

CREATE GRAPH bangcleGraph (udid, imei, imsi, mac, ip, account, c_netsegment, deviceEvent, bizEvent, deviceGeo, deviceEvent_udid, deviceEvent_imei, deviceEvent_imsi, deviceEvent_mac, bizEvent_imei, bizEvent_mac, bizEvent_ip, bizEvent_account, bizEvent_udid, bizEvent_geo)

EXPORT SCHEMA bangcleGraph


import sys
sys.path.append("../")
from config.RuleEngConfig import ruleEngConfigure
########  common biz objects #########################
from rulebase.GPathBizObj import GPathBizObj
from rulebase.GPathBizObj import GPathConfig
############ User defined bizObjects ############
class BangcleDevEvtToMacImeiObj(object):
    def __init__(self, imsi, ts_start, ts_end):
        config = GPathConfig(ruleEngConfigure["gstore"]["host"], ruleEngConfigure["gstore"]["port"])
        gpath = "imsi[\"%s\"]-deviceEvent_imsi->deviceEvent-deviceEvent_mac->mac=>.project{mac._external_id}" % imsi 
        gpathObj = GPathBizObj(config, gpath)
        self.macList = gpathObj.getList()
        gpath = "imsi[\"%s\"]-deviceEvent_imsi->deviceEvent-deviceEvent_imei->imei=>.project{imei._external_id}"  % imsi
        gpathObj = GPathBizObj(config, gpath)
        self.imeiList = gpathObj.getList()

class BangcleBizEvtToUdidObj(object):
    def __init__(self, bizEvtId):
        udids = []
        config = GPathConfig(ruleEngConfigure["gstore"]["host"], ruleEngConfigure["gstore"]["port"])
        gpath = "bizEvent[\"%s\"]-bizEvent_imei->imei-udid_imei->udid=>.project{udid._external_id}" % bizEvtId 
        gpathObj = GPathBizObj(config, gpath)
        udids = gpathObj.getList()
        gpath = "bizEvent[\"%s\"]-bizEvent_mac->mac-udid_mac->udid=>.project{udid._external_id}" % bizEvtId 
        gpathObj = GPathBizObj(config, gpath)
        udids += gpathObj.getList()
        self.udids = udids

class BangcleAcctToBizObj(object):
    def __init__(self, account, biz_op_type, ts_start, ts_end):
        config = GPathConfig(ruleEngConfigure["gstore"]["host"], ruleEngConfigure["gstore"]["port"])
        gpath = "account[\"%s\"]-bizEvent_account->bizEvent{bizEvent.action_type = '%s'}=>.project{bizEvent._external_id}" % ( account , biz_op_type)
        gpathObj = GPathBizObj(config, gpath)
        self.bizEvtList = gpathObj.getList()


class BangcleIpToBizEvtObj(object):
    def __init__(self, ip, biz_op_type, ts_start, ts_end):
        config = GPathConfig(ruleEngConfigure["gstore"]["host"], ruleEngConfigure["gstore"]["port"])
        gpath = "ip[\"%s\"]-bizEvent_ip->bizEvent{bizEvent.action_type = '%s'}=>.project{bizEvent._external_id}" % (ip, biz_op_type)
        gpathObj = GPathBizObj(config, gpath)
        self.bizEvtList = gpathObj.getList()

class BangcleIpToActEvtObj(object):
    def __init__(self, ip, ts_start, ts_end):
        config = GPathConfig(ruleEngConfigure["gstore"]["host"], ruleEngConfigure["gstore"]["port"])
        gpath = "ip[\"%s\"]-userActivationEvent_ip->userActivationEvent=>.project{userActivationEvent._external_id}" % (ip)
        gpathObj = GPathBizObj(config, gpath)
        self.evtList = gpathObj.getList()

class BangcleImeiToBizEvtObj(object):
    def __init__(self, imei, biz_op_type, ts_start, ts_end):
        config = GPathConfig(ruleEngConfigure["gstore"]["host"], ruleEngConfigure["gstore"]["port"])
        gpath = "imei[\"%s\"]-bizEvent_imei->bizEvent{bizEvent.action_type = '%s'}=>.project{bizEvent._external_id}" % (imei, biz_op_type)
        gpathObj = GPathBizObj(config, gpath)
        self.bizEvtList = gpathObj.getList()

class BangcleFpToActEvtObj(object):
    def __init__(self, fingerprint, ts_start, ts_end):
        config = GPathConfig(ruleEngConfigure["gstore"]["host"], ruleEngConfigure["gstore"]["port"])
        gpath = "fingerprint[\"%s\"]-userActivationEvent_fingerprint->userActivationEvent=>.project{userActivationEvent._external_id}" % (fingerprint)
        gpathObj = GPathBizObj(config, gpath)
        self.evtList = gpathObj.getList()

class BangcleAcct_ActEvt_Fp_Obj(object):
    def __init__(self, account,ts_start, ts_end):
        config = GPathConfig(ruleEngConfigure["gstore"]["host"], ruleEngConfigure["gstore"]["port"])
        gpath = "account[\"%s\"]-userActivationEvent_account->userActivationEvent-userActivationEvent_fingerprint->fingerprint=>.project{fingerprint._external_id}" % (account)
        gpathObj = GPathBizObj(config, gpath)
        self.fpList = gpathObj.getList()

class BangcleGeo_ActEvt_Obj(object):
    def __init__(self, geo_hash, ts_start, ts_end):
        config = GPathConfig(ruleEngConfigure["gstore"]["host"], ruleEngConfigure["gstore"]["port"])
        gpath = "geoHash[\"%s\"]-userActivationEvent_geoHash->userActivationEvent=>.project{userActivationEvent._external_id}" % (geo_hash)
        gpathObj = GPathBizObj(config, gpath)
        self.evtList = gpathObj.getList()

class BangcleAcct_ActEvt_Udid_Obj(object):
    def __init__(self, account, ts_start, ts_end):
        config = GPathConfig(ruleEngConfigure["gstore"]["host"], ruleEngConfigure["gstore"]["port"])
        gpath = "account[\"%s\"]-userActivationEvent_account->userActivationEvent-userActivationEvent_udid->udid=>.project{userActivationEvent._external_id}" % (account)
        gpathObj = GPathBizObj(config, gpath)
        self.udidList = gpathObj.getList()

class BangcleCNetSegment_ActEvt_Obj(object):
    def __init__(self, cnetsegment, ts_start, ts_end):
        config = GPathConfig(ruleEngConfigure["gstore"]["host"], ruleEngConfigure["gstore"]["port"])
        gpath = "cSubnet[\"%s\"]-ip_cSubnet->ip-userActivationEvent_ip->userActivationEvent=>.project{userActivationEvent._external_id}" % (cnetsegment)
        gpathObj = GPathBizObj(config, gpath)
        self.evtList = gpathObj.getList()

class BangcleCNetSegment_Stat_Obj(object):
    def __init__(self, cnetsegment, datetag, measurement):
        config = GPathConfig(ruleEngConfigure["gstore"]["host"], ruleEngConfigure["gstore"]["port"])
        gpath = "cSubnet[\"%s\"]-cSubnet_stat[\"%s:%s\"]=>.project{bizEvent.intval}" % (cnetsegment, datetag, measurement)
        gpathObj = GPathBizObj(config, gpath)
        self.stat = gpathObj.getList()

class BangcleCSubnet_RefId_Obj(object):
    def __init__(self, csubnet, ts_start, ts_end):
        config = GPathConfig(ruleEngConfigure["gstore"]["host"], ruleEngConfigure["gstore"]["port"])
        gpath = "cSubnet[\"%s\"]-cSubnet_ip->ip-userActivationEvent_ip->userActivationEvent=>.project{userActivationEvent.ref_id}" % (csubnet)
        gpathObj = GPathBizObj(config, gpath)
        self.refIds = gpathObj.getList()

class BangcleGeo_RefId_Obj(object):
    def __init__(self, geoHash, ts_start, ts_end):
        config = GPathConfig(ruleEngConfigure["gstore"]["host"], ruleEngConfigure["gstore"]["port"])
        gpath = "geoHash[\"%s\"]-userActivationEvent_geoHash->userActivationEvent=>.project{userActivationEvent.ref_id}" % (geoHash)
        gpathObj = GPathBizObj(config, gpath)
        self.refIds = gpathObj.getList()

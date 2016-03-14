import sys
sys.path.append("../")
from config.RuleEngConfig import ruleEngConfigure
########  common biz objects #########################
from rulebase.GPathBizObj import GPathBizObj
from rulebase.GPathBizObj import GPathConfig
############ User defined bizObjects ############
class BangcleDevEvtToMacImeiObj(object):
    def __init__(self, imsi, ts_start, ts_end):
        config = GPathConfig(ruleEngConfigure["gstore"]["host"])
        gpath = "imsi[\"%s\"]-deviceEvent_imsi->deviceEvent-deviceEvent_mac->mac=>.project{mac._external_id}" % imsi 
        gpathObj = GPathBizObj(config, gpath)
        self.macList = gpathObj.getList()
        gpath = "imsi[\"%s\"]-deviceEvent_imsi->deviceEvent-deviceEvent_imei->imei=>.project{imei._external_id}"  % imsi
        gpathObj = GPathBizObj(config, gpath)
        self.imeiList = gpathObj.getList()

class BangcleBizEvtToUdidObj(object):
    def __init__(self, bizEvtId):
        udids = []
        config = GPathConfig(ruleEngConfigure["gstore"]["host"])
        gpath = "bizEvent[\"%s\"]-bizEvent_imei->imei-udid_imei->udid=>.project{udid._external_id}" % bizEvtId 
        gpathObj = GPathBizObj(config, gpath)
        udids = gpathObj.getList()
        gpath = "bizEvent[\"%s\"]-bizEvent_mac->mac-udid_mac->udid=>.project{udid._external_id}" % bizEvtId 
        gpathObj = GPathBizObj(config, gpath)
        udids += gpathObj.getList()
        self.udids = udids

class BangcleAcctToBizObj(object):
    def __init__(self, account, biz_op_type, ts_start, ts_end):
        config = GPathConfig(ruleEngConfigure["gstore"]["host"])
        gpath = "account[\"%s\"]-bizEvent_account->bizEvent{bizEvent.action_type = '%s'}=>.project{bizEvent._external_id}" % ( account , biz_op_type)
        gpathObj = GPathBizObj(config, gpath)
        self.bizEvtList = gpathObj.getList()


class BangcleIpToBizEvtObj(object):
    def __init__(self, ip, biz_op_type, ts_start, ts_end):
        config = GPathConfig(ruleEngConfigure["gstore"]["host"])
        gpath = "ip[\"%s\"]-bizEvent_ip->bizEvent{bizEvent.action_type = '%s'}=>.project{bizEvent._external_id}" % (ip, biz_op_type)
        gpathObj = GPathBizObj(config, gpath)
        self.bizEvtList = gpathObj.getList()

class BangcleIpToActEvtObj(object):
    def __init__(self, ip, ts_start, ts_end):
        config = GPathConfig(ruleEngConfigure["gstore"]["host"])
        gpath = "ip[\"%s\"]-userActivationEvent_ip->userActivationEvent=>.project{userActivationEvent._external_id}" % (ip)
        gpathObj = GPathBizObj(config, gpath)
        self.evtList = gpathObj.getList()

class BangcleImeiToBizEvtObj(object):
    def __init__(self, imei, biz_op_type, ts_start, ts_end):
        config = GPathConfig(ruleEngConfigure["gstore"]["host"])
        gpath = "imei[\"%s\"]-bizEvent_imei->bizEvent{bizEvent.action_type = '%s'}=>.project{bizEvent._external_id}" % (imei, biz_op_type)
        gpathObj = GPathBizObj(config, gpath)
        self.bizEvtList = gpathObj.getList()

class BangcleAcct_BizEvt_Udid_Obj(object):
    def __init__(self, account, biz_op_type, ts_start, ts_end):
        config = GPathConfig(ruleEngConfigure["gstore"]["host"])
        gpath = "account[\"%s\"]-bizEvent_account->bizEvent{bizEvent.action_type = '%s'}-bizEvent_udid->udid=>.project{bizEvent._external_id}" % (account, biz_op_type)
        gpathObj = GPathBizObj(config, gpath)
        self.udidList = gpathObj.getList()

class BangcleAcct_ActEvt_Udid_Obj(object):
    def __init__(self, account, ts_start, ts_end):
        config = GPathConfig(ruleEngConfigure["gstore"]["host"])
        gpath = "account[\"%s\"]-userActivationEvent_account->userActivationEvent-userActivationEvent_udid->udid=>.project{userActivationEvent._external_id}" % (account)
        gpathObj = GPathBizObj(config, gpath)
        self.udidList = gpathObj.getList()

class BangcleCNetSegment_ActEvt_Obj(object):
    def __init__(self, cnetsegment, ts_start, ts_end):
        config = GPathConfig(ruleEngConfigure["gstore"]["host"])
        gpath = "c_netsegment[\"%s\"]-c_netsegment_ip->ip-userActivationEvent_ip->userActivationEvent-userActivationEvent_udid->udid=>.project{userActivationEvent._external_id}" % (cnetsegment)
        gpathObj = GPathBizObj(config, gpath)
        self.evtList = gpathObj.getList()

class BangcleCNetSegment_Stat_Obj(object):
    def __init__(self, cnetsegment, datetag, measurement):
        config = GPathConfig(ruleEngConfigure["gstore"]["host"])
        gpath = "c_netsegment[\"%s\"]-c_netsegment_stat[\"%s:%s\"]=>.project{bizEvent.intval}" % (cnetsegment, datetag, measurement)
        gpathObj = GPathBizObj(config, gpath)
        self.stat = gpathObj.getList()

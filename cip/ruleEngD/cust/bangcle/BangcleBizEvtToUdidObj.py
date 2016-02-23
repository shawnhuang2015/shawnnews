import sys
sys.path.append("../")
########  common biz objects #########################
from rulebase.GPathBizObj import GPathBizObj
from rulebase.GPathBizObj import GPathConfig
############ User defined bizObjects ############
class BangcleDevEvtToMacImeiObj(object):
    def __init__(self, imsi, ts_start, ts_end):
        config = GPathConfig("192.168.33.70")
        gpath = "imsi[\"%s\"]-udid_imsi->deviceEvent-deviceEvent_mac->mac=>.project{mac._external_id}" % imsi 
        gpath = 
        gpathObj = GPathBizObj(config, gpath)
        self.macList = gpathObj.getList()
        gpath = "imsi[\"%s\"]-udid_imsi->deviceEvent-deviceEvent_imei->imei=>.project{imei._external_id}"  % imsi
        gpathObj = GPathBizObj(config, gpath)
        self.imeiList = gpathObj.getList()

class BangcleBizEvtToUdidObj(object):
    def __init__(self, bizEvtId):
        udids = []
        config = GPathConfig("192.168.33.70")
        gpath = "bizEvent[\"%s\"]-bizEvent_imei->imei-udid_imei->udid=>.project{udid._external_id}" % bizEvtId 
        gpathObj = GPathBizObj(config, gpath)
        udids = gpathObj.getList()
        gpath = "bizEvent[\"%s\"]-bizEvent_mac->mac-udid_mac->udid=>.project{udid._external_id}" % bizEvtId 
        gpathObj = GPathBizObj(config, gpath)
        udids += gpathObj.getList()
        self.udids = udids

class BangcleAcctToBizObj(object):
    def __init__(self, account, ts_start, ts_end):
        config = GPathConfig("192.168.33.70")
        gpath = "account[\"%s\"]-bizEvent_account->bizEvent=>.project{bizEvent._external_id}" % account 
        gpathObj = GPathBizObj(config, gpath)
        self.bizEvtList = gpathObj.getList()

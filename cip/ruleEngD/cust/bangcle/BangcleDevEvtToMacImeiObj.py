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
        gpathObj = GPathBizObj(config, gpath)
        self.macList = gpathObj.getList()
        gpath = "imsi[\"%s\"]-udid_imsi->deviceEvent-deviceEvent_imei->imei=>.project{imei._external_id}"  % imsi
        gpathObj = GPathBizObj(config, gpath)
        self.imeiList = gpathObj.getList()

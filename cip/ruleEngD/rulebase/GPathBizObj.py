import json
import sys
sys.path.append("../")
from logger.Decorators import logTxn
########  common biz objects #########################
from rulebase.BizObjBase import BizObjBase
from rulebase.JsonBizObj import JsonBizObj 
from rulebase.RestBizObj import RestPostBizObj 
############ User defined bizObjects ############
class GPathConfig(object):
    def __init__(self, host, port):
        self.host = host
        self.port = port

class GPathBizObj(object):
    @logTxn("DEBUG GPATH_REST")
    def __init__(self, config, gpath):
        arguments = { "query" : "%s as RESULT >> out" % gpath}
        self.result = RestPostBizObj("http://%s:%s/gpath" % (config.host, config.port), json.dumps(arguments) )

    def getList(self):
        if self.result.isValid == True and self.result.error == False:
            return self.result.results['RESULT']['data']

        return []

if __name__ == '__main__':
    config = GPathConfig("192.168.33.70",9000) 
    # gpath = "imsi[\"8986002615149\"]-udid_imsi->deviceEvent-deviceEvent_mac->mac=>.project{mac._external_id}"
    gpath = "imsi[\"8986002615149\", \"89860040221405\"]=>.project{imsi._external_id}"
    gpathObj = GPathBizObj(config, gpath)
    print gpathObj.getList()

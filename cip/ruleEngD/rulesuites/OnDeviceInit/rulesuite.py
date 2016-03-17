import json
import time, sys
sys.path.append("..")
from algorithm import RuleConfltSol 
###### common functions applied to context ###########
from ruletask.Context import RuleContext
from ruletask.Context import __REQ_ACTOR
from ruletask.Context import __REQ_CP 
from ruletask.Context import __REQ_EVT 
from ruletask.Context import __WS 
from ruletask.Context import __W_WS
from ruletask.Context import __LW_WS
from ruletask.Context import __W_RET
from ruletask.Context import __RET
from ruletask.Context import __W_RULE_RET
from ruletask.Context import __RULE_RET
########  common biz objects #########################
from rulebase.BizObjBase import BizObjBase
from rulebase.JsonBizObj import JsonBizObj 
from rulebase.RestBizObj import RestBizObj 
from util.func import __R_MERGE_DICT_ARRAY

########   performance   #########################
from logger.Decorators import Performance
from monitor.Perf import Perf

#########   customization biz objects ################
from cust.bangcle.BangcleBizObj import BangcleDevEvtToMacImeiObj

def resolve_conflict(context):
    # RuleConfltSol.default_sol_conflict(context)
    pass


def start(context):
    pass

def end(context):
    resolve_conflict(context)


############Data load segment#################

############Rule segment#################

def r1_same_imsi_finger(context):
    evt = __REQ_EVT(context)
    result = BangcleDevEvtToMacImeiObj(evt.imsi, evt.ts - 3600*24, evt.ts)

    # if len(result.imeiList) > 10 or  len(result.macList) > 10:
    # TODO: for demo purpose only
    if len(result.imeiList) > 2 or  len(result.macList) > 2:
        __W_RULE_RET(context , "ALERT: imsi-%s has more than 10 different eithor imsi:%d or mac:%d in past one day" %(evt.imsi, len(result.imeiList),len(result.macList)))
        perf.post({"rule":"r1_same_imsi_finger"}, 1.0)

def r2_same_imsiimei_diff_mac(context):
    pass
#      devEvt = __REQ_EVT(context)
    #  config = GPathConfig(ruleEngConfigure["gstore"]["host"])
    #  gpath = "imsi[\"%s\"]-deviceEvent_imsi->deviceEvent=>.project{deviceEvent._external_id}" % devEvt.id
    #  gpathObj = GPathBizObj(config, gpath)
    #  evts_same_imsi = gpathObj.getList()

    #  gpath = "imei[\"%s\"]-deviceEvent_imei-deviceEvent=>.project{deviceEvent._external_id}" % devEvt.id
    #  gpathObj = GPathBizObj(config, gpath)
    #  evts_same_imei = gpathObj.getList()

    #  # TODO provide util for set and or not
    #  evts = set(evts_same_imsi) & set(evts_same_imei) - set(devEvt.id)
    #  macs = set()
    #  # Require GPath to support input as set

    #  if len(evts) > 0:
        #  for evt in evts: 
            #  gpath = "deviceEvent[\"%s\"]-deviceEvent_mac-mac=>.project{mac._external_id}" % evt.id
            #  gpathObj = GPathBizObj(config, gpath)
            #  macs |= set(gpathObj.getList())

        #  if len(macs) > 10:
            #  __W_RULE_RET(context , "ALERT: imsi-%s, imei-%s has more than 10 different mac:%d " %(devEvt.imsi, devEvt.imei, len(macs)))

def r3_same_md5(context):
    pass
#      bizEvt = __REQ_EVT(context)
    #  config = GPathConfig(ruleEngConfigure["gstore"]["host"])
    #  gpath = "md5[\"%s\"]-bizEvent_md5-bizEvent-bizEvent_mac-mac=>.project{mac._external_id}" % bizEvt.id
    #  gpathObj = GPathBizObj(config, gpath)

    #  imacs = gpathObj.getList()
    #  gpath = "md5[\"%s\"]-bizEvent_md5-bizEvent-bizEvent_imei-imei=>.project{imei._external_id}" % bizEvt.id
    #  gpathObj = GPathBizObj(config, gpath)
    #  imeis = gpathObj.getList()

    #  if len(imeis)> 10 or len(imacs) > 10 :
        #  __W_RULE_RET(context , "ALERT: same md5 has more than 10 different either mac:%d  or imeis: %d" % (len(imacs), len(imeis))



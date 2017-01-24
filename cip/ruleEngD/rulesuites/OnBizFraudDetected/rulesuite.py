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
from cust.bangcle.BangcleBizObj import BangcleBizEvtToUdidObj 
from cust.bangcle.BangcleBizObj import BangcleAcctToBizObj 
from cust.bangcle.BangcleBizObj import BangcleIpToBizEvtObj 
from cust.bangcle.BangcleBizObj import BangcleImeiToBizEvtObj

#TODO: move global definition to Perf package itself
perf = Perf("192.168.33.70","rule_result")

def resolve_conflict(context):
    # RuleConfltSol.default_sol_conflict(context)
    pass


def start(context):
    pass

def end(context):
    resolve_conflict(context)


############Data load segment#################

############Rule segment#################

def r4_operation_without_fingerprint(context):
    bizEvt = __REQ_EVT(context)
    ret = BangcleBizEvtToUdidObj(bizEvt.bizEvtId)

    if ret.udids == []:
        __W_RULE_RET(context , "ALERT: operation without fingureprint, bizEvtid:%s" % bizEvt.bizEvtId)
        # TODO: make it as a rule action
        perf.post({"rule":"r4_operation_without_fingerprint"}, 1.0)
def r14_ip_query_frequency(context):
    return
    bizEvt = __REQ_EVT(context)
    now = int(time(time()))
    ret = BangcleIpToBizEvtObj(bizEvt.ip, "query", now, now-60) 
    bizEvts = ret.bizEvtList
    if len(bizEvts)>=300:
        __W_RULE_RET(context , "ALERT: ip %s has more than 300 query times: %s in past 1 minute" % (bizEvt.ip, len(bizEvts)))

def r18_imei_login_frequency(context):
    return
    bizEvt = __REQ_EVT(context)
    now = int(time(time()))
    ret = BangcleIpToBizEvtObj(bizEvt.imei, "login", now, now-60*60*24) 
    bizEvts = ret.bizEvtList
    if len(bizEvts)>=300:
        __W_RULE_RET(context , "ALERT: imei %s has more than 10 login events: %s in past 1 day" % (bizEvt.imei, len(bizEvts)))


def r20_gps_query_frequency(context):
    pass

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
from rulebase import util

########   performance   #########################
from logger.Decorators import Performance
from monitor.Perf import Perf

#########   customization biz objects ################
from cust.bangcle.BangcleBizObj import BangcleBizEvtToUdidObj 
from cust.bangcle.BangcleBizObj import BangcleCNetSegment_BizEvt_Obj 

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
    dataAggEvt = __REQ_EVT(context)
    ret = BangcleBizEvtToUdidObj(dataAggEvt.dataAggEvtId)

    if ret.udids == []:
        __W_RULE_RET(context , "ALERT: operation without fingureprint, dataAggEvtid:%s" % dataAggEvt.dataAggEvtId)
        # TODO: make it as a rule action
        perf.post({"rule":"r4_operation_without_fingerprint"}, 1.0)
def r21_short_signup(context):
    dataAggEvt = __REQ_EVT(context)
    if dataAggEvt.measurement == "signup_duration":
        if dataAggEvt.signup_end - dataAggEvt.signup_start > 10:
            __W_RULE_RET(context , "ALERT: to short signup duration for acct:%s" % dataAggEvt.account)
            perf.post({"rule":"r21_short_signup"}, 1.0)

def r24_jifeng_tuiguang_short(context):
    dataAggEvt = __REQ_EVT(context)
    if dataAggEvt.measurement == "jifengqiang_tuiguang_duration":
        if dataAggEvt.tuiguang - dataAggEvt.jifengqiang > 30:
            __W_RULE_RET(context , "ALERT: to short signup duration from tuiguang to jifengqiang" % dataAggEvt.account)
            perf.post({"rule":"r24_jifeng_tuiguang_short"}, 1.0)

def r25_same_c_netsegment_succid(context):
    dataAggEvt = __REQ_EVT(context)
    if dataAggEvt.measurement == "c_segment_id":
        # TODO
        # perf.post({"rule":"r25_same_c_netsegment_succid"}, 1.0)
        pass

def r26_same_gps_region_succid(context):
    dataAggEvt = __REQ_EVT(context)
    if dataAggEvt.measurement == "gps_id":
        # TODO
        # perf.post({"rule":"r26_same_gps_region_succid"}, 1.0)
        pass

def r27_same_c_netsegment_user_activate(context):
    dataAggEvt = __REQ_EVT(context)
    if dataAggEvt.measurement == "c_netsegment_user_activate":
        ret = BangcleCNetSegment_BizEvt_Obj(dataAggEvt.cnetsegment, "activate", ts_start, ts_end)
        bizEvts = ret.bizEvtList
        if len(bizEvts) > 100:
            __W_RULE_RET(context , "ALERT: to many user activation on  c network segment %s" % len(bizEvts))
            perf.post({"rule":"r27_same_c_netsegment_user_activate"}, 1.0)

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
    pass
def r18_imei_login_frequency(context):
    pass
def r19_diff_device_login(context):
    pass
def r20_gps_query_frequency(context):
    pass

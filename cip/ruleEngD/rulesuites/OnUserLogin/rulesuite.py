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
from cust.bangcle.BangcleBizObj import BangcleAcctToBizObj 

def resolve_conflict(context):
    # RuleConfltSol.default_sol_conflict(context)
    pass


def start(context):
    pass

def end(context):
    resolve_conflict(context)


############Data load segment#################

############Rule segment#################

def r8_frequent_login(context):
    bizEvt = __REQ_EVT(context)
    bizEvents = BangcleAcctToBizObj(bizEvt.account, bizEvt.ts - 5*60, bizEvt.ts)
    # if len(bizEvents.bizEvtList) > 10:
    # TODO: for demo only
    if len(bizEvents.bizEvtList) > 1:
        __W_RULE_RET(context, "ALERT:account %s login exceeds 10 in past 5 minutes with %s times login" % (bizEvt.account, len(bizEvents.bizEvtList)))
        perf.post({"rule":"r8_frequent_login"}, 1.0)

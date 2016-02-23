import json
import time, sys
sys.path.append("../")
from algorithm import RuleConfltSol 
###### common functions applied to context ###########
from Context import RuleContext
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
from rulebase.util.func  import __R_SUM_DICT_ARRAY
from rulebase.util.func  import __R_MERGE_DICT_ARRAY

#########   customization biz objects ################
from cust.bangcle.BangcleBizObj import BangcleBizEvtToUdidObj 

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
    udids = BangcleBizEvtToUdidObj(bizEvt.bizEvtId)

    if udids == []:
        __W_RULE_RET(context , "ALERT: operation without fingureprint")

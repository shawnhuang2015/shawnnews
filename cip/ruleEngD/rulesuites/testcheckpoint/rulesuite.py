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

#########   customization biz objects ################
from cust.sample.LakalaRecommendProxy import LakalaRecommendProxy

def use_my_conflict(context):
    prod1 = __RULE_RET(context,"rule2")
    prod2 = __RULE_RET(context,"rule3")
    prods = util.__R_MERGE_DICT_ARRAY(prod1, prod2) 
    __W_RET(context,prods)

def resolve_conflict(context):
    # RuleConfltSol.default_sol_conflict(context)
    use_my_conflict(context)


def start(context):
    pass

def end(context):
    resolve_conflict(context)


############Data load segment#################
def load1(context):
    recommend = LakalaRecommendProxy("general")
    __W_WS(context,"recommend_prods_general", recommend.products)

def load2(context):
    recommend = LakalaRecommendProxy("advanced")
    __W_WS(context, "recommend_prods_advanced", recommend.products)
    

def load3(context):
    pass

############Rule segment#################

def rule3(context):
    __W_RULE_RET(context, __WS(context,"recommend_prods_general"))

def rule2(context):
    __W_RULE_RET(context, __WS(context,"recommend_prods_advanced"))

def rule1(context):
    evt = __REQ_EVT(context)
    if evt.age == "86":
        __W_RULE_RET(context,"Approve")
    else:
        __W_RULE_RET(context,"Deny")

def rule4(context):
    __W_RULE_RET(context,"Approve")
    pass

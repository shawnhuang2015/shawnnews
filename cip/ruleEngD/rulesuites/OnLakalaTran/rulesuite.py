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
from cust.lakala.LakalaBizObj import lakalaBizObjManager
# TODO: Below two move to util
import datetime
import time

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
def r1_merchant_continuous_txn_fail(context):
    tranEvt = __REQ_EVT(context)
    # TODO: move it as a library
    if not hasattr(tranEvt, "merc_id"):
        return

    now = int(time.time())

    transCount = lakalaBizObjManager.getMerchantTrans(tranEvt.merc_id, "fail", now-60*60, now)
    if transCount > 3:
        __W_RULE_RET(context , "ALERT: merchant %s has too many fail trans(Cnt:%s) in short time" % (tranEvt.merc_id, transCount))
        perf.post({"rule":"r1_merchant_continuous_txn_fail"}, 1.0)

def r3_merchant_dc_amount_exceed(context):
    tranEvt = __REQ_EVT(context)
    # TODO: move it as a library
    if not hasattr(tranEvt, "card_no"):
        return

    now = int(time.time())

    totalAmount = lakalaBizObjManager.getMercCardTranAmount(tranEvt.card_no, "fail", now-60*60*24, now)
    if totalAmount > 3:
        __W_RULE_RET(context , "ALERT: merchant DC %s amount exceeds" % (tranEvt.card_no, totalAmount))
        perf.post({"rule":"r3_merchant_dc_amount_exceed"}, 1.0)

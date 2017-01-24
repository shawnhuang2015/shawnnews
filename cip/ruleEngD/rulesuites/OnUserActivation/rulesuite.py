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
from cust.bangcle.BangcleBizObj import BangcleCNetSegment_ActEvt_Obj 
from cust.bangcle.BangcleBizObj import BangcleCNetSegment_Stat_Obj 
from cust.bangcle.BangcleBizObj import BangcleIpToActEvtObj 
from cust.bangcle.BangcleBizObj import BangcleFpToActEvtObj
from cust.bangcle.BangcleBizObj import BangcleAcct_ActEvt_Fp_Obj
from cust.bangcle.BangcleBizObj import BangcleGeo_ActEvt_Obj
from cust.bangcle.BangcleBizObj import BangcleGeo_RefId_Obj
from cust.bangcle.BangcleBizObj import BangcleCSubnet_RefId_Obj

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
def r4_operation_without_fingerprint(context):
    activationEvt = __REQ_EVT(context)
    # TODO: move it as a library
    if not hasattr(activationEvt, "fingerprint") or activationEvt.fingerprint == "": 
        __W_RULE_RET(context , "ALERT: activation without fingureprint")
        perf.post({"rule":"r4_operation_without_fingerprint"}, 1.0)

def r14_ip_activation_frequency(context):
    actEvt = __REQ_EVT(context)
    if not hasattr(actEvt, "ip"):
        return
    now = int(time.time())
    ret = BangcleIpToActEvtObj(actEvt.ip, now, now-60) 
    evts = ret.evtList
    # TODO: change to 300
    if len(evts)>=2:
        __W_RULE_RET(context , "ALERT: ip %s has more than 300 query times: %s in past 1 minute" % (actEvt.ip, len(evts)))

def r18_fingerprint_activation_frequency(context):
    actEvt = __REQ_EVT(context)
    if not hasattr(actEvt, "fingerprint"):
        return
    now = int(time.time())
    # TODO: Validation of fingerprint
    ret = BangcleFpToActEvtObj(actEvt.fingerprint, now, now-60*60*24) 
    evts = ret.evtList
    # TODO: change to 300
    if len(evts)>=1:
        __W_RULE_RET(context , "ALERT: fingerprint %s has more than 10 login events: %s in past 1 day" % (actEvt.fingerprint, len(evts)))

def r19_diff_device_login(context):
    actEvt = __REQ_EVT(context)
    if not hasattr(actEvt, "account"):
        return
    now = int(time.time())
    ret = BangcleAcct_ActEvt_Fp_Obj(actEvt.account, now, now-60*60*24) 
    fpList  = ret.fpList
    if len(fpList)>=10:
        __W_RULE_RET(context , "ALERT: account %s has login more than 10 different devices: %s in past 1 day" % (actEvt.account, len(fpList)))

def r20_geo_activation_frequency(context):
    actEvt = __REQ_EVT(context)
    if not hasattr(actEvt, "geoHash"):
        return
    now = int(time.time())
    ret = BangcleGeo_ActEvt_Obj(actEvt.geoHash, now, now-60*60*24) 
    evtList  = ret.evtList
    if len(evtList)>=300:
        __W_RULE_RET(context , "ALERT: same geo area has more than 300 activation events : %s in past 1 minutes" % (len(evtList)))

#  def r4_operation_without_fingerprint(context):
    #  activationEvt = __REQ_EVT(context)
    #  ret = BangcleBizEvtToUdidObj(activationEvt.activationEvtId)

    #  if ret.udids == []:
        #  __W_RULE_RET(context , "ALERT: operation without fingureprint, activationEvtid:%s" % activationEvt.activationEvtId)
        #  # TODO: make it as a rule action
        #  perf.post({"rule":"r4_operation_without_fingerprint"}, 1.0)


def r21_short_signup(context):
    activationEvt = __REQ_EVT(context)
    if not (hasattr(activationEvt, "signup_end") and hasattr(activationEvt, "signup_start")):
        return
    if activationEvt.signup_end - activationEvt.signup_start < 10:
        __W_RULE_RET(context , "ALERT: to short signup duration for acct:%s" % activationEvt.account)
        perf.post({"rule":"r21_short_signup"}, 1.0)

def r22_short_typing_gap(context):
    activationEvt = __REQ_EVT(context)
    if not hasattr(activationEvt, "typing_gap_list"):
        return
    typing_gap_list = activationEvt.typing_gap_list
    for gap in typing_gap_list:
        if gap > 0.2:
            return

    __W_RULE_RET(context , "ALERT: all typing gaps are with 200 ms")

def r23_typing_gap_vibiration(context):
    activationEvt = __REQ_EVT(context)
    if not hasattr(activationEvt, "typing_gap_list"):
        return
    typing_gap_list = activationEvt.typing_gap_list
    if len(typing_gap_list) <= 1:
        return

    for gap in typing_gap_list:
        if gap > 0.3:
            return

    for i in range(1,len(typing_gap_list)):
        if not (typing_gap_list[i-1]*0.8 <= typing_gap_list[i] and typing_gap_list[i] <= typing_gap_list[i-1]*1.2):
            return

    __W_RULE_RET(context , "ALERT: all typing gaps are within 20% vibration")

def r24_jifeng_tuiguang_short(context):
    activationEvt = __REQ_EVT(context)
    if not (hasattr(activationEvt, "tuiguang") and hasattr(activationEvt, "jifengqiang")):
        return
    if activationEvt.tuiguang - activationEvt.jifengqiang < 30:
        __W_RULE_RET(context , "ALERT: to short signup duration from tuiguang to jifengqiang account:%s" % activationEvt.account)
        perf.post({"rule":"r24_jifeng_tuiguang_short"}, 1.0)

def r25_same_c_netsegment_succid(context):
    activationEvt = __REQ_EVT(context)
    if not hasattr(activationEvt, "cSubnet"):
        return

    now = int(time.time())
    ret = BangcleCSubnet_RefId_Obj(activationEvt.cSubnet, now, now - 3600 *24 )
    refIds = ret.refIds
    if len(refIds) < 2:
        return

    refIds.sort()
    succ = 0
    for i in range(1, len(refIds)):
        if refIds[i] == refIds[i-1] + 1:
            succ = succ + 1
            if succ == 5:
                __W_RULE_RET(context , "ALERT: 5 consecutive ref_id detected for csubnet: %s - %s" % ( activationEvt.cSubnet, refIds[i-4 : i] ))

        else:
            succ = 0
    
def r26_same_gps_region_succid(context):
    activationEvt = __REQ_EVT(context)
    if not hasattr(activationEvt, "geoHash"):
        return
    now = int(time.time())
    ret = BangcleGeo_RefId_Obj(activationEvt.geoHash, now, now - 3600 *24 )
    refIds = ret.refIds
    if len(refIds) < 2:
        return

    refIds.sort()
    succ = 0
    for i in range(1, len(refIds)):
        if refIds[i] == refIds[i-1] + 1:
            succ = succ + 1
            if succ == 5:
                __W_RULE_RET(context , "ALERT: 5 consecutive ref_id detected for geoHash: %s - %s" % ( activationEvt.geoHash, refIds[i-4 : i] ))

        else:
            succ = 0

def r27_same_c_netsegment_user_activate(context):

    activationEvt = __REQ_EVT(context)
    if not hasattr(activationEvt, "cSubnet"):
        return
    dt = datetime.date.today()
    dt = datetime.datetime(dt.year, dt.month, dt.day)
    unixtime = time.mktime(dt.timetuple())
    ts0 = int(unixtime)
    hour2 = ts0 + 2*3600
    hour6 = ts0 + 6*3600

    ret = BangcleCNetSegment_ActEvt_Obj(activationEvt.cSubnet,  hour2, hour6)
    evts = ret.evtList
    if len(evts) > 100:
        __W_RULE_RET(context , "ALERT: to many user activation on  c network segment %s" % len(evts))
        perf.post({"rule":"r27_same_c_netsegment_user_activate"}, 1.0)

def r28_cnet_activation_increase(context):
    activationEvt = __REQ_EVT(context)
    if not hasattr(activationEvt, "cSubnet"):
        return
    dt = datetime.date.today()
    datetag = "%s-%s-%s" % (dt.year, dt.month, dt.day)
    ret = BangcleCNetSegment_Stat_Obj(activationEvt.cSubnet, datetag, "dailyactivation")
    curVolume = ret.GetVal()
    maxVal = 0
    for day in range (1, 6):
        datetag = datetime.datetime.fromtimestamp(time.time() - day * 24*3600).strftime('%Y-%m-%d')
        ret = BangcleCNetSegment_Stat_Obj(activationEvt.cSubnet,datetag, "dailyactivation")
        volume = ret.stat()
        maxVal = max(maxVal, volume)

    if curVolume > maxVal* 5:
        __W_RULE_RET(context , "ALERT: activation volume increase sharply on csubnet:%s" % activationEvt.cSubnet)
        perf.post({"rule":"r28_cnet_activation_increase"}, 1.0)



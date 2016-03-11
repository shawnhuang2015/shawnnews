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
from cust.bangcle.BangcleBizObj import BangcleCNetSegment_Stat_Obj 

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

def r19_diff_device_login(context):
    return
    bizEvt = __REQ_EVT(context)
    now = int(time(time()))
    ret = BangcleAcct_BizEvt_Udid_Obj(bizEvt.account, "login", now, now-60*60*24) 
    udidLists  = ret.udidList
    if len(udidList)>=10:
        __W_RULE_RET(context , "ALERT: account %s has login more than 10 different devices: %s in past 1 day" % (bizEvt.account, len(bizEvts)))

def r20_gps_query_frequency(context):
    pass
def r4_operation_without_fingerprint(context):
    activationEvt = __REQ_EVT(context)
    ret = BangcleBizEvtToUdidObj(activationEvt.activationEvtId)

    if ret.udids == []:
        __W_RULE_RET(context , "ALERT: operation without fingureprint, activationEvtid:%s" % activationEvt.activationEvtId)
        # TODO: make it as a rule action
        perf.post({"rule":"r4_operation_without_fingerprint"}, 1.0)
def r21_short_signup(context):
    activationEvt = __REQ_EVT(context)
    if activationEvt.measurement == "signup_duration":
        if activationEvt.signup_end - activationEvt.signup_start > 10:
            __W_RULE_RET(context , "ALERT: to short signup duration for acct:%s" % activationEvt.account)
            perf.post({"rule":"r21_short_signup"}, 1.0)

def r24_jifeng_tuiguang_short(context):
    activationEvt = __REQ_EVT(context)
    if activationEvt.measurement == "jifengqiang_tuiguang_duration":
        if activationEvt.tuiguang - activationEvt.jifengqiang > 30:
            __W_RULE_RET(context , "ALERT: to short signup duration from tuiguang to jifengqiang" % activationEvt.account)
            perf.post({"rule":"r24_jifeng_tuiguang_short"}, 1.0)

def r25_same_c_netsegment_succid(context):
    activationEvt = __REQ_EVT(context)
    if activationEvt.measurement == "c_segment_id":
        # TODO
        # perf.post({"rule":"r25_same_c_netsegment_succid"}, 1.0)
        pass

def r26_same_gps_region_succid(context):
    activationEvt = __REQ_EVT(context)
    if activationEvt.measurement == "gps_id":
        # TODO
        # perf.post({"rule":"r26_same_gps_region_succid"}, 1.0)
        pass

def r27_same_c_netsegment_user_activate(context):
    activationEvt = __REQ_EVT(context)
    if activationEvt.measurement == "c_netsegment_user_activate":
        dt = datetime.date.today()
        dt = datetime.datetime(dt.year, dt.month, dt.day)
        unixtime = time.mktime(dt.timetuple())
        ts0 = int(unixtime)
        hour2 = ts0 + 2*3600
        hour6 = ts0 + 6*3600

        ret = BangcleCNetSegment_BizEvt_Obj(activationEvt.cnetsegment, "activate", hour2, hour6)
        bizEvts = ret.bizEvtList
        if len(bizEvts) > 100:
            __W_RULE_RET(context , "ALERT: to many user activation on  c network segment %s" % len(bizEvts))
            perf.post({"rule":"r27_same_c_netsegment_user_activate"}, 1.0)

def r28_cnet_activation_increase(context):
    activationEvt = __REQ_EVT(context)
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



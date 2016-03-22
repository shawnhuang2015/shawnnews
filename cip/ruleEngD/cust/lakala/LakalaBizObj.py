import sys
sys.path.append("../")
sys.path.append("../..")
from config.RuleEngConfig import ruleEngConfigure
########  common biz objects #########################
from rulebase.GPathBizObj import GPathBizObj
from rulebase.GPathBizObj import GPathConfig
############ User defined bizObjects ############
class LakalaBizObjManager(object):
    def __init__(self):
        self.config = GPathConfig(ruleEngConfigure["gstore"]["host"], ruleEngConfigure["gstore"]["port"])

    def getMerchantTrans(self, merc_id, tran_status, ts_start, ts_end):
        gpath = "Merchant[\"%s\"]-Merchant_Txn->Txn{trans_status == \"%s\"  and trans_time > %s and trans_time < %s }=>.group{1 as all}[COUNT(Txn._internal_id) as cnt]" % (merc_id, tran_status, ts_start, ts_end)
        gpathObj = GPathBizObj(self.config, gpath)
        count = 0
        if gpathObj.getList() != []:
            count = gpathObj.getList()[0][1]
        return count

    def getMercCardTranAmount(self, card_no, ts_start, ts_end):
        gpath = "MercCard[\"%s\"]-CardApp_MercCard->CardApp-Merchant_CardApp->Merchant-Merchant_Txn->Txn{Txn.trans_time > %s and Txn.trans_end < %s}=>.group{1 as all}[SUM(Txn.trans_amt) as amount]" %s (card_no, ts_start, ts_end)
        gpathObj = GPathBizObj(self.config, gpath)
        amount = 0
        if gpathObj.getList() != []:
            amount = gpathObj.getList()[0][1]
        return amount

    def getMerchantTransCntAmt(self, merc_id, tran_status, ts_start, ts_end):
        count = 0
        amount = 0
        gpath = "Merchant[\"%s\"]-Merchant_Txn->Txn{trans_time > %s and trans_time < %s }=>.group{1 as all}[COUNT(Txn._internal_id) as cnt]" % (merc_id, tran_status, ts_start, ts_end)
        gpathObj = GPathBizObj(self.config, gpath)
        if gpathObj.getList() != []:
            count = gpathObj.getList()[0][1]

        gpath = "Merchant[\"%s\"]-Merchant_Txn->Txn{trans_time > %s and trans_time < %s }=>.group{1 as all}[SUM(Txn.trans_amt) as amount]" % (merc_id, tran_status, ts_start, ts_end)
        gpathObj = GPathBizObj(self.config, gpath)
        if gpathObj.getList() != []:
            amount = gpathObj.getList()[0][1]
        return count, amount

    def getSmallDupTranAmount(self, merc_id, minVal, maxVal, ts_start, ts_end):
        gpath = "Merchant[\"%s\"]-Merchant_Txn->Txn{trans_amt > minAmt and trans_amt < maxAmt and trans_time > %s and trans_time < %s }=>.group{trans_amt}[COUNT(Txn._internal_id) as cnt]" % (merc_id, minVal, maxVal,ts_start, ts_end)
        gpathObj = GPathBizObj(self.config, gpath)
        result = {}
        for item in gpathObj.getList():
            result[item[0]] = item[1]

        return result

lakalaBizObjManager = LakalaBizObjManager()

if __name__ == "__main__":
    config = GPathConfig(ruleEngConfigure["gstore"]["host"], ruleEngConfigure["gstore"]["port"])
    # The gpath -- return two vlaues -- is only supported after v4.3
    # gpath = "geoHash[\"uxvpgrypyxyzbpuxuzbrbxgzzzzzzzzz\"]-userActivationEvent_geoHash->userActivationEvent{ts == 0 and gs_touch_time == 0}=>.group{1 as all}[COUNT(userActivationEvent.ts) as cnt, SUM(userActivationEvent.ts) as sumVal]"
    gpath = "geoHash[\"uxvpgrypyxyzbpuxuzbrbxgzzzzzzzzz\"]-userActivationEvent_geoHash->userActivationEvent{ts == 0 and gs_touch_time == 0}=>.group{userActivationEvent._external_id}[COUNT(userActivationEvent.ts) as cnt]"
    gpathObj = GPathBizObj(config, gpath)
    for item in  gpathObj.getList():
        print "key:%s, cnt:%s" % (item[0], item[1])

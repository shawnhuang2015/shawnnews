import sys
sys.path.append("../")
from config.RuleEngConfig import ruleEngConfigure
########  common biz objects #########################
from rulebase.GPathBizObj import GPathBizObj
from rulebase.GPathBizObj import GPathConfig
############ User defined bizObjects ############
class LakalaBizObjManager(object):
    def __init__(self):
        self.config = GPathConfig(ruleEngConfigure["gstore"]["host"], ruleEngConfigure["gstore"]["port"])

    def getMerchantTrans(self, merc_id, tran_status, ts_start, ts_end):
        gpath = "Merchant[\"%s\"]-Merchant_Txn-Txn{Txn.trans_status=\"%s\"  and Txn.trans_time > %s and  Txn.trans_end < %s}{COUNT(Txn._external_id)}" %s (merc_id, tran_status, ts_start, ts_end)
        gpathObj = GPathBizObj(self.config, gpath)
        return gpathObj.getList()[0]

    def getMercCardTranAmount(self, card_no, ts_start, ts_end):
        gpath = "MercCard[\"%s\"]-CardApp_MercCard-CardApp-Merchant_CardApp-Merchant-Merchant_Txn-Txn{Txn.trans_time > %s and Txn.trans_end < %s}{SUM(Txn.trans_amt)}" %s (merc_id, tran_status, ts_start, ts_end)
        gpathObj = GPathBizObj(self.config, gpath)
        return gpathObj.getList()[0]

lakalaBizObjManager = LakalaBizObjManager()

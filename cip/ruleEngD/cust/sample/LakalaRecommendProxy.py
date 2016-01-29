import sys
sys.path.append("../")
########  common biz objects #########################
from rulebase.BizObjBase import BizObjBase
from rulebase.JsonBizObj import JsonBizObj 
from rulebase.RestBizObj import RestBizObj 
############ User defined bizObjects ############
class LakalaRecommendProxy(object):
    def __init__(self, algorithm):
        arguments = '{"alg":"%s"}' % algorithm
        recommend = RestBizObj("http://192.168.33.70:5000/cip/api/2.0/recommend", arguments)
        if recommend.isValid == True:
            self.products = recommend.products

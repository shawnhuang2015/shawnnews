import sys
sys.path.append("../")
from init.Env import initEnv
initEnv()

from flask import Flask, jsonify,request
import json
sys.path.append("../ruletask")
from RuleSuiteManager import RuleSuiteManager
sys.path.append("../rulebase")
sys.path.append("../monitor")
import time
from BizObjBase import BizObjBase
from RestBizObj import RestBizObj
from JsonBizObj import JsonBizObj
from threading import Timer
from init.Const import REST_OK
from init.Const import REST_BAD_REQ
from init.Const import REST_INTERNAL_ERROR
sys.path.append("../logging")
from Decorators import logTxn
from Decorators import Performance 
sys.path.append("..")
from monitor.Perf import Perf
from util.ErrHandler import buildAppResult 
from validator.validator import appSchemaValidator 


#TODO:config
ruleRoot="/home/feng.chen/gitrepo/product/cip/ruleEngD/rulesuites"

app = Flask(__name__)

rsm = RuleSuiteManager(ruleRoot, 10) 
perf = Perf("192.168.33.70","sla")


#  def reloadRuleSuite():
    #  print "reloading ..........."
    #  global rsm
    #  rsm = RuleSuiteManager(ruleRoot, 10) 
    #  Timer(3600*12, reloadRuleSuite,()).start()

#  reloadRuleSuite()

def appRespond(result):
    return jsonify({"result":result})

@app.route('/cip/api/1.0/ruleng', methods = ['POST'])
@logTxn("API_CALL RULE_EXECUTE")
@Performance(perf,{"API_CALL":"RULE_EXECUTE"})
def execute():
    global rsm
    appSchemaValidator.validate(request.data)
    if appSchemaValidator.errors != []:
        return appRespond(buildAppResult(REST_BAD_REQ, "Invalid request " + "\n".join(appSchemaValidator.errors)))

    req = json.loads(request.data)
    try:
        ret = rsm.runWithRequest(request = req)
        return appRespond(buildAppResult(REST_OK, data = ret))
    except Exception as e:
        print e
        return appRespond(buildAppResult(REST_INTERNAL_ERROR, e.message, e))
    finally:
        pass



########################Below for test purpose only #######################
@app.route('/testrest', methods = ['GET'])
def test():
    content = {"products":[{"item":"cup"},{"item":"bottle"},{"item":"ipad"}]}
    return jsonify(content)

@app.route('/cip/api/2.0/recommend', methods = ['GET'])
def recommend():
    req = json.loads(request.data)
    content = {"products":[{"item":"cup"},{"item":"bottle"},{"item":"beer"}]}
    if req["alg"] == "advanced":
        content = {"products":[{"item":"Benz"},{"item":"Palace"},{"item":"Girls"}]}
    return jsonify(content)

@app.route('/cip/api/4.0/gstore', methods = ['GET'])
def gstore():
    content = {"products":[{"item":"cup"},{"item":"bottle"},{"item":"ipad"}]}
    return jsonify(content)

if __name__ == '__main__':
    app.run(debug=False, host="0.0.0.0", processes=10)


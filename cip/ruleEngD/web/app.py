import sys
sys.path.append("../")

import json
import time
from threading import Timer
from flask import Flask, jsonify,request

from ruletask.RuleSuiteManager import RuleSuiteManager
from rulebase.BizObjBase import BizObjBase
from rulebase.RestBizObj import RestBizObj
from rulebase.JsonBizObj import JsonBizObj
from init.Const import REST_OK
from init.Const import REST_BAD_REQ
from init.Const import REST_INTERNAL_ERROR
from monitor.Perf import Perf
from util.ErrHandler import buildAppResult 
from validator.validator import appSchemaValidator 
from logger.Decorators import logTxn
from logger.Decorators import Performance 

try:
    from config.RuleEngConfig import ruleEngConfigure
except (ImportError, Exception) as e:
    print "%s, exit." % e.message 
    sys.exit (1)

app = Flask(__name__)

rsm = RuleSuiteManager(ruleEngConfigure["rule"]["root"], ruleEngConfigure["rule"]["ck_capacity"]) 
perf = Perf(ruleEngConfigure["influxdb"]["host"],ruleEngConfigure["performance"]["sla"])


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
        return appRespond(buildAppResult(REST_INTERNAL_ERROR, data = str(e)))
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


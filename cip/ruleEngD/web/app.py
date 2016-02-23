import sys
sys.path.append("../")
from init.Env import initEnv
initEnv()

from flask import Flask, jsonify,request
import json
sys.path.append("../ruletask")
from RuleSuiteManager import RuleSuiteManager
sys.path.append("../rulebase")
from BizObjBase import BizObjBase
from RestBizObj import RestBizObj
from JsonBizObj import JsonBizObj
from threading import Timer
from init.Const import REST_OK
from init.Const import REST_BAD_REQ
from init.Const import REST_INTERNAL_ERROR
sys.path.append("../logging")
from Decorators import logTxn


#TODO:config
ruleRoot="/home/feng.chen/gitrepo/product/cip/ruleEngD/rulesuites"

app = Flask(__name__)

rsm = RuleSuiteManager(ruleRoot, 10) 


def reloadRuleSuite():
    print "reloading ..........."
    global rsm
    rsm = RuleSuiteManager(ruleRoot, 10) 
    Timer(3600*12, reloadRuleSuite,()).start()

reloadRuleSuite()

def buildRestAck(code, summary = None, data = None):
    pkg = {
            "code":code,
            "summary":summary,
            "data":data
            }
    return jsonify({"result":pkg})
    


def validateRequest(req):
    #TODO
    return True

@app.route('/cip/api/1.0/ruleng', methods = ['POST'])
@logTxn("API_CALL RULE_EXECUTE")
def execute():
    global rsm
    req = json.loads(request.data)
    if not validateRequest(request):
        return buildRestAck(REST_BAD_REQ, "Invalid request " + request.data)
    try:
        ret = rsm.runWithRequest(request = req)
        return buildRestAck(REST_OK, data = ret)
    except Exception as e:
        print e
        return buildRestAck(REST_INTERNAL_ERROR, e.message, e)


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


from flask import Flask, jsonify,request
import sys
import json

sys.path.append("../ruletask")
from RuleSuiteManager import RuleSuiteManager
sys.path.append("../rulebase")
from BizObjBase import BizObjBase
from RestBizObj import RestBizObj
from JsonBizObj import JsonBizObj

#TODO:config
ruleRoot="/home/feng.chen/gitrepo/product/cip/ruleEngD/rulesuites"

app = Flask(__name__)

def validateRequest(req):
    #TODO
    pass

@app.route('/cip/api/1.0/ruleng', methods = ['POST'])
def execute():
    # Parse request json to Python Dict
    req = json.loads(request.data)
    validateRequest(request)
    rsm = RuleSuiteManager(ruleRoot, 10) 
    ret = rsm.runWithRequest(request = req)
    return jsonify({"result":ret})


@app.route('/testrest', methods = ['GET'])
def test():
    content = {"products":[{"item":"cup"},{"item":"bottle"},{"item":"ipad"}]}
    return jsonify(content)


if __name__ == '__main__':
    app.run(debug=True, host="0.0.0.0", processes=10)


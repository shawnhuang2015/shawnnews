from flask import Flask, jsonify,request
import sys
import json

sys.path.append("../ruletask")
from RuleSuiteManager import RuleSuiteManager

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
    # Fetch checkpoint name
    checkpoint = req['rule_suite']['check_point']
    rsm = RuleSuiteManager(ruleRoot, 10) 
    ret = rsm.runWithContext(checkpoint, requestContext = req)
    return jsonify({"result":ret.get("result")})


if __name__ == '__main__':
    app.run(debug=True, host="0.0.0.0", processes=10)


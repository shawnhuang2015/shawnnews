import zipfile
import yaml
import os
import threading
from RuleTaskBuilder import RuleTaskBuilder
from Context import RuleContext
import sys
sys.path.append("../rulebase")
from BizObjBase import BizObjBase

class RuleSuiteProcessor(object):
    # timeout should never be 0 
    def __init__(self, cpRoot, timeout):
        self.timeout = timeout
        if self.timeout <= 0:
            self.timeout = 10

        self.waitToFinish = threading.Event()
        self.context = RuleContext({
                "__WS__": {},
                "__RET__" : {}
                })
 

        self.cpRoot = cpRoot

        self.loadRuleSuiteMeta()
        self.jobs = self.loadRuleSuite()

    def loadRuleSuite(self):
        builder = RuleTaskBuilder()
        builder.loadRuleSuite(self.cpRoot, "rulesuite")
        builder.loadTopo("%s/%s" % (self.cpRoot, "topo.yaml"))
        builder.build(self.waitToFinish,self.context)
        return builder.jobTopo

    def loadRuleSuiteMeta(self):
        ruleMeta = yaml.load(open("%s/rulesuite.meta" % self.cpRoot).read())
        self.context.ctx["__RULE_META__"] = BizObjBase(ruleMeta)

    def populateCtxWithReq(self, actor, cp, event):
        self.context.ctx["__REQ_ACTOR__"] = actor
        self.context.ctx["__REQ_CP__"] = cp
        self.context.ctx["__REQ_EVT__"] = event

    def execute(self):
        self.jobs["start"].start()
        self.waitToFinish.wait(self.timeout)

    def getResult(self):
        return self.context.ctx["__RET__"]

    def clrResult(self):
        self.context.ctx["__RET__"] = None
        self.context.ctx["__WS__"] = None
        self.context.ctx["__REQ_ACTOR__"] = None
        self.context.ctx["__REQ_CP__"] = None
        self.context.ctx["__REQ_EVT__"] = None

if __name__ == '__main__':

    context = RuleContext({
        "__REQ_ACTOR__" : BizObjBase({}),
        "__REQ_CP__" : BizObjBase({}),
        "__REQ_EVT__" : BizObjBase({}),
        "__WS__" : {},
        "__RET__" : {}
        })
    processor = RuleSuiteProcessor("/home/feng.chen/gitrepo/product/cip/ruleEngD/rulesuites/testcheckpoint",timeout=9)
    processor.execute()
    print processor.getResult()

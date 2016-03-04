import os,time
from multiprocessing import Process
from RuleSuiteProcessor import RuleSuiteProcessor
import sys
sys.path.append("..")
from rulebase.BizObjBase import BizObjBase
from monitor.Perf import Perf

#TODO: change to config
defaultTimeOut=10
class RuleSuiteManager(object):
    '''Caching ruleSuiteProcessors '''
    def __init__(self, root, capacity):
        self.ruleroot = root
        self.capacity = capacity
        self.processors= {}
        cnt = 0
        for checkpoint in os.listdir(root):
            cnt += 1
            if cnt > self.capacity:
                break

            self.processors[checkpoint] = RuleSuiteProcessor("%s/%s" %(self.ruleroot, checkpoint),timeout=defaultTimeOut);

    def validate(self, cp):
        return os.path.isdir("%s/%s" %(self.ruleroot, cp))

    def runWithRequest(self, request):
        # TODO: hard code
        perf = Perf("192.168.33.70","sla")
        start = time.time()
        actor =  BizObjBase(request["actor"])
        cp = BizObjBase(request["checkpoint"])
        event  = BizObjBase(request["event"])

        if self.validate(cp.name) == False:
            raise Exception("Invalid check point %s" % cp.name)

        if not cp.name in self.processors.keys():
            if len(self.processors) == self.capacity:
                self.processors.popitem()
            self.processors[cp.name] = RuleSuiteProcessor("%s/%s" %(self.ruleroot, cp.name), timeout=defaultTimeOut);

        self.processors[cp.name].populateCtxWithReq(actor,cp,event)
        self.processors[cp.name].execute()
        ret = self.processors[cp.name].getResult()
        self.processors[cp.name].clrResult()

        end = time.time()
        perf.post({"checkpoint":request["checkpoint"]["name"]}, end-start)

        return ret

if __name__ == "__main__":
    rsm = RuleSuiteManager("/home/feng.chen/gitrepo/product/cip/ruleEngD/rulesuites", 10) 
    req = {
        'actor':'hello',
        'checkpoint':{'name':'testcheckpoint'},
        'event':'test'
        }
    ret = rsm.runWithRequest( request = req)
    print ret

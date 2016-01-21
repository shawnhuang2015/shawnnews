import os
from multiprocessing import Process
from RuleSuiteProcessor import RuleSuiteProcessor

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
            print  checkpoint
            cnt += 1
            if cnt > self.capacity:
                break
            self.processors[checkpoint] = RuleSuiteProcessor("%s/%s" %(self.ruleroot, checkpoint),timeout=defaultTimeOut);

    def runWithContext(self, checkpoint, requestContext = {'context':{}}):
        if not checkpoint in self.processors.keys():
            if len(self.processors) == self.capacity:
                self.processors.popitem()
            self.processors[checkpoint] = RuleSuiteProcessor("%s/%s" %(self.ruleroot, checkpoint),timeout=defaultTimeOut);

        self.processors[checkpoint].context.set('requestContext', requestContext)
        self.processors[checkpoint].execute()
        #TODO: only need return result
        return self.processors[checkpoint].context.context

if __name__ == "__main__":
    rsm = RuleSuiteManager("/home/feng.chen/gitrepo/product/cip/ruleEngD/rulesuites", 10) 
    ret = rsm.runWithContext("testcheckpoint", requestContext = {'context': {'hello':'world'}})
    print ret

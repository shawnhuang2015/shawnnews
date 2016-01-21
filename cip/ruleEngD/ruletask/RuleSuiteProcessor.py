import zipfile
import yaml
import os
import threading
from RuleTaskBuilder import RuleTaskBuilder
from Context import RuleContext

class RuleSuiteProcessor(object):
    '''Caching rule suite, should be multi processing mode, each RuleSuiteProcessor wait until all the 
    spawned threads completed and return''' 
    # timeout should not be 0 
    def __init__(self, checkpointFolder, timeout):
        checkpoint = os.path.split(checkpointFolder)[-1]

        self.waitToFinish = None
        self.timeout = timeout
        if self.timeout <= 0:
            self.timeout = 10

        self.waitToFinish = threading.Event()
        self.context = RuleContext() 
        self.context.set("result",{})
        self.context.set("ruleMeta",{})
        self.jobs = self.loadRuleSuite(checkpointFolder)

    def loadRuleSuite(self, checkpointFolder):
        builder = RuleTaskBuilder()
        builder.loadRuleSuite(checkpointFolder, "rulesuite")
        builder.loadTopo("%s/%s" % (checkpointFolder, "topo.yaml"))
        self.loadRuleSuiteMeta(checkpointFolder)
        builder.build(self.waitToFinish,self.context)
        return builder.jobTopo

    def loadRuleSuiteMeta(self,checkpointFolder):
        ruleMeta = yaml.load(open("%s/rulesuite.meta" % checkpointFolder).read())
        self.context.set("ruleMeta", ruleMeta)


    def execute(self):
        self.jobs["start"].start()

        if self.waitToFinish is not None:
            self.waitToFinish.wait(self.timeout)

#      def fetchRuleFromJar(self, jar):
        #  with zipfile.ZipFile(jar,'r') as z:
            #  for file in z.filelist:
                #  if file.filename.endswith(".yaml"):
                    #  self.topo = z.read(file.filename)
                #  elif file.filename.endswith(".py"):
                    #  self.rule = z.read(file.filename)

if __name__ == '__main__':
    mgr = RuleSuiteProcessor("/home/feng.chen/gitrepo/product/cip/ruleEngD/rulesuites/testcheckpoint",  timeout=9)
    mgr.execute()
    print "return context:"
    print mgr.context.context

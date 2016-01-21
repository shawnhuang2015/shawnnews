import importlib
import sys 
import zipimport
import threading
from Task import Triggers
from Task import Task
from Topology import Topology
from Context import RuleContext

class RuleTaskBuilder(object):
    def loadRuleSuite(self, location, ruleSuiteName):
        sys.path.append(location) 
        self.rulePackage = importlib.import_module(ruleSuiteName)
    def loadTopo(self, topoFile):
        self.topology = Topology()
        self.topology.init(topoFile)
        self.jobTopo = {}
        self.visited = {}

    def build(self, notifyFinish = None, context = None):
        # DFS from end node to build tasks
        #print self.topology.topo
        self.dfs("end", notifyFinish, context)

    def dump(self):
        for jobname, task in self.jobTopo.iteritems():
            print "jobname:%s jobTriggers:%s jobServants:%s hole:%s" % (jobname, task.triggers.triggers, task.servants.servants, task.triggers.holeNum)

    def dfs(self, jobname, notifyFinish = None, context = None):
        if jobname in self.visited.keys():
            return

        self.visited[jobname] = True

        if not jobname in self.jobTopo.keys():
            routine = getattr(self.rulePackage, jobname)
            if jobname == 'end' and notifyFinish is not None:
                self.jobTopo[jobname] = Task(jobname, routine, (notifyFinish,context,))
            else:
                self.jobTopo[jobname] = Task(jobname, routine, (context,))

        for parent in self.topology.topo[jobname]:
            if not parent in self.jobTopo.keys():
                parroutine=getattr(self.rulePackage, parent)
                self.jobTopo[parent] = Task(parent, parroutine, (context,))

            self.jobTopo[jobname].triggers.add(parent)
            self.jobTopo[parent].servants.add(self.jobTopo[jobname])
            self.dfs(parent, notifyFinish, context)

if __name__ == '__main__':
    builder = RuleTaskBuilder()
    builder.loadRuleSuite("/home/feng.chen/experiments/cip/ruleEngD/rulesuites/testcheckpoint","rulesuite")
    builder.loadTopo("topo.yaml")
    context = RuleContext()
    e = threading.Event()
    builder.build(e,context)
    builder.jobTopo["start"].start()
    e.wait(10)
    print context.get("R4")
    print "===========Topology completed ==========="

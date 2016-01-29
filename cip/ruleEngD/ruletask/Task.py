#http://softwareramblings.com/2008/06/running-functions-as-threads-in-python.html
import threading
from Context import RuleContext

class Triggers(object):
    def __init__(self):
        self.triggers = {} 
        self.holeNum = 0 
        self.mutex = threading.Lock()

    def add(self, triggerName):
        self.mutex.acquire(1)
        self.triggers[triggerName] = False
        self.holeNum = self.holeNum +1
        self.mutex.release()

    def activate(self, triggerName):
        self.mutex.acquire(1)
        self.triggers[triggerName] = True
        self.holeNum = self.holeNum -1
        self.mutex.release()

class ServantCollection(object):
    def __init__(self):
        self.servants = {}
        self.mutex = threading.Lock()

    def add(self, task):
        self.mutex.acquire(1)
        self.servants[task.name] = task
        self.mutex.release()

    def getAll(self):
        return self.servants
    # TODO: why not work
#      def __iter__(self):
        #  return self.servants.__iter__
        
class Task(threading.Thread):

    def __init__(self, name, target, args = ()):
        threading.Thread.__init__(self)
        self.name = name
        self.target = target

        self.triggers = Triggers()
        self.servants = ServantCollection()
        
        self.context = None
        self.finishEvent = None
        for arg in args:
            if type(arg) == threading._Event:
                self.finishEvent = arg
            elif type(arg) == RuleContext:
                self.context = arg 

    def triggerServants(self):
        for name, servant in self.servants.getAll().iteritems():
            servant.triggers.activate(self.name)
            if servant.triggers.holeNum == 0:
                servant.start()

    def run(self):
        self.target(self.context)
        self.triggerServants()

        # for the 'end' task, need to emit the finishevent to the RuleTaskManager
        if self.finishEvent is not None:
            self.finishEvent.set()

if __name__ == "__main__":
    pass
#      t1 = Task("t1",__import__("rulesuite.rule1"))
    #  t2 = Task("t2",__import__("rulesuite.rule2"))
    #  t1.triggers.add("t2")
    #  t2.servants.add(t1)
    #  t2.start()

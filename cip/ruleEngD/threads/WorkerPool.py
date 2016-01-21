import Queue
import threading
import time

import RuleJobSubmitter
import RuleResultPoster

class WorkerPool(object):
    def __init__(self, queue1Capacity=100, queue2Capacity = 100):
        # Queue is synchronized class helping for building WorkerPool
        self.queue1Capacity= queue1Capacity
        self.queue2Capacity= queue2Capacity
        self.queue1 = Queue.Queue()
        self.queue2 = Queue.Queue()
        for i in range(self.queue1Capacity):
            self.queue1.put(RuleJobSubmitter.RuleJobSubmitter(self.queue1))

        for i in range(self.queue2Capacity):
            self.queue2.put(RuleResultPoster.RuleResultPoster(self.queue2))

    def acquireRuleSubmitter(self):
        try:
            return  self.queue1.get(block=False)
        except Queue.Empty:
            print "Warning worker pool queue1 exhausted" 
    
    def acquireResultPoster(self):
        try:
            return  self.queue2.get(block=False)
        except Queue.Empty:
            print "Warning worker pool queue2 exhausted" 


import threading
import Queue

class Worker(threading.Thread):
    def __init__(self, queue):
        threading.Thread.__init__(self)
        self.queue = queue
    
    def doJob(self):
        pass
    def run(self):
        self.queue.get()
        doJob(self)
        self.queue.task_done()


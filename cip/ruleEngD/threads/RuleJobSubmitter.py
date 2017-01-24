import Worker

class RuleJobSubmitter(Worker.Worker):
    def __init__(self, queue):
        Worker.Worker.__init__(self, queue)

    def setJob(self, job):
        self.job = job

    def run(self):
        print "submit job %s" %self.job

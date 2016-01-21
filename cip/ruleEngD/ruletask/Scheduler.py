import threading
import TopoReader.TopoReader

class Scheduler(object):
    def setJobs(self, jobs):
        self.jobs = jobs
        self.topo = TopoReader()

    def appendJob(self, job):
        self.jobs.append(job)

    def work(self):
        for job in self.jobs:
            task = threading.Thread(target = job)
            task.start()
            task.join()

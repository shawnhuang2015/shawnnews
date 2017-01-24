import Worker

class RuleResultPoster(Worker.Worker):
    def __init__(self, queue):
        Worker.Worker.__init__(self, queue)

    def setResult(self, resultJson):
        self.result = resultJson
        
    def run(self):
        # do post result to kafka
        print "posting to kafka %s" %self.result

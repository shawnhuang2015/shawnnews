import threading

class RuleContext(object):
    def __init__(self):
        self.context = {}
        self.mutex = threading.Lock()

    def setRuleResult(self, rule, result):
        self.mutex.acquire(1)
        self.context["result"][rule] = result
        self.mutex.release()

    def setWorkingSpace(self, key, value):
        self.mutex.acquire(1)
        self.context["wspace"][rule] = result
        self.mutex.release()

    def set(self, key, value):
        self.mutex.acquire(1)
        self.context[key] = value
        self.mutex.release()
    def get(self, key):
        if key in self.context.keys():
            return self.context[key]

        return None

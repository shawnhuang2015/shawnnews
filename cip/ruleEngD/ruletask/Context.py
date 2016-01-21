import threading

class RuleContext(object):
    def __init__(self):
        self.context = {}
        self.mutex = threading.Lock()

    def set(self, key, value):
        self.mutex.acquire(1)
        self.context[key] = value
        self.mutex.release()
    def get(self, key):
        if key in self.context.keys():
            return self.context[key]

        return None

import threading

def f(num):
    print num

class A(threading.Thread):
    def __init__(self, target, arg):
        threading.Thread.__init__(self)
        self.target = target
        self.arg = arg
    def run(self):
        self.target(self.arg)

a=A(f,5)
a.start()

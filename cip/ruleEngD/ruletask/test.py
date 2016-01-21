import threading
import time
import logging

def daemon(e):
    time.sleep(2)
    print "daemon finished"
    e.set()

e = threading.Event()
d = threading.Thread(name="daemon", target = daemon, args=(e,))
d.setDaemon(True)

d.start()
e.wait(-1)
print "program completd"

import logging
import logging.config
import sys
sys.path.append("..")
from monitor.Perf import Perf
from functools import wraps
import time

logging.config.fileConfig("/home/feng.chen/gitrepo/product/cip/ruleEngD/logging/logging.conf")
logger = logging.getLogger("rotatingFileHandler")
def logTxn(api):
    def true_decorator(f):
        @wraps(f)
        def wrapped(*args,**kwargs):
            start = time.time()
            logger.info("CALL_API {0}".format(api))
            r = f(*args, **kwargs)
            end = time.time()
            logger.info("{0}, duration {1}".format(api,end-start))
            return r
        return wrapped
    return true_decorator

def Performance(perf, tags ):
    def true_decorator(f):
        @wraps(f)
        def wrapped(*args,**kwargs):
            start = time.time()
            r = f(*args, **kwargs)
            end = time.time()
            perf.post(tags, end-start)
            return r
        return wrapped
    return true_decorator

if __name__ == "__main__":
    perf = Perf("192.168.33.70","sla")
    @logTxn("runRule")
    @Performance(perf,{})
    def runRule():
        print "running rule"
    runRule()


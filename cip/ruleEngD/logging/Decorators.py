import logging
import logging.config
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


if __name__ == "__main__":
    @logTxn("runRule")
    def runRule():
        print "running rule"
    runRule()


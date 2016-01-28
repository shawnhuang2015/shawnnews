import threading
import time
##############
# Context contains 3 parts:
# 1. Request : read only, no mutex needed
# 2. Working space: mutex required as it's not guarantee two rule writing the same object in context
# 3. Result: different section for rules, no mutex needed
# ref: http://stackoverflow.com/questions/3387691/python-how-to-perfectly-override-a-dict
class RuleContext(object):
    def __init__(self):
        self.__init__({})

    def __init__(self, _dict):
        if type(_dict) == dict:
            self.ctx = _dict
            self.ws_mutex = threading.Lock()

    def __str__(self):
        str = ""
        return ', '.join(["{}_{}".format(k,v) for k,v in self.ctx.iteritems()])

# Used as getter
def __REQ_ACTOR(ctx):
    return ctx.ctx["__REQ_ACTOR__"]

def __REQ_CP(ctx):
    return ctx.ctx["__REQ_CP__"]

def __REQ_EVT(ctx):
    return ctx.ctx["__REQ_EVT__"]

# Used as getter/setter
def __WS(ctx):
    return ctx.ctx["__WS__"]


def __W_WS(ctx, key, value):
    if type(ctx) == RuleContext:
        ctx.ws_mutex.acquire(1)
        ctx.ctx["__WS__"][key] = value
        ctx.ws_mutex.release()
# Use as setter
def __W_RET(ctx, rule, ret):
    print "writing ret"
    ctx.ctx["__RET__"][rule] = ret

#  import sys
#  sys.path.append("../rulebase")
#  from BizObjBase import BizObjBase
#  from threading import Thread


#  testctx = RuleContext({
        #  "__REQ_ACTOR__" : BizObjBase({}),
        #  "__REQ_CP__" : BizObjBase({}),
        #  "__REQ_EVT__" : BizObjBase({}),
        #  "__WS__" : {},
        #  "__RET__" :{}
        #  })
#  def fun():
    #  __W_WS(testctx,"hello","world")

#  def fun2():
    #  __W_RET(testctx,"rule1", "result")

#  if __name__ == "__main__":


    #  t = Thread(target = fun)
    #  t3 = Thread(target = fun)
    #  t.start()
    #  t3.start()
    #  t2 = Thread(target = fun2)
    #  t2.start()

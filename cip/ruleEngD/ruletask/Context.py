import threading
import time
import inspect
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

# Used as getter
def __RULE_META(ctx):
    return ctx.ctx["__RULE_META__"]

# Used as getter/setter
def __WS(ctx, key):
    if key in ctx.ctx["__WS__"]:
        return ctx.ctx["__WS__"][key]
    return None

# Lock write
def __LW_WS(ctx, key, value):
    if type(ctx) == RuleContext:
        ctx.ws_mutex.acquire(1)
        ctx.ctx["__WS__"][key] = value
        ctx.ws_mutex.release()

def __W_WS(ctx, key, value):
    ctx.ctx["__WS__"][key] = value
    if type(ctx) == RuleContext:
        ctx.ctx["__WS__"][key] = value

# Use as setter/getter
def __W_RULE_RET(ctx, ret):
    # rule name is the name of function who calls __W_RET
    rulename = inspect.stack()[1][3]
    ctx.ctx["__RET__"][rulename] = ret

def __RULE_RET(ctx, rule):
    return ctx.ctx["__RET__"][rule]

# Use to overwrite the result after resolution conflict
def __W_RET(ctx,ret):
    ctx.ctx["__RET__"] = ret

def __RET(ctx):
    return ctx.ctx["__RET__"] 



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

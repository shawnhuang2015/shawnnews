import json
from Context import RuleContext
from BizObjBase import BizObjectBase 
import time
def set_rule_result(context, rule_name, result):
    context.set("result",{rule_name:result})

'''context is mandatory for all rules including the dummy start and end'''
def start(context):
    print "execute start"
    pass

def end(context):
    print "execute end"
    #User can place rule result conflict resolution here
    pass
############Data load segment#################
def load1(context):
    print context.context
    #context.set('L1','L1 done')
    print "loading 1"

def load2(context):
    #context.set('L2','L2 done')
    print "loading 2"

def load3(context):
    #context.set('L3','L3 done')
    print "loading 3"

############Rule segment#################

def rule1(context):
    #context.set('R1','R1 done')
    print "this is rule1"

    request = context["requestContext"]
    reqContext = request["context"]
    if reqContext["name"] == "Will Smith"  and reqContext["gender"]=="Male":
        set_rule_result(context, "rule1", "Approve")
    else
        set_rule_result(context, "rule1", "Deny")

def rule2(context):
    #context.set('R2','R2 done')
    print "this is rule2"
    request = context["requestContext"]
    reqContext = request["context"]
    if reqContext["age"] == "86"  and reqContext["gender"]=="Male":
        set_rule_result(context, "rule2", "Deny")
    else
        set_rule_result(context, "rule2", "Approve")

def rule3(context):
    time.sleep(2)
    obj = '''
        {
            "user":
            {
                "user_id":"111",
                "gender":"M",
                "age":"26"
            }
        }
    '''
#      result = json.loads(obj)
    #  context.set('result',{'R3':result})
#      #context.set('R3','R3 done')
    print "this is rule3"

def rule4(context):
    #context.set('R4','R4 done')
    print "this is rule4"
    print "reading from context : L1:%s" % context.get('L1')


if __name__ == '__main__':
    load1()
    rule1()
    print context.context 

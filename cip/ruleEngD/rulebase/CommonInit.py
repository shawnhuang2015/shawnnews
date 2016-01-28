##############################################
#####  Rule Context layout ###################
#
#  RequestContext(rename to Request)
#       actor
#           flow_type
#           entry_point
#           client_id
#           ip
#           session_id
#           
#       rule_suite
#           uri
#           conflict_resolution
#           check_point
#           summary
#       context(rename to Event)
#  Workspace
#  Result
#       "rule1":"result1"
#       "rule2":"result2"

from BizObjBase import BizObjBase

def initReqBizObjects(context,actor,rulesuite,event):
    if not context == None:
        if "requestContext" in context:
            request = context["requestContext"] 
            if "actor" in request:
                actor = BizObjBase(request["actor"])
            if "rule_suite" in request:
                ruleSuite = BizObjBase(request["rule_suite"])
            if "context" in request:
                event = BizObjBase(request["context"])

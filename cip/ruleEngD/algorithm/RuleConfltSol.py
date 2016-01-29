import sys
sys.path.append("../")
from ruletask.Context import __RET
from ruletask.Context import __RULE_META
from ruletask.Context import __W_RET

def default_sol_conflict(ctx):
    ruleMeta = __RULE_META(ctx)
    resolution = ruleMeta.resolution
    orgResult = __RET(ctx)

    if resolution == "HPW" and len(ruleMeta.pri) == 0:
        resolution = "ALL"

    if resolution == "HPW":
        winner = ""
        for rule in ruleMeta.pri:
            if rule in orgResult.keys():
                winner = orgResult[rule]
                break
        # Retain the only one winner and discard others
        # If winner is empty, discard in which case the policy is back to "ALL"
        if not winner == "":
            __W_RET(ctx,winner)

#      elif resolution == "MW":
        #  # Count and sort
        #  buckets = {}
        #  print orgResult
        #  for rule, result in orgResult.iteritems():
            #  if not result in buckets.keys():
                #  buckets[result] = 0

            #  buckets[result] += 1
        #  print buckets
        #  sortRet = sorted([(value,key) for (key,value) in buckets.items()], reverse=True)
        #  if len(sortRet) > 0:
            #  __W_RET(ctx,sortRet[0][1])
    else:
        # Default is ALL
        pass

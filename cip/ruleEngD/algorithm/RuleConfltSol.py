
def default_sol_conflict(context):
    ruleMeta = context.get("ruleMeta")
    resolution = ruleMeta["resolution"]
    orgResult = context.get("result")

    if resolution == "HPW" and len(ruleMeta["pri"]) == 0:
        resolution = "ALL"

    if resolution == "HPW":
        winner = ""
        for rule in ruleMeta["pri"]:
            if rule in orgResult.keys():
                winner = orgResult[rule]
                break
        # Retain the only one winner and discard others
        # If winner is empty, discard in which case the policy is back to "ALL"
        if not winner == "":
            context.set("result", winner)

    elif resolution == "MW":
        # Count and sort
        buckets = {}
        for rule, result in orgResult.iteritems():
            if not result in buckets.keys():
                buckets[result] = 0

            buckets[result] += 1
        print buckets
        sortRet = sorted([(value,key) for (key,value) in buckets.items()], reverse=True)
        if len(sortRet) > 0:
            context.set("result", sortRet[0][1])
    else:
        # Default is ALL
        pass

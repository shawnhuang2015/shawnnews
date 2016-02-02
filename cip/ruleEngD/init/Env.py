import sys
from Const import SUITE_ROOT

MODULES = ("algorithm","cache","config","cust","init","rulebase","ruletask","threads","web","logging")
SUITES = ("testcheckpoint",)

def initEnv():
    for module in MODULES:
        sys.path.append(module)
    for suite in SUITES:
        sys.path.append("%s/%s" % (SUITE_ROOT, suite))

if __name__ == "__main__":
    print MODULES
    print SUITES



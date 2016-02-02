import sys
sys.path.append("../")
from init.Env import initEnv
initEnv()

import yaml
import importlib
from ruletask.Topology import Topology
from werkzeug.contrib.cache import MemcachedCache
from init.Const import SUITE_ROOT
from init.Const import POSTFIX_META
from init.Const import POSTFIX_TOPO 
from init.Const import POSTFIX_PKG
from init.Const import SUITE_NAME_DEF
from init.Env import SUITES
from rulebase.BizObjBase import BizObjBase
import pickle

class RulEngCache(object):
    def __init__(self, host, port):
        self.cache = MemcachedCache(['%s:%s' % (host, port)])
        for ckpoint in SUITES:
            self.initSuiteAll(ckpoint)

    def initSuiteAll(self,ckpoint):
        self.initPkg(ckpoint)
        self.initTopo(ckpoint)
        self.initMeta(ckpoint)

    def initPkg(self,ckpoint):
        data=open("%s/%s/%s.pyc" %(SUITE_ROOT, ckpoint, SUITE_NAME_DEF) ,'r').read()
        # self.cache.set(ckpoint+ POSTFIX_PKG, importlib.import_module(SUITE_NAME_DEF, package=ckpoint))
        self.cache.set(ckpoint+ POSTFIX_PKG, data)

    def initTopo(self,ckpoint):
        topology = Topology()
        topology.init("%s/%s/topo.yaml" %(SUITE_ROOT, ckpoint))
        self.cache.set(ckpoint+ POSTFIX_TOPO, topology)

    def initMeta(self,ckpoint):
        ruleMeta = yaml.load(open("%s/%s/%s.meta" % (SUITE_ROOT, ckpoint, SUITE_NAME_DEF)).read())
        metaObj = BizObjBase(ruleMeta)
        self.cache.set(ckpoint+ POSTFIX_META, metaObj)


class A(object):
    def load1():
        pass
if __name__ == "__main__":
    cache = RulEngCache("127.0.0.1", 11211)
    print cache.cache.get("testcheckpoint_meta")
    print cache.cache.get("testcheckpoint_topo")
    data = cache.cache.get("testcheckpoint_pkg")
    import pickle
    a = A()
    obj = pickle.dumps(a)
    print dir(obj)
    print type(obj)
    b = pickle.loads(obj)
    print dir(b)

    #a1 = importlib.import_module(SUITE_NAME_DEF, package="testcheckpoint")
    import imp
    a1 = open("/home/feng.chen/gitrepo/product/cip/ruleEngD/rulesuites/testcheckpoint/rulesuite.py", 'r').read()
    d = {"a": a1}
    nw = imp.new_module("test")
    exec nw in d.__dict__
    print nw
    #print getattr(obj, "load1")


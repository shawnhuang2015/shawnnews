import yaml
from toposort import toposort, toposort_flatten

class Topology(object):
    def init(self,topoFile):
        self.topo = yaml.load(open(topoFile).read())
        self.topo["start"]=[]

if __name__ == '__main__':
    topo = Topology()
    topo.init("topo.yaml")
    print topo.topo
    print topo.topo['end']

import os
import ConfigParser

class RuleEngConfig(object):
    def __init__(self):
        Config = ConfigParser.ConfigParser()
        # curDir = os.path.dirname(os.path.realpath(__file__)) 
        curDir = "."
        confPath = "%s/config.ini" % curDir
        if not os.path.isfile(confPath):
            raise Exception ("Can't find %s" % confPath)
        Config.read("%s/config.ini" % curDir)
        self.conf={}
        for sec in Config.sections():
            self.conf[sec] = {}
            for opt in Config.options(sec):
                self.conf[sec][opt] = Config.get(sec, opt)

    def validate(self):
        return True

    def __getitem__(self, index):
        return self.conf[index]

ruleEngConfigure = RuleEngConfig()

if __name__ == "__main__":
    print ruleEngConfigure['kafka']['ip']

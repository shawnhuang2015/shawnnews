import ConfigParser
confRoot='/home/feng.chen/experiments/cip/ruleEngD'

class RuleEngConfig(object):
    def __init__(self):
        Config = ConfigParser.ConfigParser()
        Config.read("%s/config/config.ini" % confRoot)
        self.conf={}
        for sec in Config.sections():
            self.conf[sec] = {}
            for opt in Config.options(sec):
                self.conf[sec][opt] = Config.get(sec, opt)

    def getCfg(self):
        return self.conf

if __name__ == "__main__":
    conf = RuleEngConfig()
    print conf.getCfg()['kafka']['ip']

import logging
import logging.config
from config.RuleEngConfig import ruleEngConfigure

logging.config.fileConfig(ruleEngConfigure["log"]["conf"])
rulengLogger = logging.getLogger(ruleEngConfigure["log"]["type"])

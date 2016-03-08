import sys
sys.path.append("..")
import datetime,random,time
# from influxdb import InfluxDBClient 
from config.RuleEngConfig import ruleEngConfigure
import requests

# Rewrite InfluxDBClient since the InfluxDBClient provided has issue in installer on not finding dateutil.tz module
# @see https://docs.influxdata.com/influxdb/v0.10/guides/writing_data/
class InfluxDBClient(object):
    def __init__(self, host, database):
        self.restUrl = "http://%s:8086/write?db=%s" % (host, database)

    def write_points(self, msg):
        # 'cpu_load_short,host=server01,region=us-west value=0.64 1434055562000000000'
        try:
            print msg
            requests.post(self.restUri,data = msg, timeout = float(ruleEngConfigure["rest"]["post_timeout"]))
        except Exception:
            pass

class Perf(object):
    def __init__(self, host, measurement):
        self.poster = InfluxDBClient(host=host, database=ruleEngConfigure["influxdb"]["db"])
        self.measurement = measurement

    def post(self, tags, value):
        ts_rfc3339 = int(round(time.time() * 1000000000))
        # json_body=[{"measurement": self.measurement,"tags":tags,"time":ts_rfc3339,"fields":{"value":value}}]
        tag_str = ",".join("%s=%s" %( k, v) for k, v in tags.iteritems())
        msg =  "%s,%s value=%s %s" % (self.measurement, tag_str , value, ts_rfc3339)
        self.poster.write_points(msg)

if __name__ == "__main__":
    perf = Perf("192.168.33.70", "rule_result")
    while True:
#          perf.post({"checkpoint": "OnDeviceInit"}, random.random())
        #  perf.post({"checkpoint": "OnUserLogin"}, random.random())
        #  perf.post({"checkpoint": "OnBizFraudDetcted"}, random.random())
        # perf.post({}, random.random())
        perf.post({"rule":"r4_operation_without_fingerprint"}, 5.0)
        time.sleep(1)

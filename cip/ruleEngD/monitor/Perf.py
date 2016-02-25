import datetime
import random
import time
from influxdb import InfluxDBClient 

class Perf(object):
    def __init__(self, host, measurement):
        self.poster = InfluxDBClient(host=host, database='ruleng')
        self.measurement = measurement

    def post(self, tags, value):
        ts_rfc3339 = int(round(time.time() * 1000000000))
        json_body=[{"measurement": self.measurement,"tags":tags,"time":ts_rfc3339,"fields":{"value":value}}]
        self.poster.write_points(json_body)

if __name__ == "__main__":
    perf = Perf("192.168.33.70", "sla")
    while True:
#          perf.post({"checkpoint": "OnDeviceInit"}, random.random())
        #  perf.post({"checkpoint": "OnUserLogin"}, random.random())
        #  perf.post({"checkpoint": "OnBizFraudDetcted"}, random.random())
        perf.post({}, random.random())
        time.sleep(1)




import random
import time
import datetime
from influxdb import InfluxDBClient
client = InfluxDBClient(database='ruleng')
while True:
    checkpoint = "OnDeviceInit"
    # ts_rfc3339 = datetime.datetime.now().isoformat('T')  
    ts_rfc3339 = int(round(time.time() * 1000000000)) 
    sla = random.random() 
    json_body=[{"measurement": "sla","tags":{"checkpoint": checkpoint},"time":ts_rfc3339,"fields":{"value":sla}}]
    client.write_points(json_body)

    checkpoint = "OnUserLogin"
    ts_rfc3339 = int(round(time.time() * 1000000000)) 
    sla = random.random() 
    json_body=[{"measurement": "sla","tags":{"checkpoint": checkpoint},"time":ts_rfc3339,"fields":{"value":sla}}]
    client.write_points(json_body)
    time.sleep(1)

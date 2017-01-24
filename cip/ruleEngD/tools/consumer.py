#!/usr/bin/env python
# ref: http://kafka-python.readthedocs.org/en/latest/usage.html#kafkaconsumer

import sys,getopt
import threading,logging, time

from kafka.client import KafkaClient
from kafka import KafkaConsumer
from kafka.consumer import SimpleConsumer

def echo_opts(ip_port, topic, group, debug):
    print "echo opts:\n"
    print "ip_port:%s\ntopic:%s\ngroup:%s\ndebug:%s" %(ip_port, topic, group, debug)

def usage():
    print "producer -c ip:port -t topic -g group"

opts, args = getopt.getopt(sys.argv[1:], "hdc:t:g:")

ip_port="localhost:9092"
topic=""
group="dummy"
debug=False

for op, value in opts:
    if op == "-h":
        usage()
        sys.exit()
    elif op == "-c":
        ip_port = value
    elif op == "-t":
        topic = value
    elif op == "-g":
        group= value
    elif op == "-d":
        debug = True

echo_opts(ip_port, topic, group, debug)

if (topic ==""):
    print "Empty topic "
    usage()
    sys.exit()

if debug == True:
    logging.basicConfig(
            format='%(asctime)s.%(msecs)s:%(name)s:%(thread)d:%(levelname)s:%(process)d:%(message)s',
            level = logging.DEBUG
            )


#client = KafkaClient(ip_port)
#consumer = SimpleConsumer(client, group, topic)
#for message in consumer:
#    print (message)
print "Output\n"
#consumer = KafkaConsumer('test', group_id = 'test' , bootstrap_servers=['localhost:9092'])
consumer = KafkaConsumer(topic, group_id = group, bootstrap_servers=[ip_port])
for message in consumer:
    print ("%s:%d:%d: key=%s value=%s" % (message.topic, message.partition,
                                                  message.offset, message.key,message.value))

#!/usr/bin/env python

import sys,getopt
import threading,logging, time

from kafka.client import KafkaClient
from kafka.producer import SimpleProducer

def echo_opts(ip_port, topic, message, debug):
    print "echo opts:\n"
    print "ip_port:%s\ntopic:%s\nmessage:%s\ndebug:%s\n" %(ip_port, topic, message, debug)

def usage():
    print "producer -c ip:port -t topic -m message"

opts, args = getopt.getopt(sys.argv[1:], "hdc:t:m:")

ip_port="localhost:9092"
topic=""
message=""
debug=False

for op, value in opts:
    if op == "-h":
        usage()
        sys.exit()
    elif op == "-c":
        ip_port = value
    elif op == "-t":
        topic = value
    elif op == "-m":
        message = value
    elif op == "-d":
        debug = True

echo_opts(ip_port, topic, message, debug)

if (topic =="" or message == ""):
    print "Empty topic or message"
    usage()
    sys.exit()

if debug == True:
    logging.basicConfig(
            format='%(asctime)s.%(msecs)s:%(name)s:%(thread)d:%(levelname)s:%(process)d:%(message)s',
            level = logging.DEBUG
            )


client = KafkaClient(ip_port)
producer = SimpleProducer(client)
producer.send_messages(topic, message)


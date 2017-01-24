#!/usr/bin/env python

import threading,logging, time

import os, sys
sys.path.insert(0, "%s/%s" % (os.getcwd(), "threads"))
sys.path.insert(0, "%s/%s" % (os.getcwd(), "config"))

from kafka.client import KafkaClient
from kafka.producer import SimpleProducer
from kafka import KafkaConsumer

import json
import DummyPregel
import RuleEngConfig
import WorkerPool
from ruletask import RuleTaskManager

class RuleEngDProducer(threading.Thread):
    daemon = True

    def run(self):
        client_prod = KafkaClient("localhost:9092")
        producer = SimpleProducer(client_prod)

        while True:
            print 'producer run4\n'
            producer.send_messages('test', "test")
            producer.send_messages('test',"\xc2Hola, mundo!")
            print 'send completed\n'

            time.sleep(1)

#  class RuleEngReq(object):
    #  def __init__(self, reqJson):
        #  req = json.load(reqJson)
        #  self.__init__(self,req.actor, req.ruleUri, req.checkpoint, req.rule_resolution, req.context)

    #  def __init__(self, actor, ruleUri,checkpoint, rule_resolution,context):
        #  self.actor = actor
        #  self.ruleUri = ruleUri
        #  self.checkpoint = checkpoint
        #  self.rule_resolution = rule_resolution
        #  self.context = context
    #  def __str__(self):
        #  return "%s %s %s %s %s" % (self.actor, self.ruleUri, self.checkpoint, self.rule_resolution, self.context)


class RuleEngDaemon(object):
    def __init__(self):
        self.config = RuleEngConfig.RuleEngConfig().getCfg()
        self.workPool = WorkerPool.WorkerPool()
        ip_port = "%s:%s" % (self.config['kafka']['ip'], self.config['kafka']['port'])
        self.consumer = KafkaConsumer(self.config['kafka']['topic'], 
                  group_id = self.config['kafka']['group'], bootstrap_servers=[ip_port])

    def work(self):
        while True:
            print 'waiting data from kafka'
            for item in self.consumer:
                job = item.value
                jobsubmitter = self.workPool.acquireRuleSubmitter()
                jobsubmitter.setJob(job)
                jobsubmitter.start()
                time.sleep(0.01)


if __name__ == "__main__":
    engine = RuleEngDaemon()
    engine.work()

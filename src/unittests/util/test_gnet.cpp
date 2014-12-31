#include <gtest/gtest.h>
#include <gnet/kafka/kafkamessagequeuefactory.hpp>
#include <gnet/dummymessagequeuefactory.hpp>
#include <gutil/gtimer.hpp>
#include <boost/thread.hpp>
#include <boost/thread.hpp>

/*
Single partition test
(11:36:30.842285): (10542.575 ms) Finished thread 0 write for responseQ0
(11:36:36.874257): (6031.833 ms) Finished thread 0 read for responseQ0 current offset 10485759
Multiple partition test
(11:36:47.019659): (10128.746 ms) Finished thread 0 write for requestQ0
(11:36:50.452009): (3411.900 ms) Finished thread 0 read for requestQ0 current offset 10485759
RequestResponse test
Average request ms is 7
Average request (client to server) ms is 4
Average request (server to client) ms is 2
 */

boost::mutex mutex_;
size_t num_read_ready_;
size_t numofwrites_ = 10 << 20;

void QueueWrite(gnet::MessageQueueFactory* queuecenter, std::string topicname, size_t threadindex,
                bool usingstring){
  gnet::QueueMsgWriter* writer = queuecenter->createQueueMsgWriter(topicname);
  gutil::GTimer timer;
  for(size_t key = 0; key < numofwrites_; ++key){
    size_t value = key * 2;
    if(usingstring){
      std::string keystr = boost::lexical_cast<std::string>(key);
      std::string valuestr = boost::lexical_cast<std::string>(value);
      writer->writeString(keystr, valuestr);
    } else {
      writer->write((const char*)&key, sizeof(key), (const char*)&value, sizeof(value));
    }
  }
  queuecenter->destoryQueueMsgWriter(writer);
  boost::mutex::scoped_lock lock(mutex_);
  timer.Stop("Finished thread " + boost::lexical_cast<std::string>(threadindex) + " write for " + topicname);
}

void QueueRead(gnet::MessageQueueFactory* queuecenter, std::string topicname,
               size_t threadindex, bool usingstring){
  gnet::QueueMsgReader* reader = queuecenter->createQueueMsgReader(topicname, -2); //RD_KAFKA_OFFSET_BEGINNING
  {
    boost::mutex::scoped_lock lock(mutex_);
    ++num_read_ready_;
  }
  gutil::GTimer timer;
  size_t numberofreads = 0;
  std::vector<gnet::Message*> msg;
  while(numberofreads < numofwrites_){
    reader->readMsgs(msg);
    if(msg.size() == 0){
      // std::cout << numberofreads << ", " << numofwrites_ << "\n";
      usleep(100);
      continue;
    }
    numberofreads += msg.size();
    for(size_t i = 0; i < msg.size(); ++i){
      if(usingstring){
        size_t key = boost::lexical_cast<size_t>(msg[i]->getKeyStr());
        size_t value = boost::lexical_cast<size_t>(msg[i]->getValueStr());
        ASSERT_EQ(key * 2, value);
      } else{
        ASSERT_EQ(msg[i]->getKeyLength(), (size_t)8);
        ASSERT_EQ(msg[i]->getValueLength(), (size_t)8);
        ASSERT_EQ(*((size_t*)msg[i]->getKey()) * 2, *((size_t*)msg[i]->getValue()));
      }
      delete msg[i];
    }
    msg.clear();
  }
  ASSERT_EQ(numberofreads, numofwrites_);
  {
    boost::mutex::scoped_lock lock(mutex_);
    timer.Stop("Finished thread " + boost::lexical_cast<std::string>(threadindex) + " read for " + topicname +
               " current offset " + boost::lexical_cast<std::string>(reader->getCurrentOffset()));
  }
  queuecenter->destoryQueueMsgReader(reader);
}

void TestQueue_Batch(gnet::MessageQueueFactory* queuecenter, std::string topicname, size_t num_threads = 1,
               bool usingstring = false) {
  std::vector<boost::thread*> threads;
  for(size_t i = 0; i < num_threads; ++i)
    threads.push_back(new boost::thread(boost::bind(&QueueWrite, queuecenter, topicname + boost::lexical_cast<std::string>(i), i, usingstring)));
  for(size_t i = 0; i < threads.size(); ++i){
    threads[i]->join();
    delete threads[i];
  }
  threads.clear();
  for(size_t i = 0; i < num_threads; ++i)
    threads.push_back(new boost::thread(boost::bind(&QueueRead, queuecenter, topicname + boost::lexical_cast<std::string>(i), i, usingstring)));
  for(size_t i = 0; i < threads.size(); ++i){
    threads[i]->join();
    delete threads[i];
  }
  threads.clear();
}

size_t requestreads_;

void RequestWriteThread(gnet::QueueMsgWriter* requestwriter, size_t numofrequest){
  size_t requestwrites = 0;
  while(requestwrites < numofrequest){
    if(requestwrites > requestreads_ + 20){
      usleep(10);
      continue;
    }
    uint64_t ts = gutil::GTimer::GetTotalMilliSecondsSinceEpoch();
    requestwriter->write((const char*)&ts, sizeof(ts), (const char*)&ts, sizeof(ts));
    ++requestwrites;
  }
}

void RequestReadThread(gnet::QueueMsgReader* requestreader, size_t numofrequest){
  requestreads_ = 0;
  std::vector<gnet::Message*> msg;
  uint64_t sum = 0;
  uint64_t sum_1to2 = 0;
  uint64_t sum_2to1 = 0;
  while(requestreads_ < numofrequest){
    requestreader->readMsgs(msg);
    if(msg.size() == 0){
      usleep(10);
      continue;
    }
    requestreads_ += msg.size();
    for(size_t i = 0; i < msg.size(); ++i){
      uint64_t ts1 =  *((uint64_t*)msg[i]->getKey());
      uint64_t ts2 =  *((uint64_t*)msg[i]->getValue());
      uint64_t ts3 =  gutil::GTimer::GetTotalMilliSecondsSinceEpoch();
      // std::cout << ts1 << ","<< ts2 << ","<< ts3 << "\n";
      sum += ts3 - ts1;
      sum_1to2 += ts2 - ts1;
      sum_2to1 += ts3 - ts2;
      delete msg[i];
    }
    msg.clear();
  }
  std::cout << "Average request ms is " << (sum / numofrequest) << "\n";
  std::cout << "Average request (client to server) ms is " << (sum_1to2 / numofrequest) << "\n";
  std::cout << "Average request (server to client) ms is " << (sum_2to1 / numofrequest) << "\n";
}

void ResponseThread(gnet::QueueMsgReader* reader, gnet::QueueMsgWriter* writer, size_t numofrequest){
  size_t requests = 0;
  std::vector<gnet::Message*> msg;
  while(requests < numofrequest){
    reader->readMsgs(msg);
    if(msg.size() == 0){
      usleep(10);
      continue;
    }
    requests += msg.size();
    for(size_t i = 0; i < msg.size(); ++i){
      uint64_t ts = gutil::GTimer::GetTotalMilliSecondsSinceEpoch();
      writer->write(msg[i]->getKey(), msg[i]->getKeyLength(), (const char*)&ts, sizeof(ts));
      delete msg[i];
    }
    msg.clear();
  }
}

void TestQueue_RequestResponse(gnet::MessageQueueFactory* queuecenter, std::string requestQ, std::string responseQ,
                               size_t numofrequest) {
  gnet::QueueMsgWriter* requestQwriter = queuecenter->createQueueMsgWriter(requestQ);
  gnet::QueueMsgReader* requestQreader = queuecenter->createQueueMsgReader(requestQ, -2);
  gnet::QueueMsgWriter* responseQwriter = queuecenter->createQueueMsgWriter(responseQ);
  gnet::QueueMsgReader* responseQreader = queuecenter->createQueueMsgReader(responseQ, -2);
  std::vector<boost::thread*> threads;
  threads.push_back(new boost::thread(boost::bind(&ResponseThread, requestQreader, responseQwriter, numofrequest)));
  threads.push_back(new boost::thread(boost::bind(&RequestReadThread, responseQreader, numofrequest)));
  threads.push_back(new boost::thread(boost::bind(&RequestWriteThread, requestQwriter, numofrequest)));
  for(size_t i = 0; i < threads.size(); ++i){
    threads[i]->join();
    delete threads[i];
  }
  queuecenter->destoryQueueMsgReader(requestQreader);
  queuecenter->destoryQueueMsgReader(responseQreader);
  queuecenter->destoryQueueMsgWriter(requestQwriter);
  queuecenter->destoryQueueMsgWriter(responseQwriter);
}

TEST(GNETTEST, KAFKA_SINGLEPART) {
  // local test single partition topic write / read.
  gnet::KAFKAMessageQueueFactory messagequeuefactory("gpe1.conf", NULL);
  TestQueue_Batch(&messagequeuefactory, "responseQ");
}

TEST(GNETTEST, KAFKA_MULTIPLEPART) {
  // local test multiple partition topic write / read. It require daemon for rebalance multiple partition.
  gnet::ZookeeperDaemon daemon("127.0.0.1:7100");
  daemon.connect("127.0.0.1:13010", 30000);
  {
    gnet::KAFKAMessageQueueFactory messagequeuefactory("gpe1.conf", &daemon);
    messagequeuefactory.RegisterWorker();
    TestQueue_Batch(&messagequeuefactory, "requestQ");
  }
  daemon.stopDaemon();
}

TEST(GNETTEST, KAFKA_REQUESTRESPONSE) {
  // local test multiple partition topic write / read. It require daemon for rebalance multiple partition.
  gnet::ZookeeperDaemon daemon("127.0.0.1:7100");
  daemon.connect("127.0.0.1:13010", 30000);
  {
    gnet::KAFKAMessageQueueFactory messagequeuefactory("gpe1.conf", &daemon);
    messagequeuefactory.RegisterWorker();
    TestQueue_RequestResponse(&messagequeuefactory, "requestQ", "responseQ", 1 << 10);
  }
  daemon.stopDaemon();
}

TEST(GNETTEST, DummyQueue) {
  gnet::DummyMessageQueueFactory messagequeuefactory;
  QueueWrite(&messagequeuefactory, "test", 0, true);
  QueueRead(&messagequeuefactory, "test", 0, true);
}

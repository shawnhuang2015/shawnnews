#include <gtest/gtest.h>
#include <gnet/kafka/kafkamessagequeuefactory.hpp>
#include <gnet/dummymessagequeuefactory.hpp>
#include <gutil/gtimer.hpp>
#include <boost/thread.hpp>
#include <boost/thread.hpp>

/*
Single partition test
(17:36:08.729829): (10306.429 ms) Finished thread 0 write for responseQ0
(17:36:21.690739): (12960.733 ms) Finished thread 0 read for responseQ0 current offset 10485759
Multiple partition test
(17:36:34.652828): (12927.607 ms) Finished thread 0 write for requestQ
(17:36:34.655823): (12930.734 ms) Finished thread 0 read for requestQ current offset 0
 */

boost::mutex mutex_;
size_t num_read_ready_;
size_t numofwrites_ = 10 << 10;

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

void TestSinglePartitionQueue(gnet::MessageQueueFactory* queuecenter, std::string topicname, size_t num_threads = 1,
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

void TestMultiplePartitionQueue(gnet::MessageQueueFactory* queuecenter, std::string topicname,
               bool usingstring = false) {
  std::vector<boost::thread*> threads;
  num_read_ready_ = 0;
  threads.push_back(new boost::thread(boost::bind(&QueueRead, queuecenter, topicname, 0, usingstring)));
  while(num_read_ready_ < 1)
     usleep(100);
  threads.push_back(new boost::thread(boost::bind(&QueueWrite, queuecenter, topicname, 0, usingstring)));
  for(size_t i = 0; i < threads.size(); ++i){
    threads[i]->join();
    delete threads[i];
  }
}

TEST(GNETTEST, KAFKA_SINGLEPART) {
  // local test single partition topic write / read.
  gnet::KAFKAMessageQueueFactory messagequeuefactory("gpe1.conf", NULL);
  TestSinglePartitionQueue(&messagequeuefactory, "responseQ");
}

TEST(GNETTEST, KAFKA_MULTIPLEPART) {
  // local test multiple partition topic write / read. It require daemon for rebalance multiple partition.
  gnet::ZookeeperDaemon daemon("127.0.0.1:7100");
  daemon.connect("127.0.0.1:13010", 30000);
  {
    gnet::KAFKAMessageQueueFactory messagequeuefactory("gpe1.conf", &daemon);
    messagequeuefactory.RegisterWorker();
    TestMultiplePartitionQueue(&messagequeuefactory, "requestQ");
  }
  daemon.stopDaemon();
}

TEST(GNETTEST, DummyQueue) {
  gnet::DummyMessageQueueFactory messagequeuefactory;
  QueueWrite(&messagequeuefactory, "test", 0, true);
  QueueRead(&messagequeuefactory, "test", 0, true);
}

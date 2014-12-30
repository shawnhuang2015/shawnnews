#include <gtest/gtest.h>
#include <gnet/kafka/kafkamessagequeuefactory.hpp>
#include <gnet/dummymessagequeuefactory.hpp>
#include <gutil/gtimer.hpp>
#include <boost/thread.hpp>
#include <boost/thread.hpp>

/*
Single partition test
(11:17:43.380413): (1984.024 ms) Finished thread 0 write for responseQ
(11:17:43.397739): (2001.299 ms) Finished thread 2 write for responseQ
(11:17:43.422556): (2026.148 ms) Finished thread 1 write for responseQ
(11:17:43.424763): (2028.474 ms) Finished thread 0 read for responseQ current offset 1048575
(11:17:43.430077): (2033.806 ms) Finished thread 2 read for responseQ current offset 1048575
(11:17:43.431130): (2034.871 ms) Finished thread 1 read for responseQ current offset 1048575
Multiple partition test
(11:17:44.599239): (1119.605 ms) Finished thread 1 write for requestQ
(11:17:44.601835): (1122.137 ms) Finished thread 2 write for requestQ
(11:17:44.621353): (1141.863 ms) Finished thread 0 read for requestQ current offset 0
(11:17:44.638224): (1158.603 ms) Finished thread 0 write for requestQ
 */

boost::mutex mutex_;
size_t num_read_ready_;
size_t numofwrites_ = 1 << 20;

void QueueWrite(gnet::MessageQueueFactory* queuecenter, std::string topicname, size_t threadindex,
                size_t writethreads, bool usingstring){
  gnet::QueueMsgWriter* writer = queuecenter->createQueueMsgWriter(topicname);
  gutil::GTimer timer;
  for(size_t key = threadindex; key < numofwrites_; key += writethreads){
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

void TestQueue(gnet::MessageQueueFactory* queuecenter, std::string topicname, size_t readthreads = 3, size_t writethreads = 3,
               bool usingstring = false) {
  std::vector<boost::thread*> threads;
  num_read_ready_ = 0;
  for(size_t i = 0; i < readthreads; ++i)
    threads.push_back(new boost::thread(boost::bind(&QueueRead, queuecenter, topicname, i, usingstring)));
  while(num_read_ready_ < readthreads)
    usleep(100);
  for(size_t i = 0; i < writethreads; ++i)
    threads.push_back(new boost::thread(boost::bind(&QueueWrite, queuecenter, topicname, i, writethreads, usingstring)));
  for(size_t i = 0; i < threads.size(); ++i){
    threads[i]->join();
    delete threads[i];
  }
}

TEST(GNETTEST, KAFKA_SINGLEPART) {
  // local test single partition topic write / read.
  gnet::KAFKAMessageQueueFactory messagequeuefactory("gpe1.conf", NULL);
  TestQueue(&messagequeuefactory, "responseQ");
}

TEST(GNETTEST, KAFKA_MULTIPLEPART) {
  // local test multiple partition topic write / read. It require daemon for rebalance multiple partition.
  gnet::ZookeeperDaemon daemon("127.0.0.1:7100");
  daemon.connect("127.0.0.1:13010", 30000);
  {
    gnet::KAFKAMessageQueueFactory messagequeuefactory("gpe1.conf", &daemon);
    messagequeuefactory.RegisterWorker();
    TestQueue(&messagequeuefactory, "requestQ", 1); // can only have one read thread. otherwise zookeeper will have conflict
  }
}

TEST(GNETTEST, DummyQueue) {
  gnet::DummyMessageQueueFactory messagequeuefactory;
  QueueWrite(&messagequeuefactory, "test", 0, 1, true);
  QueueRead(&messagequeuefactory, "test", 0, true);
}

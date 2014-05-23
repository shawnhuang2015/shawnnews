/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 * kafkareader.hpp: /gdbms/src/customers/gpe_base/base/kafkaconnector.hpp
 *
 *  Created on: Nov 6, 2013
 *      Author: lichen
 ******************************************************************************/

#ifndef GPE_KAFKACONNECTOR_HPP_
#define GPE_KAFKACONNECTOR_HPP_

#include <gutil/gtimer.hpp>
#include <gutil/dummyqueue.hpp>
#include <DQGroupConsumer.h>
#include <gse2/msg/kafka/gse_kafka_writer.hpp>
#include <boost/tuple/tuple.hpp>
#include "gpe_daemon.hpp"
#include "gpeconfig.hpp"


namespace gperun {

/**
 * Listen request queue to get query requests.
 * POC modification: Not Likely.
 */
class KafkaConnector {
 public:
#ifndef ComponentTest
  typedef gdist::DQGroupConsumer QueueReader_t;
  typedef gse2::KafkaMsgWriter QueueWriter_t;
#else
  typedef gutil::DummyReadQueue QueueReader_t;
  typedef gutil::DummyWriteQueue QueueWriter_t;
#endif

  KafkaConnector(GPEDaemon* daemon, std::string connection,
                 std::string name, std::string getrequest_topic,
                 std::string prefetchrequest_topic, std::string writetopic,
                 int prefetchsize)
      : connection_(connection),
        writer_(connection, writetopic, name),
        daemon_(daemon),
        name_(name),
        getrequest_topic_(getrequest_topic),
        prefetchrequest_topic_(prefetchrequest_topic),
        max_num_concurrent_request_(prefetchsize) {
    InitReader();
    dummywritequeue_ = NULL;
#ifdef RecordQueue
    dummywritequeue_ = new gutil::DummyWriteQueue("", getrequest_topic, "");
#endif
  }

  ~KafkaConnector() {
    DeleteReader();
    if (dummywritequeue_ != NULL)
      delete dummywritequeue_;
  }

  void DeleteReader() {
    if (getrequest_reader_ != NULL) {
      delete getrequest_reader_;
      getrequest_reader_ = NULL;
    }
    if (prefetchrequest_reader_ != NULL) {
      delete prefetchrequest_reader_;
      prefetchrequest_reader_ = NULL;
    }
  }

  bool ReaderIsNull() {
    return getrequest_reader_ == NULL;
  }

  void StopReader() {
    getrequest_reader_->stop();
    if (prefetchrequest_reader_ != NULL)
      prefetchrequest_reader_->stop();
  }

  void SetResponse(std::string& requestid, std::string& response) {
    writer_.put(requestid, response);
  }

  void InitReader() {
    getrequest_reader_ = new QueueReader_t(
        connection_, daemon_, getrequest_topic_, name_, -1,
        GPEConfig::queue_sleep_time_, max_num_concurrent_request_ * 2);
    prefetchrequest_reader_ = NULL;
    if (prefetchrequest_topic_.size() > 0
        && GPEConfig::prefetch_request_limit_ > 0) {
      prefetchrequest_reader_ = new QueueReader_t(
          connection_, daemon_, prefetchrequest_topic_, name_, -1,
          GPEConfig::queue_sleep_time_,
          GPEConfig::prefetch_request_limit_ * 2);
    }
  }

  void ReadFromKafka(
      std::vector<boost::tuple<std::string, std::string, std::string> >& ready_queue,
      int maxmsgsize = std::numeric_limits<int>::max()) {
    GASSERT(ready_queue.size() == 0, ready_queue.size());
    if (!getrequest_reader_->isEmpty()) {
      gutil::GTimer timer;
      std::vector<gdist::DQMessage*> msgs = getrequest_reader_->readN(maxmsgsize);
      for (size_t i = 0; i < msgs.size(); ++i) {
        // if (GPEConfig::CheckTimeOut(msgs[i]->key))
        if (dummywritequeue_ != NULL)
          dummywritequeue_->putOneMessage(msgs[i]->key, msgs[i]->value);
        ready_queue.push_back(
            boost::tuple<std::string, std::string, std::string>(
                msgs[i]->key, msgs[i]->value, msgs[i]->timestamp_));
        delete msgs[i];
      }
      GVLOG(Verbose_UDFLow) << "read " << ready_queue.size()
                            << " get requests in "
                            << timer.GetTotalMilliSeconds() << " ms";
    } else if (prefetchrequest_reader_ != NULL
        && !prefetchrequest_reader_->isEmpty()) {
      gutil::GTimer timer;
      std::vector<gdist::DQMessage*> msgs = prefetchrequest_reader_->readN(
          GPEConfig::prefetch_request_limit_);
      for (size_t i = 0; i < msgs.size(); ++i) {
        // if (GPEConfig::CheckTimeOut(msgs[i]->key))
        ready_queue.push_back(
            boost::tuple<std::string, std::string, std::string>(
                msgs[i]->key, msgs[i]->value, msgs[i]->timestamp_));
        delete msgs[i];
      }
      GVLOG(Verbose_UDFLow) << "read " << ready_queue.size()
                            << " prefetch requests in "
                            << timer.GetTotalMilliSeconds() << " ms";
    }
  }

 private:
  QueueReader_t* getrequest_reader_;
  QueueReader_t* prefetchrequest_reader_;
  std::string connection_;
  QueueWriter_t writer_;
  GPEDaemon* daemon_;
  std::string name_;
  std::string getrequest_topic_;
  std::string prefetchrequest_topic_;
  int max_num_concurrent_request_;
  gutil::DummyWriteQueue* dummywritequeue_;
};

}   // namespace gperun

#endif /* GPE_KAFKACONNECTOR_HPP_ */

/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 * resource_test.cpp: /gperun/test/resource_test.cpp
 *
 *  Created on: Feb 13, 2013
 *      Author: lichen
 ******************************************************************************/

#include <gbucket/buckettable.hpp>
#include <gbucket/bucket.hpp>
#include <gmmnt/globalinstances.hpp>
#include <gutil/spinlock.hpp>
#include <gutil/gthreadpool.hpp>
#include <bucketcontainers/messagedatacontainer.hpp>
#include <bucketcontainers/singlevaluecontainer.hpp>
#include <bucketcontainers/multiplevaluecontainer.hpp>

#include <gtest/gtest.h>
#include <queue>
#include <vector>

VertexLocalId_t num_items_inbuckets_ = 9999;
size_t write_bucket_threads_ = 3;
VertexLocalId_t bucketsize_bits_ = 10;
VertexLocalId_t bucketsize_ = (VertexLocalId_t) 1 << bucketsize_bits_;
size_t bigwritesize_ = 1 << 24;

void ThreadSave_SingleValueBucket(
    std::pair<size_t, gbucket::BucketTable*> info) {
  size_t index = info.first;
  for (VertexLocalId_t index = info.first; index < num_items_inbuckets_;
       index += write_bucket_threads_) {
    gbucket::Bucket* bucket = info.second->GetBucket(
                                index >> bucketsize_bits_);
    gutil::GLock lock(bucket->lockmutex());
    bucket
        ->AddCell<VertexLocalId_t, unsigned int,
        gpelib::SingleValueContainer>(index, index * 2);
  }
}

TEST(BUCKETTEST, SINGLEVALUE) {
  char path[1000];
  getcwd(path, 1000);
  std::string enginecfgfile = path;
  enginecfgfile += "/enginecfg.yaml";

  gmmnt::GlobalInstances globalinstance(enginecfgfile);
  {
    gbucket::BucketTable buckettable("singvalue_test",
                                     globalinstance.memserver_,
                                     globalinstance.membuffer_,
                                     globalinstance.diskio_);
    unsigned int* intarray = (unsigned int*) globalinstance.memallocator_
                             ->RegisterAndNewptr(num_items_inbuckets_ * sizeof(unsigned int),
                                                 "singledataarray", "Array");
    std::fill(intarray, intarray + num_items_inbuckets_, 0);
    buckettable.AddMemoryDataId("singledataarray");
    VertexLocalId_t bucketindex = 0;
    while (true) {
      buckettable.AddBucket(
            new gpelib::SingleValueContainer(
              bucketindex * bucketsize_,
              std::min(bucketsize_,
                       num_items_inbuckets_ - bucketsize_ * bucketindex),
              intarray));
      bucketindex++;
      if (num_items_inbuckets_ <= bucketindex * bucketsize_)
        break;
    }
    {
      gutil::GThreadPool pool(write_bucket_threads_ - 1);
      for (size_t i = 0; i < write_bucket_threads_; i++) {
        std::pair<size_t, gbucket::BucketTable*> pair(i, &buckettable);
        pool.run_task(boost::bind(ThreadSave_SingleValueBucket, pair));
      }
      pool.wait();
    }
    for (size_t i = 0; i < num_items_inbuckets_; i++)
      EXPECT_EQ(intarray[i], i * 2);
  }
}

void ThreadSave_SingleMessageValueBucket(
    std::pair<size_t, gbucket::BucketTable*> info) {
  for (VertexLocalId_t index = 0; index < num_items_inbuckets_; ++index) {
    gbucket::Bucket* bucket = info.second->GetBucket(
                                index >> bucketsize_bits_);
    gutil::GLock lock(bucket->lockmutex());
    bucket
        ->AddCell<VertexLocalId_t, unsigned int,
        gpelib::SingleMessageDataContainer>(index, index + info.first);
  }
}

TEST(BUCKETTEST, SINGLEMESSAGE) {
  char path[1000];
  getcwd(path, 1000);
  std::string enginecfgfile = path;
  enginecfgfile += "/enginecfg.yaml";

  gmmnt::GlobalInstances globalinstance(enginecfgfile);
  {
    gbucket::BucketTable buckettable("singmsgvalue_test",
                                     globalinstance.memserver_,
                                     globalinstance.membuffer_,
                                     globalinstance.diskio_);
    unsigned int* intarray = (unsigned int*) globalinstance.memallocator_
                             ->RegisterAndNewptr(num_items_inbuckets_ * sizeof(unsigned int),
                                                 "singledataarray", "Array");
    std::fill(intarray, intarray + num_items_inbuckets_, 0);
    buckettable.AddMemoryDataId("singledataarray");
    VertexLocalId_t bucketindex = 0;
    while (true) {
      buckettable.AddBucket(
            new gpelib::SingleMessageDataContainer(
              bucketindex * bucketsize_,
              std::min(bucketsize_,
                       num_items_inbuckets_ - bucketsize_ * bucketindex),
              intarray));
      bucketindex++;
      if (num_items_inbuckets_ <= bucketindex * bucketsize_)
        break;
    }
    {
      gutil::GThreadPool pool(write_bucket_threads_ - 1);
      for (size_t i = 0; i < write_bucket_threads_; i++) {
        std::pair<size_t, gbucket::BucketTable*> pair(i, &buckettable);
        pool.run_task(
              boost::bind(ThreadSave_SingleMessageValueBucket, pair));
      }
      pool.wait();
    }
    for (size_t i = 0; i < num_items_inbuckets_; i++)
      EXPECT_EQ(
            intarray[i],
            i * write_bucket_threads_
            + write_bucket_threads_ * (write_bucket_threads_ - 1) / 2);
  }
}

void ThreadSave_MultipleValueBucket1(
    std::pair<size_t, gbucket::BucketTable*> info) {
  for (VertexLocalId_t index = 0; index < num_items_inbuckets_; ++index) {
    gbucket::Bucket* bucket = info.second->GetBucket(
                                index >> bucketsize_bits_);
    gutil::GLock lock(bucket->lockmutex());
    bucket
        ->AddCell<VertexLocalId_t, unsigned int,
        gpelib::MultipleValueContainer>(index, index + info.first);
  }
  if (info.first == 0) {
    gbucket::Bucket* bucket = info.second->GetBucket(0);
    for (size_t i = 0; i < bigwritesize_; i++) {
      gutil::GLock lock(bucket->lockmutex());
      bucket
          ->AddCell<VertexLocalId_t, unsigned int,
          gpelib::MultipleValueContainer>(0, 0);
    }
  }
}

void InitMultipleValueBucket(gmmnt::GlobalInstances& globalinstance,
                             gbucket::BucketTable& buckettable) {
  std::string offset_dataid = buckettable.name() + "offsetarray_unittest";
  BucketDataOffset_t* offsetarray =
      reinterpret_cast<BucketDataOffset_t*>(globalinstance.memallocator_
                                            ->RegisterAndNewptr(
                                              num_items_inbuckets_ * sizeof(BucketDataOffset_t), offset_dataid,
                                              "Array"));
  std::fill(offsetarray, offsetarray + num_items_inbuckets_, 0);
  buckettable.AddMemoryDataId(offset_dataid);
  VertexLocalId_t bucketindex = 0;
  while (true) {
    buckettable.AddBucket(
          new gpelib::MultipleValueContainer(
            bucketindex * bucketsize_,
            std::min(bucketsize_,
                     num_items_inbuckets_ - bucketsize_ * bucketindex),
            sizeof(unsigned int), offsetarray))->SetBucketLoadPattern(
          gbucket::BucketLoad_MultipleTimes);
    bucketindex++;
    if (num_items_inbuckets_ <= bucketindex * bucketsize_)
      break;
  }
}

void WriteMultipleValueBucket1(gmmnt::GlobalInstances& globalinstance,
                               gbucket::BucketTable& buckettable) {
  InitMultipleValueBucket(globalinstance, buckettable);
  {
    gutil::GThreadPool pool(write_bucket_threads_ - 1);
    for (size_t i = 0; i < write_bucket_threads_; i++) {
      std::pair<size_t, gbucket::BucketTable*> pair(i, &buckettable);
      pool.run_task(boost::bind(ThreadSave_MultipleValueBucket1, pair));
    }
    pool.wait();
  }
  for (size_t i = 0; i < buckettable.size(); ++i) {
    gbucket::Bucket* bucket = buckettable.GetBucket(i);
    std::string callerid = buckettable.name() + bucket->dataid();
    std::vector<std::string> requests;
    requests.push_back(bucket->dataid());
    EXPECT_EQ(
          globalinstance.memserver_->SendRequests(callerid, requests,
                                                  gmmnt::MemHigh, true),
          true);
    EXPECT_EQ(
          globalinstance.memserver_->IsAllRequestsReady(callerid, true),
          true);
    gpelib::MultipleValueContainer* container =
        (gpelib::MultipleValueContainer*) bucket->GetBucketContainer();
    BucketDataOffset_t* localoffsetarray =
        container->GetLocalOffsetArray();
    unsigned int* dataptr = (unsigned int*) bucket->memorydataptr();
    VertexLocalId_t vid = container->offset();
    for (VertexLocalId_t j = 0; j < container->size(); j++, vid++) {
      unsigned int* startptr =
          j == 0 ? dataptr : dataptr + localoffsetarray[j - 1];
      const unsigned int* endptr = dataptr + localoffsetarray[j];
      size_t size_1 = (size_t) (endptr - startptr);
      if (vid == 0)
        EXPECT_EQ(size_1, write_bucket_threads_ + bigwritesize_);
      else
        EXPECT_EQ(size_1, write_bucket_threads_);
      for (unsigned int* ptr = startptr; ptr < endptr; ++ptr)
        EXPECT_LT(*ptr - vid, write_bucket_threads_);
    }
    globalinstance.memserver_->FreeCaller(callerid);
    bucket->set_memorycacheweight(gmmnt::CacheWeight_NoCache);
  }
}

void WriteMultipleValueBucket2(gmmnt::GlobalInstances& globalinstance,
                               gbucket::BucketTable& buckettable) {
  InitMultipleValueBucket(globalinstance, buckettable);
  for (VertexLocalId_t index = 0; index < num_items_inbuckets_; ++index) {
    gbucket::Bucket* bucket = buckettable.GetBucket(
                                index >> bucketsize_bits_);
    gutil::GLock lock(bucket->lockmutex());
    bucket
        ->AddCell<VertexLocalId_t, unsigned int,
        gpelib::MultipleValueContainer>(index, index);
  }
  gbucket::Bucket* bucket = buckettable.GetBucket(0);
  for (size_t i = 0; i < bigwritesize_; i++) {
    gutil::GLock lock(bucket->lockmutex());
    bucket
        ->AddCell<VertexLocalId_t, unsigned int,
        gpelib::MultipleValueContainer>(0, 0);
  }
  /*for (size_t i = 0; i < buckettable.size(); ++i) {
   gbucket::Bucket* bucket = buckettable.GetBucket(i);
   std::string callerid = buckettable.name() + bucket->dataid();
   std::vector<std::string> requests;
   requests.push_back(bucket->dataid());
   EXPECT_EQ(
   globalinstance.memserver_->SendRequests(callerid, requests,
   gmmnt::MemHigh, true), true);
   EXPECT_EQ(globalinstance.memserver_->IsAllRequestsReady(callerid, true),
   true);
   gpelib::MultipleValueContainer* container =
   (gpelib::MultipleValueContainer*) bucket->GetBucketContainer();
   BucketDataOffset_t* localoffsetarray = container->GetLocalOffsetArray();
   unsigned int* dataptr = (unsigned int*) bucket->memorydataptr();
   VertexLocalId_t vid = container->offset();
   for (VertexLocalId_t j = 0; j < container->size(); j++, vid++) {
   unsigned int* startptr =
   j == 0 ? dataptr : dataptr + localoffsetarray[j - 1];
   const unsigned int* endptr = dataptr + localoffsetarray[j];
   EXPECT_EQ((size_t )(endptr - startptr), (size_t )1);
   for (unsigned int* ptr = startptr; ptr < endptr; ++ptr)
   EXPECT_EQ(*ptr, vid);
   }
   globalinstance.memserver_->FreeCaller(callerid);
   bucket->set_memorycacheweight(0);
   }*/
}

void WriteMultipleValueBucket3(gmmnt::GlobalInstances& globalinstance,
                               gbucket::BucketTable& buckettable) {
  InitMultipleValueBucket(globalinstance, buckettable);
  for (VertexLocalId_t index = 0; index < num_items_inbuckets_; ++index) {
    if (index % 2 == 0)
      continue;
    gbucket::Bucket* bucket = buckettable.GetBucket(
                                index >> bucketsize_bits_);
    gutil::GLock lock(bucket->lockmutex());
    bucket
        ->AddCell<VertexLocalId_t, unsigned int,
        gpelib::MultipleValueContainer>(index, index);
  }
}

/// TODO: 1. add bucket load check for single load.
/// 2. MultipleValueContainer::Append should add case when
/// this->countmode_localcountarray_ is false,
/// CopyUntouchedDataTo also need fix
TEST(BUCKETTEST, MULTIPLEVALUE) {
  char path[1000];
  getcwd(path, 1000);
  std::string enginecfgfile = path;
  enginecfgfile += "/enginecfg.yaml";
  gmmnt::GlobalInstances globalinstance(enginecfgfile);
  {
    gbucket::BucketTable buckettable1("multiplevalue_test1",
                                      globalinstance.memserver_,
                                      globalinstance.membuffer_,
                                      globalinstance.diskio_);
    WriteMultipleValueBucket1(globalinstance, buckettable1);
    gbucket::BucketTable buckettable2("multiplevalue_test2",
                                      globalinstance.memserver_,
                                      globalinstance.membuffer_,
                                      globalinstance.diskio_);
    WriteMultipleValueBucket2(globalinstance, buckettable2);
    // test append
    buckettable1.Switch(&buckettable2, true);
    for (size_t i = 0; i < buckettable1.size(); ++i) {
      gbucket::Bucket* bucket = buckettable1.GetBucket(i);
      std::string callerid = buckettable1.name() + bucket->dataid();
      std::vector<std::string> requests;
      requests.push_back(bucket->dataid());
      EXPECT_EQ(
            globalinstance.memserver_->SendRequests(callerid, requests,
                                                    gmmnt::MemHigh, true),
            true);
      EXPECT_EQ(
            globalinstance.memserver_->IsAllRequestsReady(callerid, true),
            true);
      gpelib::MultipleValueContainer* container =
          (gpelib::MultipleValueContainer*) bucket->GetBucketContainer();
      BucketDataOffset_t* localoffsetarray =
          container->GetLocalOffsetArray();
      unsigned int* dataptr = (unsigned int*) bucket->memorydataptr();
      VertexLocalId_t vid = container->offset();
      for (VertexLocalId_t j = 0; j < container->size(); j++, vid++) {
        unsigned int* startptr =
            j == 0 ? dataptr : dataptr + localoffsetarray[j - 1];
        const unsigned int* endptr = dataptr + localoffsetarray[j];
        size_t size_2 = (size_t) (endptr - startptr);
        if (vid == 0)
          EXPECT_EQ(size_2, write_bucket_threads_ + bigwritesize_ * 2 + 1);
        else
          EXPECT_EQ(size_2, write_bucket_threads_ + 1);
        for (unsigned int* ptr = startptr; ptr < endptr; ++ptr)
          EXPECT_LT(*ptr - vid, write_bucket_threads_);
      }
      globalinstance.memserver_->FreeCaller(callerid);
      bucket->set_memorycacheweight(gmmnt::CacheWeight_NoCache);
    }
  }
  {
    gbucket::BucketTable buckettable1("multiplevalue_test3",
                                      globalinstance.memserver_,
                                      globalinstance.membuffer_,
                                      globalinstance.diskio_);
    WriteMultipleValueBucket1(globalinstance, buckettable1);
    gbucket::BucketTable buckettable2("multiplevalue_test4",
                                      globalinstance.memserver_,
                                      globalinstance.membuffer_,
                                      globalinstance.diskio_);
    WriteMultipleValueBucket3(globalinstance, buckettable2);
    // test replacement
    for (size_t i = 0; i < buckettable1.size(); i++) {
      buckettable1.GetBucket(i)->CopyUntouchedDataTo(
            buckettable2.GetBucket(i));
    }
    buckettable1.Switch(&buckettable2, false);
    for (size_t i = 0; i < buckettable1.size(); ++i) {
      gbucket::Bucket* bucket = buckettable1.GetBucket(i);
      std::string callerid = buckettable1.name() + bucket->dataid();
      std::vector<std::string> requests;
      requests.push_back(bucket->dataid());
      EXPECT_EQ(
            globalinstance.memserver_->SendRequests(callerid, requests,
                                                    gmmnt::MemHigh, true),
            true);
      EXPECT_EQ(
            globalinstance.memserver_->IsAllRequestsReady(callerid, true),
            true);
      gpelib::MultipleValueContainer* container =
          (gpelib::MultipleValueContainer*) bucket->GetBucketContainer();
      BucketDataOffset_t* localoffsetarray =
          container->GetLocalOffsetArray();
      unsigned int* dataptr = (unsigned int*) bucket->memorydataptr();
      VertexLocalId_t vid = container->offset();
      for (VertexLocalId_t j = 0; j < container->size(); j++, vid++) {
        unsigned int* startptr =
            j == 0 ? dataptr : dataptr + localoffsetarray[j - 1];
        const unsigned int* endptr = dataptr + localoffsetarray[j];
        size_t size_3 = (size_t) (endptr - startptr);
        if (vid == 0)
          EXPECT_EQ(size_3, write_bucket_threads_ + bigwritesize_);
        else
          EXPECT_EQ(size_3, j % 2 == 0 ? write_bucket_threads_ : 1);
        for (unsigned int* ptr = startptr; ptr < endptr; ++ptr)
          EXPECT_LT(*ptr - vid, write_bucket_threads_);
      }
      globalinstance.memserver_->FreeCaller(callerid);
      bucket->set_memorycacheweight(gmmnt::CacheWeight_NoCache);
    }
  }
}

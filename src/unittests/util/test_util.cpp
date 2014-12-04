/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 * resource_test.cpp: /gperun/test/resource_test.cpp
 *
 *  Created on: Feb 13, 2013
 *      Author: lichen
 ******************************************************************************/

#include <gtest/gtest.h>
#include <gutil/bitqueue.hpp>
#include <gutil/compactwriter.hpp>
#include <gutil/gcharbuffer.hpp>
#include <gutil/gsparsearray.hpp>
#include <gutil/compactwriter.hpp>
#include <gutil/gtimer.hpp>
#include <gutil/gsystem.hpp>
#include <gutil/goutputstream.hpp>
#include <gutil/filelinereader.hpp>
#include <gutil/gdiskutil.hpp>
#include <gmmnt/globaldiskio.hpp>
#include <boost/thread.hpp>

typedef gutil::GSparseArray<uint32_t> GSparseArray_t;

uint32_t gsparsearray_size_inbits_ = 24;
GSparseArray_t* sparsearray_;
bool gsparsearray_running_= true;
uint32_t gsparsearray_num_write_ = 0;
uint32_t gsparsearray_num_read_ = 0;
uint32_t gsparsearray_num_clean_ = 0;
uint32_t gsparsearray_gap_ = 128;

void GSparseArray_Write(){
  gutil::pthread_spinlock_t* ret_lock = NULL;
  //std::cout << "GSparseArray_WriteThread\n";
  for(uint32_t id = 0; id < (((uint32_t)1) << gsparsearray_size_inbits_); id+=gsparsearray_gap_){
    uint32_t* value = sparsearray_->GetPointerForWrite(id, ret_lock, true);
    ++(*value);
    gutil::pthread_spin_writeunlock(ret_lock);
  }
}

void GSparseArray_WriteThread(){
  while(gsparsearray_running_){
    GSparseArray_Write();
    ++gsparsearray_num_write_;
  }
}

void GSparseArray_Cleanup(){
  gutil::pthread_spinlock_t* ret_lock = NULL;
  //std::cout << "GSparseArray_CleanupThread\n";
  for(uint32_t id = 0; id < (((uint32_t)1) << gsparsearray_size_inbits_); id+=gsparsearray_gap_){
    uint32_t* value = sparsearray_->GetPointerForWrite(id, ret_lock, false);
    if(value != NULL){
      if(id % 7 == 0)
        sparsearray_->RemovePointer(id);
      gutil::pthread_spin_writeunlock(ret_lock);
    }
  }
}

void GSparseArray_CleanupThread(){
  while(gsparsearray_running_){
    GSparseArray_Cleanup();
    ++gsparsearray_num_clean_;
  }
}

void GSparseArray_Read(){
  gutil::pthread_spinlock_t* ret_lock = NULL;
  //std::cout << "GSparseArray_ReadThread\n";
  for(uint32_t id = 0; id < (((uint32_t)1) << gsparsearray_size_inbits_); id+=gsparsearray_gap_){
    uint32_t* value = sparsearray_->GetPointerForRead(id, ret_lock);
    if(value != NULL) {
      //std::cout << id << "," << *value << "\n";
      gutil::pthread_spin_readunlock(ret_lock);
      // std::cout << *ret_lock << "\n";
    }
  }
}

void GSparseArray_ReadThread(){
  while(gsparsearray_running_){
    GSparseArray_Read();
    ++gsparsearray_num_read_;
  }
}

void GSparseArray_SingleTest(){
  gutil::pthread_spinlock_t* ret_lock = NULL;
  for(size_t i = 0; i < (((uint32_t)1) << gsparsearray_size_inbits_) - 1; i+=gsparsearray_gap_){
    uint32_t* value = sparsearray_->GetPointerForWrite(i, ret_lock, true);
    ASSERT_EQ(*value, 0u);
    ++(*value);
    gutil::pthread_spin_writeunlock(ret_lock);

    value = sparsearray_->GetPointerForRead(i, ret_lock);
    ASSERT_EQ(*value, 1u);
    gutil::pthread_spin_readunlock(ret_lock);

    value = sparsearray_->GetPointerForWrite(i + 1, ret_lock, true);
    ASSERT_EQ(*value, 0u);
    sparsearray_->RemovePointer(i + 1);
    gutil::pthread_spin_writeunlock(ret_lock);

    value = sparsearray_->GetPointerForRead(i + 1, ret_lock);
    ASSERT_EQ(value == NULL, true);
  }
}

void GSparseArray_RunThreadTest(size_t num_writethread, size_t num_readthread, size_t num_cleanthread, size_t sleepseconds){
  gutil::GTimer timer;
  gsparsearray_num_write_ = 0;
  gsparsearray_num_read_ = 0;
  gsparsearray_num_clean_ = 0;
  gsparsearray_running_ = true;
  std::vector<boost::thread*> threads;
  for(size_t i = 0; i < num_writethread; ++i)
    threads.push_back(new boost::thread(&GSparseArray_WriteThread));
  for(size_t i = 0; i < num_cleanthread; ++i)
    threads.push_back(new boost::thread(&GSparseArray_CleanupThread));
  for(size_t i = 0; i < num_readthread; ++i)
    threads.push_back(new boost::thread(&GSparseArray_ReadThread));
  sleep(sleepseconds);
  gsparsearray_running_ = false;
  for(size_t i = 0; i < threads.size(); ++i){
    threads[i]->join();
    delete threads[i];
  }
  std::stringstream ss;
  ss << num_writethread  << " write threads finish " << gsparsearray_num_write_ << ", "
     << num_readthread << " read threads finish " <<  gsparsearray_num_read_ << ", "
     << num_cleanthread << " cleanup threads finish " << gsparsearray_num_clean_;
  timer.Stop(ss.str());
}

void TestBlockSize(uint32_t blocksize_inbits){
  std::cout << "Total array size " << (1 << gsparsearray_size_inbits_)
            << ", # of items " << ((1 << gsparsearray_size_inbits_) / gsparsearray_gap_)
            << ", block size " << (1 << blocksize_inbits) << "\n";
  sparsearray_ = new GSparseArray_t(gsparsearray_size_inbits_, blocksize_inbits);
  gutil::GTimer timer0;
  GSparseArray_SingleTest();
  timer0.Stop("Run GSparseArray_SingleTest with " + boost::lexical_cast<std::string>(
                gutil::GSystem::get_proc_virtual_memory()) + "MB memory");
  gutil::GTimer timer1;
  GSparseArray_Cleanup();
  timer1.Stop("Run GSparseArray_Cleanup once");
  gutil::GTimer timer2;
  GSparseArray_Write();
  timer2.Stop("Run GSparseArray_Write once");
  gutil::GTimer timer3;
  GSparseArray_Read();
  timer3.Stop("Run GSparseArray_Read once");
  size_t sleepseconds = 3;
  GSparseArray_RunThreadTest(0, 10, 0, sleepseconds);
  GSparseArray_RunThreadTest(1, 10, 0, sleepseconds);
  GSparseArray_RunThreadTest(2, 10, 0, sleepseconds);
  GSparseArray_RunThreadTest(2, 10, 1, sleepseconds);
  delete sparsearray_;
}

TEST(UTILTEST, SparseArray) {
  TestBlockSize(gsparsearray_size_inbits_ - 10);
  TestBlockSize(0);
}

TEST(UTILTEST, Write_Read_Text) {
  std::string file = "/tmp/write.txt";
  size_t linecount = 1 << 22;
  {
    gutil::GTimer timer;
    {
      std::ofstream filestream(file.c_str());
      gutil::GOutputStream out(2, &filestream);
      for(size_t i = 0; i < linecount; ++i){
        out.WriteUnsignedInt(i);
        out.put(',');
        out << "str_" << i;
        out.put(',');
        out.WriteUnsignedFloat(i);
        out.put('\n');
      }
    }
    timer.Stop("write " + boost::lexical_cast<std::string>(linecount) + " lines.");
  }
  {
    gutil::GTimer timer;
    size_t linenum = 0;
    {
      gutil::FileLineReader reader(file);
      uint64_t longvalue;
      char* strptr = NULL;
      size_t strlen = 0;
      double doublevalue = 0;
      while(reader.MoveNextLine()){
        ASSERT_EQ(reader.NextUnsignedLong(longvalue, ','), true);
        ASSERT_EQ(longvalue, linenum);
        ASSERT_EQ(reader.NextString(strptr, strlen, ','), true);
        strptr[strlen] = 0;
        ASSERT_EQ(strptr, "str_" + boost::lexical_cast<std::string>(linenum));
        ASSERT_EQ(reader.NextDouble(doublevalue, ','), true);
        ASSERT_EQ(std::abs(doublevalue - linenum) < 0.000001, true);
        linenum++;
      }
      ASSERT_EQ(linenum, linecount);
    }
    timer.Stop("read " + boost::lexical_cast<std::string>(linenum) + " lines.");
  }
  {
    gutil::GTimer timer;
    size_t linenum = 0;
    {
      gutil::FileLineReader reader(file);
      uint64_t longvalue;
      char* strptr = NULL;
      size_t strlen = 0;
      double doublevalue = 0;
      while(reader.MoveNextLine()){
        ASSERT_EQ(reader.NextUnsignedLong(longvalue, ','), true);
        ASSERT_EQ(longvalue, linenum);
        ASSERT_EQ(reader.NextString(strptr, strlen, ','), true);
        strptr[strlen] = 0;
        ASSERT_EQ(strptr, "str_" + boost::lexical_cast<std::string>(linenum));
        ASSERT_EQ(reader.NextDouble(doublevalue, ','), true);
        ASSERT_EQ(std::abs(doublevalue - linenum) < 0.000001, true);
        linenum++;
        if(linenum == 100) // test partial read
          break;
      }
    }
    timer.Stop("read " + boost::lexical_cast<std::string>(linenum) + " lines.");
  }
  gutil::FileLineReader::WordCount(file, ',');
}

TEST(UTILTEST, Write_Read_Binary) {
  std::string file = "/tmp/write.bin";
  size_t size = 1 << 20;
  {
    gutil::CompactWriter writer(file);
    for(size_t i = 0; i < size; ++i)
      writer.writeCompressed(i);
  }
  {
    size_t filesize = boost::filesystem::file_size(file);
    char* data = new char[filesize];
    gmmnt::GlobalDiskIO::readonefile(256 * 1024, file, filesize, data);
    uint8_t* ptr = (uint8_t*)data;
    for(size_t i = 0; i < size; ++i){
      ASSERT_EQ(gutil::CompactWriter::readCompressed(ptr), i);
    }
    delete[] data;
  }
}

void TestBitSet(bool sparsemode){
  size_t size = 6 << 20;
  gutil::BitQueue bitqueue(size, "id1");
  bitqueue.ResetBitQueue(sparsemode);
  for(size_t i = 0; i < size; i+=2){
    bitqueue.setbit(i);
  }
  for(size_t i = 0; i < size; i+=3){
    bitqueue.setbit(i);
  }
  ASSERT_EQ(bitqueue.dirtycount(), size * 4 / 6);
  bitqueue.SetInOrder();
  size_t count = 0;
  gutil::BitQueueIter iter;
  while(bitqueue.movenext(iter)){
    ASSERT_EQ(iter.cindex_ % 2 == 0 || iter.cindex_ % 3 == 0, true);
    count++;
  }
  ASSERT_EQ(bitqueue.dirtycount(), count);
}

TEST(UTILTEST, BitQueue) {
  TestBitSet(false);
  TestBitSet(true);
}

TEST(UTILTEST, GCharBuffer) {
  size_t size = 1 << 20;
  gutil::GCharBuffer buffer;
  std::string str = "12345";
  for(size_t i = 0; i < size; ++i){
    buffer.writeCompressed(i);
    buffer.write(const_cast<char*>(str.c_str()), (i % 5) + 1);
    buffer.put(0);
  }
  char* binary = buffer.buffer_;
  for(size_t i = 0; i < size; ++i){
    uint8_t* ptr = (uint8_t*)binary;
    ASSERT_EQ(gutil::CompactWriter::readCompressed(ptr), i);
    binary = (char*)ptr;
    ASSERT_EQ(strlen(binary), (i % 5) + 1);
    binary += strlen(binary) + 1;
  }
}


TEST(UTILTEST, DISABLED_TestingReadPerformance1) {
  std::string filename = "/Users/lichen/Downloads/dataset/twitter_rv.zip";
  char buffer[4096];
  {
    gutil::GTimer timer;
    FILE* file = fopen(filename.c_str(), "r");
    for(size_t i = 0; i < 4000; ++i)
      gmmnt::GlobalDiskIO::readfilebinary(file, i << 20, 4096, buffer);
    fclose(file);
    timer.Stop("fread");
  }
}

TEST(UTILTEST, DISABLED_TestingReadPerformance2) {
  //std::string filename = "/Users/lichen/Downloads/dataset/twitter_rv.zip";
  std::string filename = "/Volumes/A/twitter_rv.zip";
  char buffer[4096];
  {
    gutil::GTimer timer;
    int file = open(filename.c_str(), O_RDONLY);
    for(size_t i = 0; i < 4000; ++i){
      gmmnt::GlobalDiskIO::RawReadFileBinary(file, i << 20, 4096, buffer);
      if(i == 10)
        gutil::CompactWriter::PrintBytes(std::cout, "", (uint8_t*)buffer, 100);
    }
    close(file);
    timer.Stop("read");
  }
}

//--gtest_also_run_disabled_tests
TEST(UTILTEST, DISABLED_TestingReadPerformance3) {
  std::string filename = "/dev/rdisk1s2";
  {
    std::string srcfilename = "/Users/lichen/Downloads/dataset/twitter_rv.zip";
    size_t filesize = boost::filesystem::file_size(srcfilename);
    char* data = new char[filesize];
    gmmnt::GlobalDiskIO::readonefile(1 << 20, srcfilename,
                                     filesize, data);
    filesize -= filesize % 4096;
    gutil::GTimer timer;
    int file = open(filename.c_str(), O_RDWR| O_NONBLOCK);
    lseek(file, 0, SEEK_SET);
    char* startptr = data;
    char* endptr = data + (20 << 20);
    while(startptr < endptr){
      size_t writesize = write(file, startptr, 4096);
      GASSERT(writesize == 4096, writesize << "," << 4096);
      startptr += 4096;
    }
    close(file);
    timer.Stop("write /dev/rdisk");
    delete data;
  }
  char buffer[4096];
  {
    gutil::GTimer timer;
    int file = open(filename.c_str(), O_RDONLY);
    for(size_t i = 0; i < 20; ++i){
      gmmnt::GlobalDiskIO::RawReadFileBinary(file, i << 20, 4096, buffer);
      if(i == 10)
        gutil::CompactWriter::PrintBytes(std::cout, "", (uint8_t*)buffer, 100);
    }
    close(file);
    timer.Stop("read /dev/rdisk");
  }
}


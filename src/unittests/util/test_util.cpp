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
#include <gutil/compactwriter.hpp>
#include <gutil/jsonreader.hpp>
#include <gmmnt/globaldiskio.hpp>
#include <boost/thread.hpp>
#include <jsoncpp/include/json/json.h>

typedef gutil::GSparseArray<uint32_t> GSparseArray_t;

uint32_t gsparsearray_size_inbits_ = 24;
GSparseArray_t* sparsearray_;
bool gsparsearray_running_= true;
uint32_t gsparsearray_num_write_ = 0;
uint32_t gsparsearray_num_read_ = 0;
uint32_t gsparsearray_num_clean_ = 0;
uint32_t gsparsearray_gap_ = 128;

void GSparseArray_Write(){
  gutil::spinlock_mutex* ret_lock = NULL;
  //std::cout << "GSparseArray_WriteThread\n";
  for(uint32_t id = 0; id < (((uint32_t)1) << gsparsearray_size_inbits_); id+=gsparsearray_gap_){
    uint32_t* value = sparsearray_->GetPointerForWrite(id, ret_lock, true);
    ++(*value);
    gutil::spin_writeunlock(ret_lock);
  }
}

void GSparseArray_WriteThread(){
  while(gsparsearray_running_){
    GSparseArray_Write();
    ++gsparsearray_num_write_;
  }
}

void GSparseArray_Cleanup(){
  gutil::spinlock_mutex* ret_lock = NULL;
  //std::cout << "GSparseArray_CleanupThread\n";
  for(uint32_t id = 0; id < (((uint32_t)1) << gsparsearray_size_inbits_); id+=gsparsearray_gap_){
    uint32_t* value = sparsearray_->GetPointerForWrite(id, ret_lock, false);
    if(value != NULL){
      if(id % 7 == 0)
        sparsearray_->RemovePointer(id);
      gutil::spin_writeunlock(ret_lock);
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
  gutil::spinlock_mutex* ret_lock = NULL;
  //std::cout << "GSparseArray_ReadThread\n";
  for(uint32_t id = 0; id < (((uint32_t)1) << gsparsearray_size_inbits_); id+=gsparsearray_gap_){
    uint32_t* value = sparsearray_->GetPointerForRead(id, ret_lock);
    if(value != NULL) {
      //std::cout << id << "," << *value << "\n";
      gutil::spin_readunlock(ret_lock);
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
  gutil::spinlock_mutex* ret_lock = NULL;
  for(size_t i = 0; i < (((uint32_t)1) << gsparsearray_size_inbits_) - 1; i+=gsparsearray_gap_){
    uint32_t* value = sparsearray_->GetPointerForWrite(i, ret_lock, true);
    ASSERT_EQ(*value, 0u);
    ++(*value);
    gutil::spin_writeunlock(ret_lock);

    value = sparsearray_->GetPointerForRead(i, ret_lock);
    ASSERT_EQ(*value, 1u);
    gutil::spin_readunlock(ret_lock);

    value = sparsearray_->GetPointerForWrite(i + 1, ret_lock, true);
    ASSERT_EQ(*value, 0u);
    sparsearray_->RemovePointer(i + 1);
    gutil::spin_writeunlock(ret_lock);

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

void Write_Read_Text(std::string lineend, bool tofile) {
  std::string file = "/tmp/write.txt";
  size_t linecount = 1 << 22;
  std::string strdata = "";
  {
    gutil::GTimer timer;
    std::ostream* outstream;
    if(tofile)
      outstream = new std::ofstream(file.c_str());
    else
      outstream = new std::stringstream();
    {
      gutil::GOutputStream out(2, outstream);
      for(size_t i = 0; i < linecount; ++i){
        out.WriteUnsignedInt(i);
        out.put(',');
        out << "str_" << i;
        out.put(',');
        out.WriteUnsignedFloat(i);
        out << lineend;
      }
    }
    if(!tofile){
      strdata = ((std::stringstream*)outstream)->str();
    }
    delete outstream;
    timer.Stop("write " + boost::lexical_cast<std::string>(linecount) + " lines.");
  }
  {
    gutil::GTimer timer;
    size_t linenum = 0;
    {
      gutil::FileLineReader* reader;
      if(tofile)
        reader = new gutil::FileLineReader(file, 0, true, lineend);
      else
        reader = new gutil::FileLineReader(strdata.c_str(), strdata.size(), lineend);
      uint64_t longvalue;
      char* strptr = NULL;
      size_t strlen = 0;
      double doublevalue = 0;
      while(reader->MoveNextLine()){
        ASSERT_EQ(reader->NextUnsignedLong(longvalue, ','), true);
        ASSERT_EQ(longvalue, linenum);
        ASSERT_EQ(reader->NextString(strptr, strlen, ','), true);
        strptr[strlen] = 0;
        ASSERT_EQ(strptr, "str_" + boost::lexical_cast<std::string>(linenum));
        strptr[strlen] = ',';
        ASSERT_EQ(reader->NextDouble(doublevalue, ','), true);
        ASSERT_EQ(std::abs(doublevalue - linenum) < 0.000001, true);
        linenum++;
      }
      ASSERT_EQ(linenum, linecount);
      delete reader;
    }
    timer.Stop("read " + boost::lexical_cast<std::string>(linenum) + " lines.");
  }
  {
    gutil::GTimer timer;
    size_t linenum = 0;
    {
      gutil::FileLineReader* reader;
      if(tofile)
        reader = new gutil::FileLineReader(file, 0, true, lineend);
      else
        reader = new gutil::FileLineReader(strdata.c_str(), strdata.size(), lineend);
      uint64_t longvalue;
      char* strptr = NULL;
      size_t strlen = 0;
      double doublevalue = 0;
      while(reader->MoveNextLine()){
        ASSERT_EQ(reader->NextUnsignedLong(longvalue, ','), true);
        ASSERT_EQ(longvalue, linenum);
        ASSERT_EQ(reader->NextString(strptr, strlen, ','), true);
        strptr[strlen] = 0;
        ASSERT_EQ(strptr, "str_" + boost::lexical_cast<std::string>(linenum));
        strptr[strlen] = ',';
        ASSERT_EQ(reader->NextDouble(doublevalue, ','), true);
        ASSERT_EQ(std::abs(doublevalue - linenum) < 0.000001, true);
        linenum++;
        if(linenum == 100) // test partial read
          break;
      }
      delete reader;
    }
    timer.Stop("read " + boost::lexical_cast<std::string>(linenum) + " lines.");
  }
  {
    gutil::GTimer timer;
    size_t readsize = 0;
    {
      gutil::FileLineReader* reader;
      size_t filesize = 0;
      if(tofile){
        reader = new gutil::FileLineReader(file, 0, true, lineend);
        filesize = boost::filesystem::file_size(file);
      }
      else{
        reader = new gutil::FileLineReader(strdata.c_str(), strdata.size(), lineend);
        filesize = strdata.size();
      }
      while(reader->MoveNextLine(100000)){
          readsize += reader->lineend() - reader->linestart() + 1;
      }
      ASSERT_EQ(readsize, filesize);
      delete reader;
    }
    timer.Stop("read " + boost::lexical_cast<std::string>(readsize) + " bytes.");
  }
  {
    gutil::FileLineReader* reader;
    if(tofile)
      reader = new gutil::FileLineReader(file, 0, true, lineend);
    else
      reader = new gutil::FileLineReader(strdata.c_str(), strdata.size(), lineend);
    reader->WordCount(',');
    delete reader;
  }
}

TEST(UTILTEST, Write_Read_Text){
  Write_Read_Text("\r\n", true);
  Write_Read_Text("\n", true);
  Write_Read_Text("&", true);
  Write_Read_Text("\r\n", false);
  Write_Read_Text("\n", false);
  Write_Read_Text("&", false);
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
    gmmnt::GlobalDiskIO::readonefile(file, data, filesize);
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

TEST(UTILTEST, CompactWriter) {
  char buf[100];
  char* tmpptr = buf;
  uint8_t* tmpptr2 = (uint8_t*)buf;
  for(size_t i = 1; i <= 8; ++i){
    uint64_t test1 = (((uint64_t)1) << ((i  - 1)* 7));
    uint64_t test2 = (((uint64_t)1) << (i * 7)) - 1;
    // std::cout << test1 << ", " << test2 << "\n";
    ASSERT_EQ(gutil::CompactWriter::GetCompressedByteSize(test1), i);
    ASSERT_EQ(gutil::CompactWriter::GetCompressedByteSize(test2), i);
    tmpptr = buf;
    ASSERT_EQ(gutil::CompactWriter::writeCompressed(test1, tmpptr), i);
    tmpptr = buf;
    ASSERT_EQ(gutil::CompactWriter::GetCompressedSize((uint8_t*)tmpptr), i);
    tmpptr2 = (uint8_t*)buf;
    ASSERT_EQ(gutil::CompactWriter::readCompressed(tmpptr2), test1);
    tmpptr = buf;
    ASSERT_EQ(gutil::CompactWriter::writeCompressed(test2, tmpptr), i);
    tmpptr = buf;
    ASSERT_EQ(gutil::CompactWriter::GetCompressedSize((uint8_t*)tmpptr), i);
    tmpptr2 = (uint8_t*)buf;
    ASSERT_EQ(gutil::CompactWriter::readCompressed(tmpptr2), test2);
  }
}

bool JSONReader(std::string& input, std::ostream* os) {
  gutil::GTimer timer;
  gutil::JSONReader reader((char*)input.c_str(), input.size());
  if(!reader.StartObject()) return false;
  std::string name;
  std::string strvalue;
  double doublevalue = 0;
  int64_t intvalue = 0;
  bool boolvalue = false;
  std::string vid, srcvid, tgtvid, vtype, srcvtype, tgtvtype, etype;
  long checksum = 0;
  while(true){
    bool hasmore = reader.GetName(name);
    if(!hasmore)
      break;
    if(name == "vertices"){
      if(!reader.StartArray()) return false;
      while(reader.ArrayHasMore()){
        if(!reader.StartObject()) return false;
        if(!reader.GetName(name) && name != "id") return false;
        if(!reader.GetString(vid)) return false;
        if(!reader.GetName(name) && name != "type") return false;
        if(!reader.GetString(vtype)) return false;
        reader.GetName(name);
        if(name == "v0_a1"){
          if(!reader.GetInt(intvalue)) return false;
          reader.GetName(name);
        }
        else
          intvalue = -1;
        if(os != NULL)
          (*os) << "vertex " << vid << ", " << vtype << ", " << intvalue << "\n";
        checksum += vid.size() + vtype.size() + intvalue;
        if(name.size() > 0)  return false;
        if(!reader.EndObject()) return false;
      }
    } else if(name == "edges"){
      if(!reader.StartArray()) return false;
      while(reader.ArrayHasMore()){
        if(!reader.StartObject()) return false;
        if(!reader.GetName(name) && name != "srcid") return false;
        if(!reader.GetString(srcvid)) return false;
        if(!reader.GetName(name) && name != "srctype") return false;
        if(!reader.GetString(srcvtype)) return false;
        if(!reader.GetName(name) && name != "tgtid") return false;
        if(!reader.GetString(tgtvid)) return false;
        if(!reader.GetName(name) && name != "tgttype") return false;
        if(!reader.GetString(tgtvtype)) return false;
        if(!reader.GetName(name) && name != "type") return false;
        if(!reader.GetString(etype)) return false;
        if(os != NULL)
          (*os) << "edge " << srcvid << ", " << srcvtype << ", " << tgtvid << ", " << tgtvtype<< ", " << etype;
        checksum += srcvid.size() + srcvtype.size() + tgtvid.size() + tgtvtype.size() + etype.size() ;
        /*
         // the way to go through attribute list and match with reader.
         reader.GetName(name);
         for(size_t i = 0; i < attrmeta.size(); ++i){
           if(name == attrmeta[i].name_){
             1. get value accroding to attribute type
             2. write it to binary
             3. reader.GetName(name); // move to next reader item
           } else
             //write default value by attribute type
         }
         //name should be empty right here. Otherwise json provide something extra which we did not handle.
         */
        if(etype == "etype0"){ //\"type\":\"etype0\",\"e0_a1\":10,\"e0_a2\":\"abc\"
          reader.GetName(name);
          if(name == "e0_a1"){
            if(!reader.GetInt(intvalue)) return false;
            reader.GetName(name);
          }
          else
            intvalue = -1;
          if(name == "e0_a2"){
            if(!reader.GetString(strvalue)) return false;
            reader.GetName(name);
          }
          else
            strvalue = "";
          if(os != NULL)
            (*os) << ", " << intvalue << ", " << strvalue;
          checksum += intvalue + strvalue.size();
        } else if(etype == "etype1"){ //\"type\":\"etype1\",\"e1_a1\":true,\"e1_a2\":10.07
          reader.GetName(name);
          if(name == "e1_a1"){
            if(!reader.GetBool(boolvalue)) return false;
            reader.GetName(name);
          }
          else
            boolvalue = false;
          if(name == "e1_a2"){
            if(!reader.GetDouble(doublevalue)) return false;
            reader.GetName(name);
          } else
            doublevalue = -1;
          if(os != NULL)
            (*os) << ", " << boolvalue << ", " << doublevalue;
          checksum += boolvalue + (long)doublevalue;
        }
        if(os != NULL)
          (*os) << "\n";
        if(name.size() > 0)  return false;
        if(!reader.EndObject()) return false;
      }
    } else {
      if(!reader.GetString(strvalue)) return false;
      if(os != NULL)
        (*os) << name << ": " << strvalue << "\n";
      checksum += strvalue.size();
    }
  }
  if(!reader.EndObject()) return false;
  timer.Stop("JSONReader parse " + boost::lexical_cast<std::string>(input.size()) + " bytes json string with checksum " + boost::lexical_cast<std::string>(checksum));
  return true;
}

void JSONCPP(std::string& input, std::ostream* os){
  gutil::GTimer timer;
  Json::Value dataRoot;
  Json::Reader jsonReader;
  jsonReader.parse(input, dataRoot);
  std::string strvalue;
  double doublevalue = 0;
  int64_t intvalue = 0;
  bool boolvalue = false;
  std::string vid, srcvid, tgtvid, vtype, srcvtype, tgtvtype, etype;
  long checksum = 0;
  std::string name ="action";
  strvalue = dataRoot[name].asString();
  checksum += strvalue.size();
  if(os != NULL)
    (*os) << name << ": " << strvalue << "\n";
  name ="behavior";
  strvalue = dataRoot[name].asString();
  checksum += strvalue.size();
  if(os != NULL)
    (*os) << name << ": " << strvalue << "\n";
  Json::Value& vertices =  dataRoot["vertices"];
  for(int i = 0; i < (int)vertices.size(); ++i){
    Json::Value& vertex =  vertices[i];
    vid = vertex["id"].asString();
    vtype = vertex["type"].asString();
    intvalue = vertex["v0_a1"].asInt();
    if(os != NULL)
      (*os) << "vertex " << vid << ", " << vtype << ", " << intvalue << "\n";
    checksum += vid.size() + vtype.size() + intvalue;
  }
  Json::Value& edges =  dataRoot["edges"];
  for(int i = 0; i < (int)edges.size(); ++i){
    Json::Value& edge =  edges[i];
    srcvid = edge["srcid"].asString();
    srcvtype = edge["srctype"].asString();
    tgtvid = edge["tgtid"].asString();
    tgtvtype = edge["tgttype"].asString();
    etype = edge["type"].asString() ;
    if(os != NULL)
      (*os) << "edge " << srcvid << ", " << srcvtype  << ", " << tgtvid  << ", " << tgtvtype << ", " << etype;
    checksum += srcvid.size() + srcvtype.size() + tgtvid.size() + tgtvtype.size() + etype.size() ;
    if(etype == "etype0"){
      intvalue = edge["e0_a1"].asInt();
      strvalue =  edge["e0_a2"].asString();
      if(os != NULL)
        (*os) << ", " << intvalue << ", " << strvalue;
      checksum += intvalue + strvalue.size();
    }
    else if(etype == "etype1"){
      boolvalue = edge["e1_a1"].asBool();
      doublevalue = edge["e1_a2"].asDouble();
      if(os != NULL)
        (*os) << ", " << boolvalue  << ", " << doublevalue;
      checksum += boolvalue + (long)doublevalue;
    }
    if(os != NULL)
      (*os) << "\n";
  }
  timer.Stop("JsonCPP parse " + boost::lexical_cast<std::string>(input.size()) + " bytes json string with checksum " + boost::lexical_cast<std::string>(checksum));
}

TEST(UTILTEST, JSONReader) {
  size_t loop = 1000000;
  std::stringstream ss;
  ss << "{\"action\":\"insert\",\"behavior\":\"consistent\",\"vertices\":[";
  for(size_t i = 0; i < loop; ++i)
    ss << (i == 0 ? "" : ",") << "{\"id\":\"user1\",\"type\":\"vtype0\",\"v0_a1\":10},{\"id\":\"user2\",\"type\":\"vtype0\",\"v0_a1\":11}";
  ss << "],\"edges\":[";
  for(size_t i = 0; i < loop; ++i)
    ss << (i == 0 ? "" : ",") << "{\"srcid\":\"user1\",\"srctype\":\"vtype0\",\"tgtid\":\"user2\",\"tgttype\":\"vtype0\",\"type\":\"etype0\",\"e0_a1\":10,\"e0_a2\":\"abc\"},{\"srcid\":\"user1\",\"srctype\":\"vtype0\",\"tgtid\":\"user2\",\"tgttype\":\"vtype0\",\"type\":\"etype1\",\"e1_a1\":true,\"e1_a2\":10.07}";
  ss << "]}";
  std::string input = ss.str();
  ASSERT_EQ(JSONReader(input, NULL), true);
  JSONCPP(input, NULL);
}

TEST(UTILTEST, DISABLED_TestingReadPerformance1) {
  std::string filename = "/Users/lichen/Downloads/dataset/twitter_rv.zip";
  char buffer[4096];
  {
    gutil::GTimer timer;
    int file = open(filename.c_str(), O_RDONLY);
    for(size_t i = 0; i < 4000; ++i)
      gmmnt::GlobalDiskIO::RawReadFileBinary(file, i << 20, 4096, buffer);
    close(file);
    timer.Stop("C read");
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
    gmmnt::GlobalDiskIO::readonefile(srcfilename, data, filesize);
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


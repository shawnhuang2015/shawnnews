/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 * test_ext_gpe.cpp: /unittest/core_ext/test_ext_gpe.cpp
 *
 *  Created on: Oct 13, 2014
 *      Author: lichen
 ******************************************************************************/

#include <gtest/gtest.h>
#include "../../core_ext/gpe/unionfind.hpp"
#include "../../core_ext/gpe/global_vector.hpp"
#include "../../core_ext/gpe/global_set.hpp"
#include "../../core_ext/gpe/global_heap.hpp"

/// TODO add unit test for core_ext gpe

TEST(Ext_GPE, UnionFind) {
  int size = 1 << 20;
  {
    gperun::UnionFind unionfind(size);
    for(int i = size - 1; i > 0; --i)
      unionfind.union_sets(i, i - 1);
    ASSERT_EQ(unionfind.numGroups(), 1);
    for(int i = 0; i < size; ++i){
      ASSERT_EQ(unionfind.findParent(i), size -1);
    }
  }
  {
    gperun::UnionFind unionfind(size);
    for(int i = 1; i < size; ++i)
      unionfind.union_sets(i - 1, i);
    ASSERT_EQ(unionfind.numGroups(), 1);
    for(int i = 0; i < size; ++i){
      ASSERT_EQ(unionfind.findParent(i), 0);
    }
  }
}

TEST(Ext_GPE, GlobalVector) {
  typedef UDIMPL::VectorVariable<int> Variable_t;
  int size = 1 << 20;
  int maxsize = 2 * size + 100;
  Variable_t vectorvariable(maxsize);
  Variable_t* copy1 = (Variable_t*)vectorvariable.MakeLocalCopy();
  for(int i = 0; i < size; ++i)
    copy1->Reduce(&i);
  vectorvariable.Combine(copy1);
  delete copy1;
  ASSERT_EQ(vectorvariable.getResults().size(), size);
  Variable_t* copy2 = (Variable_t*)vectorvariable.MakeLocalCopy();
  for(int i = size; i < 2 * size; ++i)
    copy2->Reduce(&i);
  vectorvariable.Combine(copy2);
  delete copy2;
  ASSERT_EQ(vectorvariable.getResults().size(), 2 * size);
  Variable_t* copy3 = (Variable_t*)vectorvariable.MakeLocalCopy();
  for(int i = size * 2; i < size * 3; ++i)
    copy3->Reduce(&i);
  vectorvariable.Combine(copy3);
  delete copy3;
  ASSERT_EQ(vectorvariable.getResults().size(), maxsize);
}

TEST(Ext_GPE, GlobalSet) {
  typedef UDIMPL::SetVariable<int> Variable_t;
  int size = 1 << 20;
  Variable_t setvariable;
  Variable_t* copy1 = (Variable_t*)setvariable.MakeLocalCopy();
  for(int i = 0; i < size; ++i)
    copy1->Reduce(&i);
  setvariable.Combine(copy1);
  delete copy1;
  ASSERT_EQ(setvariable.getResults().size(), size);
  Variable_t* copy2 = (Variable_t*)setvariable.MakeLocalCopy();
  for(int i = size / 2; i < size + size / 2; ++i)
    copy2->Reduce(&i);
  setvariable.Combine(copy2);
  delete copy2;
  ASSERT_EQ(setvariable.getResults().size(), size + size / 2);
  Variable_t* copy3 = (Variable_t*)setvariable.MakeLocalCopy();
  for(int i = size; i < 2 * size; ++i)
    copy3->Reduce(&i);
  setvariable.Combine(copy3);
  delete copy3;
  ASSERT_EQ(setvariable.getResults().size(), 2 * size);
}


TEST(Ext_GPE, GlobalHeap) {
  typedef UDIMPL::HeapVariable<int> Variable_t;
  int size = 1 << 20;
  int maxsize = 1 << 10;
  Variable_t heapvariable(maxsize);
  Variable_t* copy1 = (Variable_t*)heapvariable.MakeLocalCopy();
  for(int i = 0; i < size; ++i)
    copy1->Reduce(&i);
  heapvariable.Combine(copy1);
  delete copy1;
  ASSERT_EQ(heapvariable.size(), maxsize);
  Variable_t* copy2 = (Variable_t*)heapvariable.MakeLocalCopy();
  for(int i = size / 2; i < size + size / 2; ++i)
    copy2->Reduce(&i);
  heapvariable.Combine(copy2);
  delete copy2;
  ASSERT_EQ(heapvariable.size(), maxsize);
  Variable_t* copy3 = (Variable_t*)heapvariable.MakeLocalCopy();
  for(int i = size; i < 2 * size; ++i)
    copy3->Reduce(&i);
  heapvariable.Combine(copy3);
  delete copy3;
  ASSERT_EQ(heapvariable.size(), maxsize);

  // ADAM: added this to test new features.
  // 1.  re-size heap.
  int newMaxSize = 1 << 20;
  heapvariable.resize_heap(newMaxSize);
  // 1.a increasing size should leave variable unchnaged.
  ASSERT_EQ(heapvariable.size(), maxsize);
  // original heap should be filled with [0..maxsize-1]
  // we should be able to add maxsize, maxsize+1 to verify we can go past the old capacity by 2
  heapvariable.Reduce(maxsize);
  heapvariable.Reduce(maxsize+1);
  ASSERT_EQ(heapvariable.size(),maxsize+2);


  // 1.b decreasing size should truncate the heap.
  newMaxSize = 1<<9;
  heapvariable.resize_heap(newMaxSize);
  ASSERT_EQ(heapvariable.size(),newMaxSize);

  // are the results sorted at the end?
  std::cout << heapvariable.getFinalResults().size() << "\n";
  std::vector<int>& intvec = heapvariable.getFinalResults();
  for(int i = 0; i < intvec.size(); ++i){
    ASSERT_EQ(intvec[i], i);
  }

  // 2.  clear the heap, it should now be empty.
  heapvariable.clear_heap();
  ASSERT_EQ(heapvariable.size(), 0);

}

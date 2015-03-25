/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 *
 * Created on: 04-09-2014
 * Author: ntietz
 *
 * An easy-to-use global heap. It can be unbounded or you can specify the
 * maximum number of elements you want to store.
 ******************************************************************************
 */

#ifndef GPERUN_COMMON_GLOBAL_HEAP_HPP_
#define GPERUN_COMMON_GLOBAL_HEAP_HPP_

#include <gpelib4/enginebase/variable.hpp>
#include <algorithm>
#include <functional>

using namespace gpelib4;

namespace UDIMPL {

  /** A HeapVariable lets you store a sorted list of results with an optional limit.
   *
   * The HeapVariable is a max-heap, so the root of the tree is the highest value.
   * When you insert new elements, if (size > maxSize), then the largest element
   * will get removed (so if "A < B" is "true", then B gets removed before A gets
   * removed).
   *
   * This is counter-intuitive behavior so be VERY CAREFUL. Consider that you have a
   * heap with standard operator< and limit of 2. (below, I show it as a queue)
   *  add "1": [] -> [1]
   *  add "2": [1] -> [2,1]
   *  add "3": [2,1] -> [3,2,1] -> [2,1]
   *  add "0": [2,1] -> [2,1,0] -> [1,0]
   * So if you wanted to keep the top-k, then you would define operator< in a way that
   * inverts the ordering.
   *
   * The first argument, ELEMENT_t, is just your data member. If you do not define
   * a comparator, then it must have operator< defined, else the comparator will not
   * compile.
   *
   * The second argument, COMPARATOR_t, must be a function object implementing
   * the function <(ELEMENT_t x, ELEMENT_t y) so you can do "x < y".
   */
  template <typename ELEMENT_t, typename COMPARATOR_t = std::less<ELEMENT_t> >
  class HeapVariable : public BaseVariableObject {
   public:
    HeapVariable(): comparator_(), contents_(),
        maxSize_(std::numeric_limits<uint32_t>::max()), finalsorted_(false) { }
    HeapVariable(uint32_t maxSize): comparator_(),
        contents_(), maxSize_(maxSize), finalsorted_(false) { }

    ~HeapVariable() { }

    BaseVariableObject* MakeLocalCopy() const {
      return new HeapVariable<ELEMENT_t, COMPARATOR_t>(maxSize_);
    }

    void Combine(BaseVariableObject* other) {
      HeapVariable<ELEMENT_t> *otherGlobalHeap = (HeapVariable<ELEMENT_t>*)other;
      std::vector<ELEMENT_t> otherHeap = otherGlobalHeap->contents_;

      while (otherHeap.size() > 0) {
        // Pop from other heap and add it to this heap
        std::pop_heap(otherHeap.begin(), otherHeap.end(), comparator_);
        contents_.push_back(otherHeap.back());
        otherHeap.pop_back();
        std::push_heap(contents_.begin(), contents_.end(), comparator_);

        // Enforce maximum heap size
        while (contents_.size() > maxSize_) {
          std::pop_heap(contents_.begin(), contents_.end(), comparator_);
          contents_.pop_back();
        }
      }
    }

    void Reduce(const void *newValue) {
      const ELEMENT_t* newElement = reinterpret_cast<const ELEMENT_t*>(newValue);

      contents_.push_back(*newElement);
      std::push_heap(contents_.begin(), contents_.end(), comparator_);

      // Enforce maximum heap size
      while (contents_.size() > maxSize_) {
        std::pop_heap(contents_.begin(), contents_.end(), comparator_);
        contents_.pop_back();
      }
    }

    /** This function returns the results in sorted order.
     *
     * It should only be called at the end of all execution, because it
     * destroys the heap by putting it into a normal sorted order.
     */
    std::vector<ELEMENT_t>& getFinalResults() {
      if(!finalsorted_){
        std::sort_heap(contents_.begin(), contents_.end(), comparator_);
        finalsorted_ = true;
      }
      return contents_;
    }


    void clear_heap(){
      contents_.clear();
      finalsorted_ = false;
    }

    void resize_heap(uint32_t newSize){
      maxSize_ = newSize;
      if(contents_.size() > maxSize_){
        // Enforce maximum heap size
        while (contents_.size() > maxSize_) {
          std::pop_heap(contents_.begin(), contents_.end(), comparator_);
          contents_.pop_back();
        }
      }
    }


    size_t size() {
      return contents_.size();
    }

    void* GetValuePtr() {
      return this;
    }

    // serialization method
    void WriteTo(std::ostream& ostream) {
      std::ostream_iterator<ELEMENT_t> out_iter(ostream, " ");
      std::copy(contents_.begin(), contents_.end(), out_iter);
    }

    // de-serialization method
    void Loadfrom(std::istream& istream) {
      std::istream_iterator<ELEMENT_t> in_iter(istream);
      contents_ = std::vector<ELEMENT_t>(in_iter, std::istream_iterator<ELEMENT_t>());
    }

    COMPARATOR_t comparator_;
    std::vector<ELEMENT_t> contents_;
    uint32_t maxSize_;
    bool finalsorted_;
  };

}

#endif

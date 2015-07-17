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
      printf("DEBUG: make local copy\n");
      HeapVariable* copy = new HeapVariable<ELEMENT_t, COMPARATOR_t>(maxSize_);
      // This might not be the best idea.
      copy->comparator_ = comparator_;
      return copy;
    }

    void Combine(BaseVariableObject* other) {
      HeapVariable<ELEMENT_t> *otherGlobalHeap = (HeapVariable<ELEMENT_t>*)other;
      typename GVector<ELEMENT_t>::T otherHeap = otherGlobalHeap->contents_;

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


//    this heap is strange and confusing, even to those who developed it!
//    here's the story:  think of this heap as holding all of the "best" things that you want
//                       where the very BEST thing is actually at the bottom and the WORST thing
//                       is at the very top of the heap, easy to find.  And the size is limited,
//                       so when the heap is full, and we find something BETTER than the WORST item
//                       (by the code, this means that comparator_(New, WORST) == true, we will get
//                       rid of the worst thing by popping the heap, and push the new thing on.
//    ok, let's summarize the steps:
//    0)  if size < maxSize  Push_heap(New)
//    1)  Comp(New,Heap[0]) == true:  this means that Heap[0] will be removed from a full heap.
//    a)  if Comp(New, Heap[1]) == false && Comp(New, Heap[2]) == false  then Heap[0] = New
//    b)  else:  Pop_heap(),  Push_heap(New)

    void Reduce(const void *newValue) {
      const ELEMENT_t newElement = *(reinterpret_cast<const ELEMENT_t*>(newValue));
      if(contents_.size() < maxSize_) {
        contents_.push_back(newElement);
        std::push_heap(contents_.begin(), contents_.end(), comparator_);
      }else if(comparator_(newElement,contents_[0]) == true) {
        // optimization: the next two 'worst' items are in positions 1 and 2.
        // if we are better than the WORST--contents_[0]  but not contents_[1] and contents_[2]
        // then we can just emplace the new item as the current root.
        if(comparator_(newElement,contents_[1]) == false && comparator_(newElement,contents_[2]) == false) {
          contents_[0] = newElement;
        }else {
          std::pop_heap(contents_.begin(), contents_.end(), comparator_);
          contents_[contents_.size()-1] = newElement;
          std::push_heap(contents_.begin(), contents_.end(), comparator_);
        }
      }
    }

    /** This function returns the results in sorted order.
     *
     * It should only be called at the end of all execution, because it
     * destroys the heap by putting it into a normal sorted order.
     */
typename GVector<ELEMENT_t>::T& getFinalResults() {
      if(!finalsorted_){
        std::sort_heap(contents_.begin(), contents_.end(), comparator_);
        finalsorted_ = true;
      }
      return contents_;
    }

    void resetComparator(const COMPARATOR_t& cmpr) {
      comparator_ = cmpr;
    }

    // In case `comparator_' changes, need to re-heapify the contents using the new comparator.
    void heapify() {
      std::make_heap(contents_.begin(), contents_.end(), comparator_);
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
      contents_ = typename GVector<ELEMENT_t>::T(in_iter, std::istream_iterator<ELEMENT_t>());
    }

    COMPARATOR_t comparator_;
    typename GVector<ELEMENT_t>::T contents_;
    uint32_t maxSize_;
    bool finalsorted_;
  };

}

#endif

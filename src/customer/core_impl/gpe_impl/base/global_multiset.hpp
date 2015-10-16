/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 *
 * Created on: 04-08-2014
 * Author: Eric Chu
 *
 * An easy to use gloabl Set implementation
 ******************************************************************************
 */

#ifndef GPERUN_COMMON_GLOBAL_MULTISET_HPP_
#define GPERUN_COMMON_GLOBAL_MULTISET_HPP_

#include <gpelib4/enginebase/variable.hpp>
#include <boost/range/algorithm/set_algorithm.hpp>

using namespace gpelib4;

namespace UDIMPL {

/***
     * MultiSetVariable: define a gloable multi Set variable for storing all
     * the result records
     */
template <typename ELEMENT_t, typename hasher = boost::hash<ELEMENT_t> >
class MultiSetVariable : public BaseVariableObject {
public:
  typedef boost::unordered_multiset<ELEMENT_t, hasher> Set_t;

  MultiSetVariable() {}

  ~MultiSetVariable() {}

  MultiSetVariable(Set_t& copy){
    localResults_ = Set_t(copy);
  }

  BaseVariableObject* MakeLocalCopy() const {
    return new MultiSetVariable<ELEMENT_t,hasher>();
  }

  void Combine(BaseVariableObject *other) {
    MultiSetVariable<ELEMENT_t,hasher> *otherSet = (MultiSetVariable<ELEMENT_t,hasher> *)other;
    localResults_.insert(otherSet->localResults_.begin(), otherSet->localResults_.end());
  }

  void add(ELEMENT_t result) {
    localResults_.insert(result);
  }

  void Reduce(const void *newValue) {
    add(*(reinterpret_cast<const ELEMENT_t*>(newValue)));
  }

  void * GetValuePtr(){
    return &localResults_;
  }

  Set_t& getResults() {
    return localResults_;
  }

  /// serialization method
  void WriteTo(std::ostream& ostream) {
    std::ostream_iterator<ELEMENT_t> out_it(ostream," ");
    std::copy(localResults_.begin(), localResults_.end(), out_it);
  }

  /// de-serialization method
  void LoadFrom(std::istream& istream) {
    std::istream_iterator<ELEMENT_t> in_it(istream);
    localResults_ = boost::unordered_multiset<ELEMENT_t,hasher>(in_it, std::istream_iterator<ELEMENT_t>());
  }


private:
  Set_t localResults_;
};
}

#endif


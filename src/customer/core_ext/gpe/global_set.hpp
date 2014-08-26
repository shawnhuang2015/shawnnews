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

#ifndef GPERUN_COMMON_GLOBAL_SET_HPP_
#define GPERUN_COMMON_GLOBAL_SET_HPP_

#include <gpelib4/enginebase/variable.hpp>
#include <boost/unordered_set.hpp>

using namespace gpelib4;

namespace UDIMPL {

/***
     * SetVariable: define a gloable Set variable for storing all
     * the result records
     */
template <typename ELEMENT_t, typename hasher = boost::hash<ELEMENT_t> >
class SetVariable : public BaseVariableObject {
public:
  SetVariable() {}

  ~SetVariable() {}

  SetVariable(boost::unordered_set<ELEMENT_t,hasher> copy){
    localResults_ = boost::unordered_set<ELEMENT_t,hasher>(copy);
  }

  BaseVariableObject* MakeLocalCopy() const {
    return new SetVariable<ELEMENT_t,hasher>();
  }

  void Combine(BaseVariableObject *other) {
    SetVariable<ELEMENT_t,hasher> *otherSet = (SetVariable<ELEMENT_t,hasher> *)other;
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

  boost::unordered_set<ELEMENT_t,hasher>& getResults() {
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
    localResults_ = boost::unordered_set<ELEMENT_t,hasher>(in_it, istream_iterator<ELEMENT_t>());
  }


private:
  boost::unordered_set<ELEMENT_t,hasher> localResults_;
};
}

#endif

/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 *
 * Created on: 04-08-2014
 * Author: Eric Chu
 *
 * An easy to use gloabl vector implementation
 ******************************************************************************
 */

#ifndef GPERUN_COMMON_GLOBAL_VECTOR_HPP_
#define GPERUN_COMMON_GLOBAL_VECTOR_HPP_

#include <gpelib4/enginebase/variable.hpp>

using namespace gpelib4;

namespace gperun {

/***
     * VectorVariable: define a gloable vector variable for storing all
     * the result records
     */
template <typename ELEMENT_t>
class VectorVariable : public BaseVariableObject {
public:
  VectorVariable() {}

  ~VectorVariable() {}

  VectorVariable(std::vector<ELEMENT_t> copy){
    localResults_ = std::vector<ELEMENT_t>(copy);
  }

  BaseVariableObject* MakeLocalCopy() const {
    return new VectorVariable<ELEMENT_t>();
  }

  void Combine(BaseVariableObject *other) {
    VectorVariable<ELEMENT_t> *otherVector = (VectorVariable<ELEMENT_t> *)other;
    localResults_.insert(localResults_.end(), \
                         otherVector->localResults_.begin(), otherVector->localResults_.end());
  }

  void add(ELEMENT_t result) {
    localResults_.push_back(result);
  }

  void Reduce(const void *newValue) {
    add(*(reinterpret_cast<const ELEMENT_t*>(newValue)));
  }

  void * GetValuePtr(){
    return &localResults_;
  }

  std::vector<ELEMENT_t>& getResults() {
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
    localResults_ = std::vector<ELEMENT_t>(in_it, istream_iterator<ELEMENT_t>());
  }


private:
  std::vector<ELEMENT_t> localResults_;
};
}

#endif

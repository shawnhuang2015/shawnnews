/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 *
 * Created on: Jan 16, 2015
 * Author: ntietz, based on work by Xinyu Chang
 *
 * An easy to use gloabl Map implementation
 ******************************************************************************
 */

#ifndef GPERUN_COMMON_GLOBAL_MAP_HPP_
#define GPERUN_COMMON_GLOBAL_MAP_HPP_

#include <gpelib4/enginebase/variable.hpp>
#include <boost/unordered_map.hpp>

using namespace gpelib4;

namespace UDIMPL {

  template <typename KEY, typename VALUE>
  class MapVariable : public BaseVariableObject {
   public:
    MapVariable() {
      // Nothing to construct.
    }

    MapVariable(boost::unordered_map<KEY,VALUE> m) : map_(m) {
      // Nothing to construct.
    }

    ~MapVariable() {
      // Nothing to destruct.
    }

    BaseVariableObject* MakeLocalCopy() const {
      return new MapVariable<KEY,VALUE>();
    }

    void Combine(BaseVariableObject* other) {
      typename boost::unordered_map<KEY,VALUE>::iterator iter;
      MapVariable<KEY,VALUE>* otherMap = reinterpret_cast<MapVariable<KEY,VALUE>*>(other);
      for (iter = otherMap->map_.begin(); iter != otherMap->map_.end(); ++iter) {
        add(iter->first, iter->second);
      }
    }

    void add(KEY key, VALUE value) {
      if (!contains(key)) {
        map_[key] = value;
      } else {
        (*map_[key]) += (*value);
      }
    }

    VALUE get(KEY key) {
      return map_[key];
    }

    uint32_t getSize() {
      return map_.size();
    }

    bool contains(KEY key) {
      return (map_.find(key) != map_.end());
    }

    void Reduce(const void* newValue) {
      typedef std::pair<KEY,VALUE> kvpair;
      const kvpair* pair = reinterpret_cast<const kvpair*>(newValue);
      add(pair->first, pair->second);
    }

    void* GetValuePtr() {
      return &map_;
    }

    void WriteTo(std::ostream& ostream) {
      // This method is not implemented for the single-machine mode.
    }

    void LoadFrom(std::istream& istream) {
      // This method is not implemented for the single-machine mode.
    }

   private:
    boost::unordered_map<KEY,VALUE> map_;
  };

}

#endif


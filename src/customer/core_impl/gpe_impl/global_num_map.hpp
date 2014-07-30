/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: Neo4J Benchmarking
 *
 * Created on: 7/30/2014
 * Author: Arne Leitert
 * Description: This is a hashmap to use as global variable in a UDF. It
 *              stores key value pairs which both have a numeric type.
 ******************************************************************************/

namespace UDIMPL {

  template <class keytype, class valtype, class hasher = boost::hash<keytype> >
  class GlobalNumericMap : public BaseVariableObject {
    typedef boost::unordered_map<keytype,valtype,hasher> MAP;
    MAP value_map_;
    bool islocalcopy_;
  public:
    GlobalNumericMap(){
      value_map_ = MAP();
      islocalcopy_ = false;
    }

    GlobalNumericMap(const GlobalNumericMap& other){
      value_map_ = MAP(other.value_map_);
      islocalcopy_ = false;
    }

    valtype& operator[](keytype const& k){
      return value_map_[k];
    }

    BaseVariableObject* MakeLocalCopy() const {
      GlobalNumericMap * copy = new GlobalNumericMap(*this);
      copy->islocalcopy_ = true;
      return copy;
    }

    void * GetValuePtr(){
      return this;
    }

    void Reduce(const void* newvalue){
      // nothing to do here...this function is not called.
      GASSERT(false, "Collect is not implemented for class GlobalMap");
    }

    void Combine(BaseVariableObject* other1){
      GlobalNumericMap *other = static_cast<GlobalNumericMap* >(other1);
      typename MAP::iterator oIT;

      for(oIT = other->value_map_.begin(); oIT != other->value_map_.end(); ++oIT){
        value_map_[oIT->first] = (oIT->second);
      }
    }

    void WriteTo(std::ostream& os){
      keytype key;
      valtype value;
      typename MAP::iterator IT;
      os<<value_map_.size();
      for(IT = value_map_.begin(); IT != value_map_.end(); ++IT){
        key = IT->first;
        value = IT->second;

        os.write(reinterpret_cast<char*>(&key), sizeof(keytype));
        os.write(reinterpret_cast<char*>(&value), sizeof(valtype));
      }

    }

    void Clear(){
      value_map_.clear();
    }

    void LoadFrom(std::istream& is){
      keytype key;
      valtype value;
      typename MAP::size_type size;
      is>>size;

      for(typename MAP::size_type i = 0; i<size; i++){
        is.read(reinterpret_cast<char*>(&key), sizeof(keytype));
        is.read(reinterpret_cast<char*>(&value), sizeof(valtype));
         value_map_[key] = value;
      }
    }
  };
}//namespace UDIMPL


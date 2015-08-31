/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPath
 *
 *  Created on: 8/6/2015
 *  Author: Renchu
 *  Description: Variant visitor methods to handle numerical operations
 ******************************************************************************/

#ifndef SRC_CUSTOMER_COREIMPL_VARIANT_NUMERICAL_OPERATORS_HPP_
#define SRC_CUSTOMER_COREIMPL_VARIANT_NUMERICAL_OPERATORS_HPP_

#include <gutil/gsqlcontainer.hpp>
#include "boost/variant.hpp"
#include "gpathTypeDefines.hpp"

namespace UDIMPL {

using namespace GPATH_TOOLS;

class LessThan : public boost::static_visitor<bool> {
public:
  bool operator()(const int& l, const double& r) const {
    return l < r;
  }
  
  bool operator()(const double& l, const int& r) const {
    return l < r;
  }
  
  template <typename T>
  bool operator()(const T& l, const T& r) const {
    return l < r;
  }

  template <typename T, typename U>
  bool operator()(const T& l, const U& r) const {
    return false;
  }
};

class LessEqual : public boost::static_visitor<bool> {
public:
  bool operator()(const int& l, const double& r) const {
    return l <= r;
  }
  
  bool operator()(const double& l, const int& r) const {
    return l <= r;
  }
  
  template <typename T>
  bool operator()(const T& l, const T& r) const {
    return l <= r;
  }

  template <typename T, typename U>
  bool operator()(const T& l, const U& r) const {
    return false;
  }
};

class GreaterThan : public boost::static_visitor<bool> {
public:
  bool operator()(const int& l, const double& r) const {
    return l > r;
  }
  
  bool operator()(const double& l, const int& r) const {
    return l > r;
  }
  
  template <typename T>
  bool operator()(const T& l, const T& r) const {
    return l > r;
  }

  template <typename T, typename U>
  bool operator()(const T& l, const U& r) const {
    return false;
  }
};

class GreaterEqual : public boost::static_visitor<bool> {
public:
  bool operator()(const int& l, const double& r) const {
    return l >= r;
  }
  
  bool operator()(const double& l, const int& r) const {
    return l >= r;
  }
  
  template <typename T>
  bool operator()(const T& l, const T& r) const {
    return l >= r;
  }

  template <typename T, typename U>
  bool operator()(const T& l, const U& r) const {
    return false;
  }
};

class Equal : public boost::static_visitor<bool> {
public:
  bool operator()(const int& l, const double& r) const {
    return l == r;
  }
  
  bool operator()(const double& l, const int& r) const {
    return l == r;
  }
  
  template <typename T>
  bool operator()(const T& l, const T& r) const {
    return l == r;
  }

  template <typename T, typename U>
  bool operator()(const T& l, const U& r) const {
    return false;
  }
};

class NotEqual : public boost::static_visitor<bool> {
public:
  bool operator()(const int& l, const double& r) const {
    return l != r;
  }
  
  bool operator()(const double& l, const int& r) const {
    return l != r;
  }
  
  template <typename T>
  bool operator()(const T& l, const T& r) const {
    return l != r;
  }

  template <typename T, typename U>
  bool operator()(const T& l, const U& r) const {
    return true;
  }
};

class And : public boost::static_visitor<bool> {
public:
  bool operator()(const bool& l, const bool& r) const {
    return l && r;
  }

  template <typename T>
  bool operator()(const T& l, const T& r) const {
    return false;
  }

  template <typename T, typename U>
  bool operator()(const T& l, const U& r) const {
    return false;
  }

};

class Or : public boost::static_visitor<bool> {
public:
  bool operator()(const bool& l, const bool& r) const {
    return l || r;
  }
  
  template <typename T>
  bool operator()(const T& l, const T& r) const {
    return false;
  }

  template <typename T, typename U>
  bool operator()(const T& l, const U& r) const {
    return false;
  }

};

class Not : public boost::static_visitor<bool> {
public:
  bool operator()(const bool& l) const {
    return !l;
  }

  template <typename T>
  bool operator()(const T& l) const {
    return false;
  }
};

class Plus : public boost::static_visitor<VAL> {
public:
  VAL operator()(const int& l, const double& r) const {
    return l + r;
  }

  VAL operator()(const double& l, const int& r) const {
    return l + r;
  }

  VAL operator()(const char*& l, const char*& r) const {
    return false;
  }
  
  template <typename T>
  VAL operator()(const T& l, const T& r) const {
    return l + r;
  }
  
  template <typename T, typename U>
  VAL operator()(const T& l, const U& r) const {
    return false;
  }

};

class Minus : public boost::static_visitor<VAL> {
public:
  VAL operator()(const int& l, const double& r) const {
    return l - r;
  }

  VAL operator()(const int& l, const int& r) const {
    return l - r;
  }

  VAL operator()(const double& l, const int& r) const {
    return l - r;
  }

  VAL operator()(const double& l, const double& r) const {
    return l - r;
  }
  
  template <typename T, typename U>
  VAL operator()(const T& l, const U& r) const {
    return false;
  }
};

class Mul : public boost::static_visitor<VAL> {
public:
  VAL operator()(const int& l, const double& r) const {
    return l * r;
  }

  VAL operator()(const int& l, const int& r) const {
    return l * r;
  }

  VAL operator()(const double& l, const int& r) const {
    return l * r;
  }

  VAL operator()(const double& l, const double& r) const {
    return l * r;
  }
  
  template <typename T, typename U>
  VAL operator()(const T& l, const U& r) const {
    return false;
  }
};

class Div : public boost::static_visitor<VAL> {
public:
  VAL operator()(const int& l, const double& r) const {
    return l / r;
  }

  VAL operator()(const int& l, const int& r) const {
    return l / r;
  }

  VAL operator()(const double& l, const int& r) const {
    return l / r;
  }

  VAL operator()(const double& l, const double& r) const {
    return l / r;
  }
  
  template <typename T, typename U>
  VAL operator()(const T& l, const U& r) const {
    return false;
  }
};

}

#endif


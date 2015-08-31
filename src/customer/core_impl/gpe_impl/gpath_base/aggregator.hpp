
/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: Generic path query, aggregator
 *
 *      Author: Renchu Song
 *      Date: July 21th 2015
 ******************************************************************************/
#include <gutil/gsqlcontainer.hpp>
#include <gpe/serviceimplbase.hpp>
#include "boost/variant.hpp"
#include "variant_numerical_operators.hpp"
#include "gpathTypeDefines.hpp"

#ifndef SRC_CUSTOMER_COREIMPL_GPATH_BASE_AGGREGATOR_HPP_
#define SRC_CUSTOMER_COREIMPL_GPATH_BASE_AGGREGATOR_HPP_


namespace UDIMPL {
  namespace GPATH_TOOLS {
    class Aggregator;
    
    typedef boost::shared_ptr<Aggregator> AGGREGATOR_PTR;
    
    class Aggregator {
    public:
      virtual void reset() = 0;
      virtual void combine(VAL val, int duplicate) = 0;
      virtual void combine(AGGREGATOR_PTR other) = 0;
      virtual VAL reduce() = 0;
      virtual ~Aggregator() = 0;
    };
    
    class CountAggregator : public Aggregator {
    public:
      CountAggregator() { reset(); }
      VAL cnt_;
      void reset();
      void combine(AGGREGATOR_PTR other);
      void combine(VAL val, int duplicate);
      VAL reduce();
      ~CountAggregator();
    };
    
    class SumAggregator : public Aggregator {
    public:
      SumAggregator() { reset(); }
      VAL sum_;
      void reset();
      void combine(AGGREGATOR_PTR other);
      void combine(VAL val, int duplicate);
      VAL reduce();
      ~SumAggregator();
    };
    
    class MaxAggregator : public Aggregator {
    public:
      MaxAggregator() { reset(); }
      VAL max_;
      void reset();
      void combine(AGGREGATOR_PTR other);
      void combine(VAL val, int duplicate);
      VAL reduce();
      ~MaxAggregator();
    };
    
    class MinAggregator : public Aggregator {
    public:
      MinAggregator() { reset(); }
      VAL min_;
      void reset();
      void combine(AGGREGATOR_PTR other);
      void combine(VAL val, int duplicate);
      VAL reduce();
      ~MinAggregator();
    };
    
    class AvgAggregator : public Aggregator {
    public:
      AvgAggregator() { reset(); }
      VAL sum_;
      int cnt_;
      void reset();
      void combine(AGGREGATOR_PTR other);
      void combine(VAL val, int duplicate);
      VAL reduce();
      ~AvgAggregator();
    };
    
    class DistinctAggregator : public Aggregator {
    public:
      DistinctAggregator() { reset(); }
      GUnorderedSet<VAL>::T values_;
      void reset();
      void combine(AGGREGATOR_PTR other);
      void combine(VAL val, int duplicate);
      VAL reduce();
      ~DistinctAggregator();
    };
    
    class AggregatorFactory {
    public:
      static AGGREGATOR_PTR create(std::string aggFunc);
    };
  
  }
}

#endif


#include "aggregator.hpp"

namespace UDIMPL {
  namespace GPATH_TOOLS {
    Aggregator::~Aggregator() { }
  
    void CountAggregator::reset() {
      cnt_ = 0;
    }
  
    void CountAggregator::combine(AGGREGATOR_PTR other) {
      VAL val = (reinterpret_cast<CountAggregator*>(other.get()))->cnt_;
      combine(val, 1);
    }
  
    void CountAggregator::combine(VAL val, int duplicate) {
      VAL dup = duplicate;
      VAL mulVal = boost::apply_visitor(Mul(), val, dup);
      cnt_ = boost::apply_visitor(Plus(), cnt_, mulVal);
    }
  
    VAL CountAggregator::reduce() {
      return cnt_;
    }
    
    CountAggregator::~CountAggregator() { }
  
  
    void SumAggregator::reset() {
      sum_ = 0;
    }
  
    void SumAggregator::combine(AGGREGATOR_PTR other) {
      VAL val = (reinterpret_cast<SumAggregator*>(other.get()))->sum_;
      combine(val, 1);
    }
  
    void SumAggregator::combine(VAL val, int duplicate) {
      VAL dup = duplicate;
      VAL mulVal = boost::apply_visitor(Mul(), val, dup);
      sum_ = boost::apply_visitor(Plus(), sum_, mulVal);
    }
  
    VAL SumAggregator::reduce() {
      return sum_;
    }
  
    SumAggregator::~SumAggregator() { }
  
  
    void MaxAggregator::reset() {
      max_ = -std::numeric_limits<double>::max();
    }
  
    void MaxAggregator::combine(AGGREGATOR_PTR other) {
      VAL val = (reinterpret_cast<MaxAggregator*>(other.get()))->max_;
      combine(val, 1);
    }
  
    void MaxAggregator::combine(VAL val, int duplicate) {
      if (boost::apply_visitor(LessThan(), max_, val)) {
        max_ = val;
      }
    }
  
    VAL MaxAggregator::reduce() {
      return max_;
    }
  
    MaxAggregator::~MaxAggregator() { }
  
  
    void MinAggregator::reset() {
      min_ = std::numeric_limits<double>::max();
    }
  
    void MinAggregator::combine(AGGREGATOR_PTR other) {
      VAL val = (reinterpret_cast<MinAggregator*>(other.get()))->min_;
      combine(val, 1);
    }
  
    void MinAggregator::combine(VAL val, int duplicate) {
      if (boost::apply_visitor(GreaterThan(), min_, val)) {
        min_ = val;
      }
    }
  
    VAL MinAggregator::reduce() {
      return min_;
    }
  
    MinAggregator::~MinAggregator() { }
  
    void AvgAggregator::reset() {
      sum_ = 0;
      cnt_ = 0;
    }
  
    void AvgAggregator::combine(AGGREGATOR_PTR other) {
      AvgAggregator* oPtr = reinterpret_cast<AvgAggregator*>(other.get());
      sum_ = boost::apply_visitor(Plus(), sum_, oPtr->sum_);
      cnt_ += oPtr->cnt_;
    }
  
    void AvgAggregator::combine(VAL val, int duplicate) {
      VAL dup = duplicate;
      VAL mulVal = boost::apply_visitor(Mul(), val, dup);
      sum_ = boost::apply_visitor(Plus(), sum_, mulVal);
      cnt_ += duplicate;
    }
  
    VAL AvgAggregator::reduce() {
      VAL count = cnt_;
      return boost::apply_visitor(Div(), sum_, count);
    }
    
    AvgAggregator::~AvgAggregator() { }

    void DistinctAggregator::reset() {
      values_.clear();
    }
  
    void DistinctAggregator::combine(AGGREGATOR_PTR other) {
      GUnorderedSet<VAL>::T& otherValues = (reinterpret_cast<DistinctAggregator*>(other.get()))->values_;
      values_.insert(otherValues.begin(), otherValues.end());
    }
  
    void DistinctAggregator::combine(VAL val, int duplicate) {
      values_.insert(val);
    }
  
    VAL DistinctAggregator::reduce() {
      return (int)values_.size();
    }
  
    DistinctAggregator::~DistinctAggregator() {
      values_.clear();
    }
  
    AGGREGATOR_PTR AggregatorFactory::create(std::string aggFunc) {
      if (aggFunc == AGG_CNT) {
        return AGGREGATOR_PTR(new CountAggregator());
      }
      else if (aggFunc == AGG_MIN) {
        return AGGREGATOR_PTR(new MinAggregator());
      }
      else if (aggFunc == AGG_MAX) {
        return AGGREGATOR_PTR(new MaxAggregator());
      }
      else if (aggFunc == AGG_SUM) {
        return AGGREGATOR_PTR(new SumAggregator());
      }
      else if (aggFunc == AGG_AVG) {
        return AGGREGATOR_PTR(new AvgAggregator());
      }
      else if (aggFunc == AGG_DISTINCT) {
        return AGGREGATOR_PTR(new DistinctAggregator());
      }
      return AGGREGATOR_PTR((Aggregator*)(NULL));
    }
  }
}


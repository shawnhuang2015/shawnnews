#include "../aggregator.hpp"

using namespace UDIMPL::GPATH_TOOLS;

int main() {
  // Count
  std::cout << "Count" << std::endl;
  AGGREGATOR_PTR count1 = AggregatorFactory::create("Count");
  count1->combine(1, 1);
  count1->combine(1, 2);
  count1->combine(1, 3);
  std::cout << count1->reduce() << std::endl;

  AGGREGATOR_PTR count2 = AggregatorFactory::create("Count");
  count2->combine(1, 2);
  count2->combine(1, 4);
  std::cout << count2->reduce() << std::endl;
  
  count1->combine(count2);
  std::cout << count1->reduce() << std::endl;
  
  // Sum
  std::cout << "Sum" << std::endl;
  AGGREGATOR_PTR sum1 = AggregatorFactory::create("Sum");
  sum1->combine(3.5, 4);
  sum1->combine(4, 2);
  sum1->combine(10.0, 3);
  std::cout << sum1->reduce() << std::endl;

  AGGREGATOR_PTR sum2 = AggregatorFactory::create("Sum");
  sum2->combine(1, 4);
  sum2->combine(1, 1);
  sum2->combine(3.4, 1);
  std::cout << sum2->reduce() << std::endl;
  
  sum1->combine(sum2);
  std::cout << sum1->reduce() << std::endl;
  
  // Max
  std::cout << "Max" << std::endl;
  AGGREGATOR_PTR max1 = AggregatorFactory::create("Max");
  std::cout << max1->reduce() << std::endl;
  max1->combine(3.5, 3);
  max1->combine(4, 2);
  max1->combine(-10.0, 4);
  std::cout << max1->reduce() << std::endl;

  AGGREGATOR_PTR max2 = AggregatorFactory::create("Max");
  max2->combine(-8.6, 5);
  max2->combine(-3.4, 2);
  std::cout << max2->reduce() << std::endl;
  
  max1->combine(max2);
  std::cout << max1->reduce() << std::endl;
  
  // Min
  std::cout << "Min" << std::endl;
  AGGREGATOR_PTR min1 = AggregatorFactory::create("Min");
  std::cout << min1->reduce() << std::endl;
  min1->combine(3.5, 3);
  min1->combine(4, 2);
  min1->combine(-10.0, 3);
  std::cout << min1->reduce() << std::endl;

  AGGREGATOR_PTR min2 = AggregatorFactory::create("Min");
  min2->combine(-8.6, 1);
  min2->combine(-3.4, 1);
  std::cout << min2->reduce() << std::endl;
  
  min1->combine(min2);
  std::cout << min1->reduce() << std::endl;
  
  // Avg
  std::cout << "Avg" << std::endl;
  AGGREGATOR_PTR avg1 = AggregatorFactory::create("Avg");
  avg1->combine(3.5, 8);
  avg1->combine(4, 1);
  avg1->combine(-10.0, 3);
  std::cout << avg1->reduce() << std::endl;

  AGGREGATOR_PTR avg2 = AggregatorFactory::create("Avg");
  avg2->combine(-8.6, 1);
  avg2->combine(-3.4, 2);
  std::cout << avg2->reduce() << std::endl;
  
  avg1->combine(avg2);
  std::cout << avg1->reduce() << std::endl;
  
  // Distinct
  std::cout << "Distinct" << std::endl;
  AGGREGATOR_PTR dist1 = AggregatorFactory::create("Distinct");
  dist1->combine(3, 2);
  dist1->combine(4, 1);
  dist1->combine(-10.0, 3);
  dist1->combine(4, 2);
  dist1->combine(-10.0, 1);
  dist1->combine(4, 1);
  dist1->combine(5, 2);
  std::cout << dist1->reduce() << std::endl;

  AGGREGATOR_PTR dist2 = AggregatorFactory::create("Distinct");
  dist2->combine(-10.0, 2);
  dist2->combine(3, 1);
  dist2->combine(-4, 3);
  dist2->combine(4, 2);
  std::cout << dist2->reduce() << std::endl;
  
  dist1->combine(dist2);
  std::cout << dist1->reduce() << std::endl;
  
  return 0;
}

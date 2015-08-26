#include <iostream>
#include "../ragraph.hpp"

using namespace UDIMPL;

int main() {
  std::string raStr = "{\"anchor_node\":{\"vtypecheck\":\"v1\",\"vname_internal\":\"v1[1]\",\"membership_list\":[\"id1\"],\"vfilter\":\"\",\"v_drop_list\":[],\"v_add_list\":[\"_internal_id\",\"name\",\"age\"]},\"steps\":[{\"etypecheck\":\"e1\",\"efilter\":\"A(e1[1].a,int) C(5,int) O(>)\",\"ename_internal\":\"e1[1]\",\"e_drop_list\":[\"e1[1].a\"],\"e_add_list\":[\"a\",\"b\"],\"vtypecheck\":\"v2\",\"vname_internal\":\"v2[1]\",\"membership_list\":[],\"vfilter\":\"A(e1[1].b) A(v2[1].c) O(!=)\",\"v_drop_list\":[\"e1[1].b\",\"v2[1].c\"],\"v_add_list\":[\"c\",\"_internal_id\"]},{\"etypecheck\":\"e2\",\"efilter\":\"\",\"e_drop_list\":[],\"ename_internal\":\"e2[1]\",\"e_add_list\":[],\"vtypecheck\":\"v3\",\"vfilter\":\"\",\"vname_internal\":\"v3[1]\",\"membership_list\":[],\"v_drop_list\":[],\"v_add_list\":[\"age\",\"phone\",\"_internal_id\"]}],\"ra\":[{\"return_name\":\"B\",\"output\":true,\"input_name\":\"A\",\"ops\":[{\"ra_type\":\"union\",\"also_depends_on\":\".\",\"ra_options\":{\"argument\":\".\"}}]},{\"return_name\":\"A\",\"output\":true,\"input_name\":\".\",\"ops\":[{\"ra_type\":\"select\",\"ra_options\":{\"filter\":\"A(v3[1].age,int) C(5,int) O(<)\"}},{\"ra_type\":\"project\",\"ra_options\":{\"projection_expressions\":[\"A(v1[1].name,string)\",\"A(v1[1].age,int)\",\"A(v3[1].phone,int)\"],\"projection_names\":[\"v1[1].name\",\"v1[1].age\",\"v3[1].phone\"]}},{\"ra_type\":\"group\",\"ra_options\":{\"projection_expressions\":[\"A(v1[1].name,string)\",\"A(v1[1].age,int)\"],\"projection_names\":[\"v1[1].name\",\"v1[1].age\"],\"aggregation\":{\"function\":\"Sum\",\"projection_expression\":\"A(v1[1].age,int)\",\"projection_name\":\"Sum\"}}}]}]}";
  Json::Reader reader;
  Json::Value raValue;
  reader.parse(raStr, raValue);
  GPATH_TOOLS::RAGraph graph(raValue);
  std::cout << graph << std::endl;
  return 0;
}


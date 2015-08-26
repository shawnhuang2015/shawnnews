#include <iostream>
#include "../column_meta.hpp"
#include <gutil/gsqlcontainer.hpp>
#include <gpe/serviceimplbase.hpp>

using namespace UDIMPL::GPATH_TOOLS;

int main() {
  GVector<std::string>::T a;
  a.push_back("A1");
  a.push_back("A2");
  a.push_back("A3");
  a.push_back("A4");

  GVector<std::string>::T b;
  b.push_back("B1");
  b.push_back("B2");
  
  ColumnMeta cMeta;
  cMeta.addDAGLayer(a);
  cMeta.addDAGLayer(b);
  
  std::cout << cMeta << std::endl;
  uint32_t s, t;
  std::string str = "A3";
  cMeta.getAttrIndex(str, s);
  std::cout << "A3 " << s << std::endl;
  str = "B1";
  cMeta.getAttrIndex(str, s);
  std::cout << "B1 " << s << std::endl;
  str = "A2";
  cMeta.getAttrLayerAndBias(str, s, t);
  std::cout << "A2 " << s << " " << t << std::endl;
  str = "B2";
  cMeta.getAttrLayerAndBias(str, s, t);
  std::cout << "B2 " << s << " " << t << std::endl;
  
  std::string raStr = "{\"anchor_node\":{\"vtypecheck\":\"v1\",\"vname_internal\":\"v1[1]\",\"membership_list\":[\"id1\"],\"vfilter\":\"\",\"v_drop_list\":[],\"v_add_list\":[\"_internal_id\",\"name\",\"age\"]},\"steps\":[{\"etypecheck\":\"e1\",\"efilter\":\"A(e1[1].a,int)C(5,int)O(>)\",\"ename_internal\":\"e1[1]\",\"e_drop_list\":[\"e1[1].a\"],\"e_add_list\":[\"a\",\"b\"],\"vtypecheck\":\"v2\",\"vname_internal\":\"v2[1]\",\"membership_list\":[],\"vfilter\":\"A(e1[1].b)A(v2[1].c)O(!=)\",\"v_drop_list\":[\"e1[1].b\",\"v2[1].c\"],\"v_add_list\":[\"c\",\"_internal_id\"]},{\"etypecheck\":\"e2\",\"efilter\":\"\",\"e_drop_list\":[],\"ename_internal\":\"e2[1]\",\"e_add_list\":[],\"vtypecheck\":\"v3\",\"vfilter\":\"\",\"vname_internal\":\"v3[1]\",\"membership_list\":[],\"v_drop_list\":[],\"v_add_list\":[\"age\",\"phone\",\"_internal_id\"]}],\"ra\":[{\"return_name\":\"B\",\"output\":true,\"input_name\":\"A\",\"ops\":[{\"ra_type\":\"union\",\"also_depends_on\":\".\",\"ra_options\":{\"argument\":\".\"}}]},{\"return_name\":\"A\",\"output\":true,\"input_name\":\".\",\"ops\":[{\"ra_type\":\"select\",\"ra_options\":{\"filter\":\"A(v3[1].age,int)C(5,int)O(<)\"}},{\"ra_type\":\"project\",\"ra_options\":{\"projection_expressions\":[\"A(v1[1].name,string)\",\"A(v1[1].age,int)\",\"A(v3[1].phone,int)\"],\"projection_names\":[\"v1[1].name\",\"v1[1].age\",\"v3[1].phone\"]}},{\"ra_type\":\"group\",\"ra_options\":{\"projection_expressions\":[\"A(v1[1].name,string)\",\"A(v1[1].age,int)\"],\"projection_names\":[\"v1[1].name\",\"v1[1].age\"],\"aggregation\":{\"function\":\"Sum\",\"projection_expression\":\"A(v1[1].age,int)\",\"projection_name\":\"Sum\"}}}]}]}";
  Json::Reader reader;
  Json::Value raValue;
  reader.parse(raStr, raValue);
  ColumnMeta cMeta2;
  cMeta2.buildBasicCtMeta(raValue);
  std::cout << cMeta2 << std::endl;
  
  return 0;  
}

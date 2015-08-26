#include <iostream>
#include <gutil/gsqlcontainer.hpp>
#include <gpe/serviceimplbase.hpp>
#include "../context_table.hpp"
#include "../expression.hpp"
#include "../dag_traversal.hpp"
#include "../column_meta.hpp"
#include "../operations.hpp"

using namespace UDIMPL::GPATH_TOOLS;

// Write context table, with RANode
void writeContextTable(CONTEXT_TABLE_PTR table, RANode* raNode) {
  ColumnMeta& columnMeta = raNode->newColumnMeta_;
  
  //write the name of contextTable
  std::string& tableName = raNode->nodeName_;

  std::cout << "Table " << tableName << std::endl;
  
  //write column names
  for (uint32_t k = 0; k < columnMeta.indexToInternalName.size(); ++k) {
    std::cout << columnMeta.indexToInternalName[k] << ",";
  }
  std::cout << std::endl;
  //write rows of contextTable
  DAGTraversal traversor(table);
  VECTOR_ANY row(columnMeta.colNumber);   // Have an initial size as the whole context table
  while (traversor.next(columnMeta, row)) {
    uint32_t dupNumber = traversor.getDuplicatedNum();
    for (uint32_t i = 0; i < dupNumber; ++i) {
      for (uint32_t lp = 0; lp < row.size(); ++lp) {
        std::cout << row[lp] << ",";
      }
      std::cout << std::endl;
    }
  }
}

int main() {
  std::string jsonStr = "{\"anchor_node\":{\"vtypecheck\":\"v1\",\"vname_internal\":\"v1[1]\",\"vfilter\":\"\",\"v_drop_list\":[],\"v_add_list\":[\"a\",\"b\"]},\"steps\":[{\"etypecheck\":\"e1\",\"efilter\":\"\",\"ename_internal\":\"e1[1]\",\"e_drop_list\":[],\"e_add_list\":[\"c\",\"d\"],\"vtypecheck\":\"v2\",\"vname_internal\":\"v2[1]\",\"membership_list\":[],\"vfilter\":\"\",\"v_drop_list\":[],\"v_add_list\":[\"e\"]},{\"etypecheck\":\"e2\",\"efilter\":\"\",\"e_drop_list\":[],\"ename_internal\":\"e2[1]\",\"e_add_list\":[],\"vtypecheck\":\"v3\",\"vfilter\":\"\",\"vname_internal\":\"v3[1]\",\"membership_list\":[],\"v_drop_list\":[],\"v_add_list\":[\"f\",\"g\"]}],\"ra\":[{\"return_name\":\"A\",\"output\":true,\"input_name\":\".\",\"ops\":[{\"ra_type\":\"select\",\"ra_options\":{\"filter\":\"A(v2[1].e) A(v3[1].g) O(==)\"}}]},{\"return_name\":\"B\",\"output\":true,\"input_name\":\".\",\"ops\":[{\"ra_type\":\"project\",\"ra_options\":{\"projection_expressions\":[\"A(v1[1].b)\",\"A(e1[1].c)\",\"A(e1[1].d)\",\"A(v2[1].e)\",\"A(v3[1].g)\"],\"projection_names\":[\"b\",\"c\",\"d\",\"e\",\"g\"]}}]},{\"return_name\":\"C\",\"output\":true,\"input_name\":\".\",\"ops\":[{\"ra_type\":\"group\",\"ra_options\":{\"projection_expressions\":[\"A(e1[1].d)\",\"A(v2[1].e)\"],\"projection_names\":[\"d\",\"e\"],\"aggregation\":{\"function\":\"Sum\",\"projection_expression\":\"A(v1[1].b)\",\"projection_name\":\"Sum_b\"}}}]},{\"return_name\":\"D\",\"output\":true,\"input_name\":\".\",\"ops\":[{\"ra_type\":\"union\",\"also_depends_on\":\".\",\"ra_options\":{\"argument\":\".\"}}]}]}";
  
  Json::Reader reader;
  Json::Value raValue;
  reader.parse(jsonStr, raValue);
  RAGraph graph(raValue);
  
  VECTOR_ANY val12;     // 1, 2
  val12.push_back(1);
  val12.push_back(2);
  VECTOR_ANY val21;     // 2, 1
  val21.push_back(2);
  val21.push_back(1);
  VECTOR_ANY val1;      // 1
  val1.push_back(1);
  VECTOR_ANY val2;      // 2
  val2.push_back(2);
  
  // Layer 0
  CONTEXT_TABLE_PTR v11(new ContextTable(CONTEXT_TABLE_NULL_PTR));
  CONTEXT_TABLE_PTR v12(new ContextTable(CONTEXT_TABLE_NULL_PTR));
  v11->addColumns(val12);
  v12->addColumns(val21);
  // Layer 1
  CONTEXT_TABLE_PTR e11(new ContextTable(1, v11));
  CONTEXT_TABLE_PTR e12(new ContextTable(1, v11));
  CONTEXT_TABLE_PTR e13(new ContextTable(1, v11));
  CONTEXT_TABLE_PTR e14(new ContextTable(1, v11));
  CONTEXT_TABLE_PTR e15(new ContextTable(1, v12));
  CONTEXT_TABLE_PTR e16(new ContextTable(1, v12));
  CONTEXT_TABLE_PTR e17(new ContextTable(1, v12));
  CONTEXT_TABLE_PTR e18(new ContextTable(1, v12));
  e11->addColumns(val12);
  e12->addColumns(val21);
  e13->addColumns(val12);
  e14->addColumns(val21);
  e15->addColumns(val12);
  e16->addColumns(val21);
  e17->addColumns(val12);
  e18->addColumns(val21);
  // Layer 2
  ContextTableMessage msg1(e11);
  ContextTableMessage msg2(e12);
  ContextTableMessage msg3(e13);
  ContextTableMessage msg4(e14);
  ContextTableMessage msg5(e15);
  ContextTableMessage msg6(e16);
  ContextTableMessage msg7(e17);
  ContextTableMessage msg8(e18);
  msg1 += msg2;
  msg1 += msg5;
  msg1 += msg6;
  msg3 += msg4;
  msg3 += msg7;
  msg3 += msg8;
  CONTEXT_TABLE_PTR v21(new ContextTable(2, msg1.parents_));
  CONTEXT_TABLE_PTR v22(new ContextTable(2, msg3.parents_));
  v21->addColumns(val1);
  v22->addColumns(val2);
  // Layer 3
  CONTEXT_TABLE_PTR e21(new ContextTable(3, v21));
  CONTEXT_TABLE_PTR e22(new ContextTable(3, v21));
  CONTEXT_TABLE_PTR e23(new ContextTable(3, v22));
  CONTEXT_TABLE_PTR e24(new ContextTable(3, v22));
  // Layer 4
  ContextTableMessage msg9(e21);
  ContextTableMessage msg10(e22);
  ContextTableMessage msg11(e23);
  ContextTableMessage msg12(e24);
  msg9 += msg11;
  msg10 += msg12;
  CONTEXT_TABLE_PTR v31(new ContextTable(4, msg9.parents_));
  CONTEXT_TABLE_PTR v32(new ContextTable(4, msg10.parents_));
  v31->addColumns(val12);
  v32->addColumns(val21);

  //std::cout << *v11 << *v12 << *e11 << *e12 << *e13 << *e14 << *e15 << *e16 << *e17 << *e18 << *v21 << *v22 << *e21 << *e22 << *e23 << *e24 << *v31 << *v32 << std::endl;
  
  // The context table "."
  std::string dotTable = ".";
  writeContextTable(v31, graph.getRANodeByName(dotTable));
  writeContextTable(v32, graph.getRANodeByName(dotTable));
  
  /*CONTEXT_TABLE_PTR dot1(new ContextTable(5, v31));
  CONTEXT_TABLE_PTR dot2(new ContextTable(5, v32));
  (*dot2) += *dot1;
  std::cout << *dot1 << std::endl;
  std::cout << *dot2 << std::endl;
  writeContextTable(dot2, graph.getRANodeByName(dotTable));
  */
  std::cout << "DoNothing" << std::endl;
  OPERATIONS_POINTER op_v31_none = Operations::make_operations(graph.raNodes_[0]);
  op_v31_none->push(v31);
  CONTEXT_TABLE_PTR v31_none = op_v31_none->execReduce();
  writeContextTable(v31_none, graph.raNodes_[0]);
  OPERATIONS_POINTER op_v32_none = Operations::make_operations(graph.raNodes_[0]);
  op_v32_none->push(v32);
  CONTEXT_TABLE_PTR v32_none = op_v32_none->execReduce();
  writeContextTable(v32_none, graph.raNodes_[0]);
  CONTEXT_TABLE_PTR v31_none_wd = op_v31_none->prepareForGlobalMap(v31_none);
  CONTEXT_TABLE_PTR v32_none_wd = op_v32_none->prepareForGlobalMap(v32_none);
  (*v31_none_wd) += (*v32_none_wd);
  writeContextTable(v31_none_wd, graph.raNodes_[0]);
  
  std::cout << "DoSelect" << std::endl;
  OPERATIONS_POINTER op_v31_sel = Operations::make_operations(graph.raNodes_[1]);
  op_v31_sel->push(v31);
  CONTEXT_TABLE_PTR v31_sel = op_v31_sel->execReduce();
  writeContextTable(v31_sel, graph.raNodes_[1]);
  OPERATIONS_POINTER op_v32_sel = Operations::make_operations(graph.raNodes_[1]);
  op_v32_sel->push(v32);
  CONTEXT_TABLE_PTR v32_sel = op_v32_sel->execReduce();
  writeContextTable(v32_sel, graph.raNodes_[1]);
  CONTEXT_TABLE_PTR v31_sel_wd = op_v31_sel->prepareForGlobalMap(v31_sel);
  CONTEXT_TABLE_PTR v32_sel_wd = op_v32_sel->prepareForGlobalMap(v32_sel);
  (*v31_sel_wd) += (*v32_sel_wd);
  writeContextTable(v31_sel_wd, graph.raNodes_[1]);
  
  std::cout << "DoProject" << std::endl;
  OPERATIONS_POINTER op_v31_proj = Operations::make_operations(graph.raNodes_[2]);
  op_v31_proj->push(v31);
  CONTEXT_TABLE_PTR v31_proj = op_v31_proj->execReduce();
  writeContextTable(v31_proj, graph.raNodes_[2]);
  OPERATIONS_POINTER op_v32_proj = Operations::make_operations(graph.raNodes_[2]);
  op_v32_proj->push(v32);
  CONTEXT_TABLE_PTR v32_proj = op_v32_proj->execReduce();
  writeContextTable(v32_proj, graph.raNodes_[2]);
  CONTEXT_TABLE_PTR v31_proj_wd = op_v31_proj->prepareForGlobalMap(v31_proj);
  CONTEXT_TABLE_PTR v32_proj_wd = op_v32_proj->prepareForGlobalMap(v32_proj);
  (*v31_proj_wd) += (*v32_proj_wd);
  writeContextTable(v31_proj_wd, graph.raNodes_[2]);

  std::cout << "DoUnion" << std::endl;
  OPERATIONS_POINTER op_v31_uni = Operations::make_operations(graph.raNodes_[4]);
  op_v31_uni->push(v31);
  op_v31_uni->push(v32);
  CONTEXT_TABLE_PTR v31_uni = op_v31_uni->execReduce();
  writeContextTable(v31_uni, graph.raNodes_[4]);
  OPERATIONS_POINTER op_v32_uni = Operations::make_operations(graph.raNodes_[4]);
  op_v32_uni->push(v32);
  op_v32_uni->push(v31_uni);
  CONTEXT_TABLE_PTR v32_uni = op_v32_uni->execReduce();
  writeContextTable(v32_uni, graph.raNodes_[4]);
  CONTEXT_TABLE_PTR v31_uni_wd = op_v31_uni->prepareForGlobalMap(v31_uni);
  CONTEXT_TABLE_PTR v32_uni_wd = op_v32_uni->prepareForGlobalMap(v32_uni);
  (*v31_uni_wd) += (*v32_uni_wd);
  writeContextTable(v31_uni_wd, graph.raNodes_[4]);

  std::cout << "DoGroup" << std::endl;
  OPERATIONS_POINTER op_v31_grp = Operations::make_operations(graph.raNodes_[3]);
  op_v31_grp->push(v31);
  CONTEXT_TABLE_PTR v31_grp = op_v31_grp->execReduce();
  std::cout << (GroupContextTable&)(*v31_grp) << std::endl;
  OPERATIONS_POINTER op_v32_grp = Operations::make_operations(graph.raNodes_[3]);
  op_v32_grp->push(v32);
  CONTEXT_TABLE_PTR v32_grp = op_v32_grp->execReduce();
  std::cout << (GroupContextTable&)(*v32_grp) << std::endl;
  CONTEXT_TABLE_PTR v31_grp_wd = op_v31_grp->prepareForGlobalMap(v31_grp);
  CONTEXT_TABLE_PTR v32_grp_wd = op_v32_grp->prepareForGlobalMap(v32_grp);
  (*v31_grp) += (*v32_grp);
  std::cout << (GroupContextTable&)(*v31_grp) << std::endl;
  v31_grp->getContextTable();
  std::cout << *v31_grp << std::endl;
  writeContextTable(v31_grp, graph.raNodes_[3]);

  return 0;
}

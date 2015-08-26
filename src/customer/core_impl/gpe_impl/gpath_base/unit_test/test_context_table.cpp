#include <iostream>
#include "../context_table.hpp"
#include "../expression.hpp"
#include "../dag_traversal.hpp"
#include "../column_meta.hpp"
#include <gutil/gsqlcontainer.hpp>
#include <gpe/serviceimplbase.hpp>

using namespace UDIMPL::GPATH_TOOLS;

int main() {
  CONTEXT_TABLE_PTR nulPtr((ContextTable*)NULL);

  CONTEXT_TABLE_PTR pt1 = CONTEXT_TABLE_PTR(new ContextTable(CONTEXT_TABLE_NULL_PTR));
  pt1->attr.push_back("v10");
  CONTEXT_TABLE_PTR pt2 = CONTEXT_TABLE_PTR(new ContextTable(CONTEXT_TABLE_NULL_PTR));
  pt2->attr.push_back("v11");
  CONTEXT_TABLE_PTR pt3 = CONTEXT_TABLE_PTR(new ContextTable(CONTEXT_TABLE_NULL_PTR));
  pt3->attr.push_back("v12");
  CONTEXT_TABLE_PTR pt4 = CONTEXT_TABLE_PTR(new ContextTable(CONTEXT_TABLE_NULL_PTR));
  pt4->attr.push_back("v13");
  CONTEXT_TABLE_PTR pt5 = CONTEXT_TABLE_PTR(new ContextTable(CONTEXT_TABLE_NULL_PTR));
  pt5->attr.push_back("v14");

  std::cout << "layer0" << std::endl;
  std::cout << *pt1 << std::endl;
  std::cout << *pt2 << std::endl;
  std::cout << *pt3 << std::endl;
  std::cout << *pt4 << std::endl;
  std::cout << *pt5 << std::endl;
  
  ContextTableMessage msg1 = ContextTableMessage(pt1);
  ContextTableMessage msg2 = ContextTableMessage(pt2);
  ContextTableMessage msg3 = ContextTableMessage(pt3);
  ContextTableMessage msg4 = ContextTableMessage(pt4);
  ContextTableMessage msg5 = ContextTableMessage(pt5);
  
  std::cout << "layer0 to layer1 message" << std::endl;
  std::cout << msg1 << std::endl;
  std::cout << msg2 << std::endl;
  std::cout << msg3 << std::endl;
  std::cout << msg4 << std::endl;
  std::cout << msg5 << std::endl;

  ContextTableMessage msg6 = msg1;
  msg6 += msg2;
  msg6 += msg3;
  ContextTableMessage msg7 = msg2;
  msg7 += msg3;
  msg7 += msg4;
  msg7 += msg5;
  CONTEXT_TABLE_PTR pt6 = CONTEXT_TABLE_PTR(new ContextTable(1, msg6.parents_));
  pt6->attr.push_back("v20");
  CONTEXT_TABLE_PTR pt7 = CONTEXT_TABLE_PTR(new ContextTable(1, msg7.parents_));
  pt7->attr.push_back("v21");

  std::cout << "layer1" << std::endl;
  std::cout << *pt6 << std::endl;
  std::cout << *pt7 << std::endl;

  ContextTableMessage msg8 = ContextTableMessage(pt6);
  msg8 += ContextTableMessage(pt7);
  CONTEXT_TABLE_PTR pt8 = CONTEXT_TABLE_PTR(new ContextTable(2, msg8.parents_));
  pt8->attr.push_back("v30");

  std::cout << "layer2" << std::endl;
  std::cout << *pt8 << std::endl;

  pt6->isSkipped=true;
  pt7->isSkipped=true;
  ContextTableMessage msg9 = ContextTableMessage(pt6);
  msg9 += ContextTableMessage(pt7);
  CONTEXT_TABLE_PTR pt9 = CONTEXT_TABLE_PTR(new ContextTable(2, msg9.parents_));
  pt9->attr.push_back("v30_s");

  std::cout << "layer2 skip layer1" << std::endl;
  std::cout << *pt9 << std::endl;
  std::cout << "\nStarting tests on traversal now!\n";

//Start testing on the traversal of context_table
  GVector<std::string>::T layer0;
  layer0.push_back("v10");
  layer0.push_back("v11");
  GVector<std::string>::T layer1;
  layer1.push_back("e10");
  layer1.push_back("e11");
  GVector<std::string>::T layer2;
  layer2.push_back("v20");
  layer2.push_back("v21");
  GVector<std::string>::T layer3;
  layer3.push_back("e20");
  layer3.push_back("e21");
  GVector<std::string>::T layer4;
  layer4.push_back("v30");
  layer4.push_back("v31");
  GVector<std::string>::T layer5;
  layer5.push_back("v40");
  layer5.push_back("v41");
  ColumnMeta cMeta;
  cMeta.addDAGLayer(layer0);
  cMeta.addDAGLayer(layer1);
  cMeta.addDAGLayer(layer2);
  cMeta.addDAGLayer(layer3);
  cMeta.addDAGLayer(layer4);
  cMeta.addDAGLayer(layer5);
  //build the graph  
  std::cout << "The layer 0, vertex,  is:\n";
  CONTEXT_TABLE_PTR pt01 = CONTEXT_TABLE_PTR(new ContextTable(CONTEXT_TABLE_NULL_PTR));
  pt01->attr.push_back("v10");
  pt01->attr.push_back(10);
  pt01->dupNumFromParents[0] = 10;
  std::cout << *pt01 << std::endl;
  CONTEXT_TABLE_PTR pt02 = CONTEXT_TABLE_PTR(new ContextTable(CONTEXT_TABLE_NULL_PTR));
  pt02->dupNumFromParents[0] = 20;
  pt02->attr.push_back("v11");
  pt02->attr.push_back(11);
  std::cout << *pt02 << std::endl;
  CONTEXT_TABLE_PTR pt03 = CONTEXT_TABLE_PTR(new ContextTable(CONTEXT_TABLE_NULL_PTR));
  pt03->dupNumFromParents[0] = 30;
  pt03->attr.push_back("v12");
  pt03->attr.push_back(11);
  std::cout << *pt03 << std::endl;
  CONTEXT_TABLE_PTR pt04 = CONTEXT_TABLE_PTR(new ContextTable(CONTEXT_TABLE_NULL_PTR));
  pt04->dupNumFromParents[0] = 40;
  pt04->attr.push_back("v13");
  pt04->attr.push_back(8);
  std::cout << *pt04 << std::endl;
  //layer 1
  std::cout << "The layer 1, edge,  is:\n";
  CONTEXT_TABLE_PTR pt11 = CONTEXT_TABLE_PTR(new ContextTable(1, pt01));
  pt11->attr.push_back(std::string("e11"));
  pt11->attr.push_back(1);
  std::cout << *pt11 << std::endl;
  CONTEXT_TABLE_PTR pt12 = CONTEXT_TABLE_PTR(new ContextTable(1, pt02));
  pt12->attr.push_back(std::string("e12"));
  pt12->attr.push_back(1);
  std::cout << *pt12 << std::endl;
  CONTEXT_TABLE_PTR pt13 = CONTEXT_TABLE_PTR(new ContextTable(1, pt03));
  pt13->attr.push_back(std::string("e13"));
  pt13->attr.push_back(1);
  std::cout << *pt13 << std::endl;
  CONTEXT_TABLE_PTR pt14 = CONTEXT_TABLE_PTR(new ContextTable(1, pt04));
  pt14->attr.push_back(std::string("e14"));
  pt14->attr.push_back(1);
  std::cout << *pt14 << std::endl;
  
  ContextTableMessage msg11 = ContextTableMessage(pt11);
  ContextTableMessage msg12 = ContextTableMessage(pt12);
  ContextTableMessage msg13 = ContextTableMessage(pt13);
  ContextTableMessage msg14 = ContextTableMessage(pt14);
  
  std::cout << "layer1 to layer2 message" << std::endl;
  std::cout << msg11 << std::endl;
  std::cout << msg12 << std::endl;
  std::cout << msg13 << std::endl;
  std::cout << msg14 << std::endl;

  std::cout << "After combine,  the layer1 to layer2 message\n" << std::endl;
  ContextTableMessage msg21 = msg11;
  msg21 += msg12;
  std::cout << msg21 << std::endl;
  ContextTableMessage msg22 = msg13;
  msg22 += msg14;
  std::cout << msg22 << std::endl;
  //layer 2
  std::cout << "The layer 2, vertex, is:\n";
  CONTEXT_TABLE_PTR pt21 = CONTEXT_TABLE_PTR(new ContextTable(2, msg21.parents_));
  pt21->dupNumFromParents[0] = 3;
  pt21->attr.push_back(std::string("v21"));
  pt21->attr.push_back(20);
  //pt21->validPaths.clear();
  std::cout << *pt21 << std::endl;
  CONTEXT_TABLE_PTR pt22 = CONTEXT_TABLE_PTR(new ContextTable(2, msg22.parents_));
  pt22->dupNumFromParents[1] = 5;
  pt22->attr.push_back(std::string("v22"));
  pt22->attr.push_back(21);
  pt22->validPaths.clear();
  pt22->validPaths.push_back(1);
  std::cout << *pt22 << std::endl;
  //layer 3
  std::cout << "The layer 3, edge,  is:\n";
  CONTEXT_TABLE_PTR pt31 = CONTEXT_TABLE_PTR(new ContextTable(3, pt21));
  pt31->attr.push_back(std::string("e21"));
  pt31->attr.push_back(2);
  std::cout << *pt31 << std::endl;
  CONTEXT_TABLE_PTR pt32 = CONTEXT_TABLE_PTR(new ContextTable(3, pt22));
  pt32->attr.push_back(std::string("e22"));
  pt32->attr.push_back(1);
  std::cout << *pt32 << std::endl;
  //layer 4
  ContextTableMessage msg31 = ContextTableMessage(pt31);
  ContextTableMessage msg32 = ContextTableMessage(pt32);
  std::cout << "layer3 to layer4 message, before combination\n" << std::endl;
  std::cout << msg31 << std::endl;
  std::cout << msg32 << std::endl;
  std::cout << "After combine,  the layer3 to layer4 message\n" << std::endl;
  ContextTableMessage msg312 = msg31;
  msg312 += msg32;
  std::cout << msg312 << std::endl;

  std::cout << "The layer 4, vertex, is:\n";
  CONTEXT_TABLE_PTR pt41 = CONTEXT_TABLE_PTR(new ContextTable(4, msg312.parents_));
  pt41->attr.push_back(std::string("v3"));
  pt41->attr.push_back(11);
  std::cout << *pt41 << std::endl;

  std::cout << "The layer 5, vertex, is:\n";
  ContextTableMessage msg51 = ContextTableMessage(pt41);
  CONTEXT_TABLE_PTR pt51 = CONTEXT_TABLE_PTR(new ContextTable(5, msg51.parents_));
  VECTOR_ANY attr51;
  attr51.push_back(std::string("v4"));
  attr51.push_back(51);
  pt51->attr = attr51;
  std::cout << *pt51 << std::endl;


  std::string filter_v3 = "A(v11) A(v31) O(==)";
  GVector<Expression>::T expressions;
  expressions.push_back(Expression(filter_v3, cMeta));
  std::string v31 = "A(v31)";
  std::string v21 = "A(v21)";
  std::string v10 = "A(v11)";
  expressions.push_back(Expression(v31, cMeta));
  expressions.push_back(Expression(v21, cMeta));
  expressions.push_back(Expression(v10, cMeta));

  std::cout << "\nTesting the contextTable constructor\n";
  std::cout << "This is for testing the traversal and filter\n";
  StackNode sNode(pt51, pt51->validPaths);  
  DAGTraversal traversal; 
  uint32_t minLayerIndex = std::numeric_limits<uint32_t>::max();
  for (uint32_t i = 0; i < expressions.size(); ++i) {
    if (expressions[i].minLayerIndex_ < minLayerIndex) {
      minLayerIndex = expressions[i].minLayerIndex_;
    }
  }
  traversal.minLayerIndex_ = minLayerIndex;
  traversal.pushValidParentsIntoStack(sNode);
  uint32_t jump = 0;
  uint32_t j = 0;
  while(traversal.next(expressions, jump)){
    std::cout << "In the " << j << "th next() call\n";
    if (expressions[0].pass()) {
      std::cout << "it passes the filter with jump = " << jump << " and the traversal obtains the following values:\n";
      for (int i = expressions.size() - 1; i >= 1; --i) {
        std::cout << expressions[i].evaluate() << ",";
      }
      std::cout << "\nduplicated number of this traversal is: " << traversal.getDuplicatedNum();
      std::cout << std::endl;
    } else {
      std::cout << "it does not pass the filter with jump = " << jump << " and the traversal obtains the following values:"<< std::endl;
      for (int i = expressions.size() - 1; i >= 1; --i) {
        std::cout << expressions[i].evaluate() << ",";
      }
      std::cout << "\nduplicated number of this traversal is: " << traversal.getDuplicatedNum();
      std::cout << std::endl;
    }
    j++;
  }
 
  //construct pt5 from pt41
  std::cout << "\nTesting the contextTable constructor from ptr and using the same filter\n";
  std::cout << "This is for testing constructor with pointer and filer\n";
  CONTEXT_TABLE_PTR pt5Frompt4(new ContextTable(5, pt41, expressions, attr51));
  std::cout << *pt5Frompt4 << std::endl;
  DAGTraversal traversal1(pt5Frompt4);
  VECTOR_ANY attr(cMeta.colNumber, 0);
  uint32_t j1 = 0; 
  while(traversal1.next(cMeta, attr)){
    std::cout << "In the " << j1 << "th next() call visited the following vertices: \n";
    for (int i = 0; i < attr.size(); ++i) {
      std::cout << attr[i] << ", ";
    }
    std::cout << "\nduplicated number of this traversal is: " << traversal1.getDuplicatedNum();
    std::cout<<std::endl;
    j1++;
  }

  std::string filter_v21 = "A(v21) C(21,int) O(<)";
  expressions[0] = Expression(filter_v21, cMeta);
  
  //construct pt5 from pt41 using filter filter_v21
  std::cout << "\nTesting the contextTable constructor from message and using new filter v21 < 21\n";
  std::cout << "This is for testing constructor with map and filer\n";
  
  CONTEXT_TABLE_PTR pt5_v21(new ContextTable(5, msg51.parents_, expressions, attr51));
  std::cout << *pt5_v21 << std::endl;
  DAGTraversal traversal2(pt5_v21);
  j1 = 0; 
  while(traversal2.next(cMeta, attr)){
    std::cout << "In the " << j1 << "th next() call visited the following vertices: \n";
    for (int i = 0; i < attr.size(); ++i) {
      std::cout << attr[i] << ", ";
    }
    std::cout << "\nduplicated number of this traversal is: " << traversal2.getDuplicatedNum();
    std::cout<<std::endl;
    j1++;
  }
  //testing message with skipped contextTable, set v21 as skipped
  std::cout << "\nTesting how the message handle skipped contextTable, v21 is set as skipped\n";
  //layer 3 again
  std::cout << "The layer 3, edge,  is:\n";
  pt21->isSkipped = true;//set pt21 as skipped
  pt22->isSkipped = true;//set pt22 as skipped
  CONTEXT_TABLE_PTR pt31_s = CONTEXT_TABLE_PTR(new ContextTable(3, pt21));
  pt31_s->attr.push_back(std::string("e21"));
  pt31_s->attr.push_back(2);
  std::cout << *pt31_s << std::endl;
  CONTEXT_TABLE_PTR pt32_s = CONTEXT_TABLE_PTR(new ContextTable(3, pt22));
  pt32_s->attr.push_back(std::string("e22"));
  pt32_s->attr.push_back(1);
  std::cout << *pt32_s << std::endl;
  //layer 4
  ContextTableMessage msg31_s = ContextTableMessage(pt31_s);
  ContextTableMessage msg32_s = ContextTableMessage(pt32_s);
  std::cout << "layer3_s to layer4_s message, before combination\n" << std::endl;
  std::cout << msg31_s << std::endl;
  std::cout << msg32_s << std::endl;
  std::cout << "After combine,  the layer3_s to layer4_s message\n" << std::endl;
  ContextTableMessage msg312_s = msg31_s;
  msg312_s += msg32_s;
  std::cout << msg312_s << std::endl;

  std::cout << "The layer 4_s, vertex, is:\n";
  CONTEXT_TABLE_PTR pt41_s = CONTEXT_TABLE_PTR(new ContextTable(4, msg312_s.parents_));
  pt41_s->attr.push_back(std::string("v3"));
  pt41_s->attr.push_back(11);
  pt41_s->setSkipped();//skip v3
  std::cout << *pt41_s << std::endl;

  std::cout << "The layer 5_s, vertex, is:\n";
  ContextTableMessage msg51_s = ContextTableMessage(pt41_s);
  std::cout << "msg51_s is :\n";
  std::cout << msg51_s << std::endl;
  CONTEXT_TABLE_PTR pt51_s = CONTEXT_TABLE_PTR(new ContextTable(5, msg51_s.parents_));
  VECTOR_ANY attr51_s;
  attr51_s.push_back(std::string("v4"));
  attr51_s.push_back(51);
  pt51_s->attr = attr51_s;
  std::cout << *pt51_s << std::endl;
//cMeta must be changed for the skipped cases
  GVector<std::string>::T layer0_s;
  layer0_s.push_back("v10");
  layer0_s.push_back("v11");
  GVector<std::string>::T layer1_s;
  layer1_s.push_back("e10");
  layer1_s.push_back("e11");
  GVector<std::string>::T layer2_s;
  GVector<std::string>::T layer3_s;
  layer3_s.push_back("e20");
  layer3_s.push_back("e21");
  GVector<std::string>::T layer4_s;
  GVector<std::string>::T layer5_s;
  layer5_s.push_back("v40");
  layer5_s.push_back("v41");
  ColumnMeta cMeta_s;
  cMeta_s.addDAGLayer(layer0_s);
  cMeta_s.addDAGLayer(layer1_s);
  cMeta_s.addDAGLayer(layer2_s);
  cMeta_s.addDAGLayer(layer3_s);
  cMeta_s.addDAGLayer(layer4_s);
  cMeta_s.addDAGLayer(layer5_s);

  std::string filter_v11 = "A(v11) C(10,int) O(>)";
  expressions[0] = Expression(filter_v11, cMeta_s);

  std::cout << "\nThe layer 5_s using filter, is:\n"; 
  CONTEXT_TABLE_PTR pt5_v21_s(new ContextTable(5, msg51_s.parents_, expressions, attr51));
  std::cout << *pt5_v21_s << std::endl;
  DAGTraversal traversal_s(pt5_v21_s);
  j1 = 0; 
  VECTOR_ANY attr_s(cMeta_s.colNumber, 0);
  while(traversal_s.next(cMeta_s, attr_s)){
    std::cout << "In the " << j1 << "th next() call visited the following vertices: \n";
    for (int i = 0; i < attr_s.size(); ++i) {
      std::cout << attr_s[i] << ", ";
    }
    std::cout << "\nduplicated number of this traversal is: " << traversal_s.getDuplicatedNum();
    std::cout<<std::endl;
    j1++;
  }
//Testing for the first layer is starting from non-zero layer
//Start testing on the traversal of context_table
  std::cout << "\n\n\nStart testing on non-zero layer" << std::endl;
  GVector<std::string>::T layer04;
  layer0.clear();
  layer0.push_back("v10");
  layer0.push_back("v11");
  layer1.clear();
  layer1.push_back("e10");
  layer1.push_back("e11");
  layer2.clear();
  layer2.push_back("v20");
  layer2.push_back("v21");
  layer3.clear();
  layer3.push_back("e20");
  layer3.push_back("e21");
  layer4.clear();
  layer4.push_back("v30");
  layer4.push_back("v31");
  layer5.clear();
  layer5.push_back("v40");
  layer5.push_back("v41");
  cMeta = ColumnMeta();
  cMeta.addDAGLayer(layer04);
  cMeta.addDAGLayer(layer04);
  cMeta.addDAGLayer(layer04);
  cMeta.addDAGLayer(layer04);
  cMeta.addDAGLayer(layer04);
  cMeta.addDAGLayer(layer0);
  cMeta.addDAGLayer(layer1);
  cMeta.addDAGLayer(layer2);
  cMeta.addDAGLayer(layer3);
  cMeta.addDAGLayer(layer4);
  cMeta.addDAGLayer(layer5);
  //build the graph  
  std::cout << "The layer 0, vertex,  is:\n";
  CONTEXT_TABLE_PTR p01_l5 = CONTEXT_TABLE_PTR(new ContextTable(CONTEXT_TABLE_NULL_PTR));
  p01_l5->isSkipped = true;
  CONTEXT_TABLE_PTR p02_l5 = CONTEXT_TABLE_PTR(new ContextTable(CONTEXT_TABLE_NULL_PTR));
  p02_l5->isSkipped = true;
  CONTEXT_TABLE_PTR p03_l5 = CONTEXT_TABLE_PTR(new ContextTable(CONTEXT_TABLE_NULL_PTR));
  p03_l5->isSkipped = true;
  CONTEXT_TABLE_PTR p04_l5 = CONTEXT_TABLE_PTR(new ContextTable(CONTEXT_TABLE_NULL_PTR));
  p04_l5->isSkipped = true;
  std::cout << "The layer 1, vertex,  is:\n";
  CONTEXT_TABLE_PTR p11_l5 = CONTEXT_TABLE_PTR(new ContextTable(1, p01_l5));
  p11_l5->isSkipped = true;
  CONTEXT_TABLE_PTR p12_l5 = CONTEXT_TABLE_PTR(new ContextTable(1, p02_l5));
  p12_l5->isSkipped = true;
  CONTEXT_TABLE_PTR p13_l5 = CONTEXT_TABLE_PTR(new ContextTable(1, p03_l5));
  p13_l5->isSkipped = true;
  CONTEXT_TABLE_PTR p14_l5 = CONTEXT_TABLE_PTR(new ContextTable(1, p04_l5));
  p14_l5->isSkipped = true;
  std::cout << "The layer 2, vertex,  is:\n";
  CONTEXT_TABLE_PTR p21_l5 = CONTEXT_TABLE_PTR(new ContextTable(2, p11_l5));
  p21_l5->isSkipped = true;
  CONTEXT_TABLE_PTR p22_l5 = CONTEXT_TABLE_PTR(new ContextTable(2, p12_l5));
  p22_l5->isSkipped = true;
  CONTEXT_TABLE_PTR p23_l5 = CONTEXT_TABLE_PTR(new ContextTable(2, p13_l5));
  p23_l5->isSkipped = true;
  CONTEXT_TABLE_PTR p24_l5 = CONTEXT_TABLE_PTR(new ContextTable(2, p14_l5));
  p24_l5->isSkipped = true;
  std::cout << "The layer 3, vertex,  is:\n";
  CONTEXT_TABLE_PTR p31_l5 = CONTEXT_TABLE_PTR(new ContextTable(3, p21_l5));
  p31_l5->isSkipped = true;
  CONTEXT_TABLE_PTR p32_l5 = CONTEXT_TABLE_PTR(new ContextTable(3, p22_l5));
  p32_l5->isSkipped = true;
  CONTEXT_TABLE_PTR p33_l5 = CONTEXT_TABLE_PTR(new ContextTable(3, p23_l5));
  p33_l5->isSkipped = true;
  CONTEXT_TABLE_PTR p34_l5 = CONTEXT_TABLE_PTR(new ContextTable(3, p24_l5));
  p34_l5->isSkipped = true;
  std::cout << "The layer 4, vertex,  is:\n";
  CONTEXT_TABLE_PTR p41_l5 = CONTEXT_TABLE_PTR(new ContextTable(4, p31_l5));
  p41_l5->isSkipped = true;
  CONTEXT_TABLE_PTR p42_l5 = CONTEXT_TABLE_PTR(new ContextTable(4, p32_l5));
  p42_l5->isSkipped = true;
  CONTEXT_TABLE_PTR p43_l5 = CONTEXT_TABLE_PTR(new ContextTable(4, p33_l5));
  p43_l5->isSkipped = true;
  CONTEXT_TABLE_PTR p44_l5 = CONTEXT_TABLE_PTR(new ContextTable(4, p34_l5));
  p44_l5->isSkipped = true;
  std::cout << "The layer 5, vertex,  is:\n";
  CONTEXT_TABLE_PTR pt01_l5 = CONTEXT_TABLE_PTR(new ContextTable(5, p41_l5));
  pt01_l5->attr.push_back("v10");
  pt01_l5->attr.push_back(10);
  pt01_l5->dupNumFromParents[0] = 10;
  std::cout << *pt01_l5 << std::endl;
  CONTEXT_TABLE_PTR pt02_l5 = CONTEXT_TABLE_PTR(new ContextTable(5, p42_l5));
  pt02_l5->dupNumFromParents[0] = 20;
  pt02_l5->attr.push_back("v11");
  pt02_l5->attr.push_back(11);
  std::cout << *pt02_l5 << std::endl;
  CONTEXT_TABLE_PTR pt03_l5 = CONTEXT_TABLE_PTR(new ContextTable(5, p43_l5));
  pt03_l5->dupNumFromParents[0] = 30;
  pt03_l5->attr.push_back("v12");
  pt03_l5->attr.push_back(11);
  std::cout << *pt03_l5 << std::endl;
  CONTEXT_TABLE_PTR pt04_l5 = CONTEXT_TABLE_PTR(new ContextTable(5, p44_l5));
  pt04_l5->dupNumFromParents[0] = 40;
  pt04_l5->attr.push_back("v13");
  pt04_l5->attr.push_back(8);
  std::cout << *pt04_l5 << std::endl;
  //layer 1
  std::cout << "The layer 6, edge,  is:\n";
  CONTEXT_TABLE_PTR pt11_l5 = CONTEXT_TABLE_PTR(new ContextTable(6, pt01_l5));
  pt11_l5->attr.push_back(std::string("e11"));
  pt11_l5->attr.push_back(1);
  std::cout << *pt11_l5 << std::endl;
  CONTEXT_TABLE_PTR pt12_l5 = CONTEXT_TABLE_PTR(new ContextTable(6, pt02_l5));
  pt12_l5->attr.push_back(std::string("e12"));
  pt12_l5->attr.push_back(1);
  std::cout << *pt12_l5 << std::endl;
  CONTEXT_TABLE_PTR pt13_l5 = CONTEXT_TABLE_PTR(new ContextTable(6, pt03_l5));
  pt13_l5->attr.push_back(std::string("e13"));
  pt13_l5->attr.push_back(1);
  std::cout << *pt13_l5 << std::endl;
  CONTEXT_TABLE_PTR pt14_l5 = CONTEXT_TABLE_PTR(new ContextTable(6, pt04_l5));
  pt14_l5->attr.push_back(std::string("e14"));
  pt14_l5->attr.push_back(1);
  std::cout << *pt14_l5 << std::endl;
  
  ContextTableMessage msg11_l5 = ContextTableMessage(pt11_l5);
  ContextTableMessage msg12_l5 = ContextTableMessage(pt12_l5);
  ContextTableMessage msg13_l5 = ContextTableMessage(pt13_l5);
  ContextTableMessage msg14_l5 = ContextTableMessage(pt14_l5);
  
  std::cout << "layer6 to layer7 message" << std::endl;
  std::cout << msg11_l5 << std::endl;
  std::cout << msg12_l5 << std::endl;
  std::cout << msg13_l5 << std::endl;
  std::cout << msg14_l5 << std::endl;

  std::cout << "After combine,  the layer6 to layer7 message\n" << std::endl;
  ContextTableMessage msg21_l5 = msg11_l5;
  msg21_l5 += msg12_l5;
  std::cout << msg21_l5 << std::endl;
  ContextTableMessage msg22_l5 = msg13_l5;
  msg22_l5 += msg14_l5;
  std::cout << msg22_l5 << std::endl;
  //layer 2
  std::cout << "The layer 7, vertex, is:\n";
  CONTEXT_TABLE_PTR pt21_l5 = CONTEXT_TABLE_PTR(new ContextTable(7, msg21_l5.parents_));
  pt21_l5->dupNumFromParents[0] = 3;
  pt21_l5->attr.push_back(std::string("v21"));
  pt21_l5->attr.push_back(20);
  //pt21->validPaths.clear();
  std::cout << *pt21_l5 << std::endl;
  CONTEXT_TABLE_PTR pt22_l5 = CONTEXT_TABLE_PTR(new ContextTable(7, msg22_l5.parents_));
  pt22_l5->dupNumFromParents[1] = 5;
  pt22_l5->attr.push_back(std::string("v22"));
  pt22_l5->attr.push_back(21);
  pt22_l5->validPaths.clear();
  pt22_l5->validPaths.push_back(1);
  std::cout << *pt22_l5 << std::endl;
  //layer 3
  std::cout << "The layer 8, edge,  is:\n";
  CONTEXT_TABLE_PTR pt31_l5 = CONTEXT_TABLE_PTR(new ContextTable(8, pt21_l5));
  pt31_l5->attr.push_back(std::string("e21"));
  pt31_l5->attr.push_back(2);
  std::cout << *pt31_l5 << std::endl;
  CONTEXT_TABLE_PTR pt32_l5 = CONTEXT_TABLE_PTR(new ContextTable(8, pt22_l5));
  pt32_l5->attr.push_back(std::string("e22"));
  pt32_l5->attr.push_back(1);
  std::cout << *pt32_l5 << std::endl;
  //layer 4
  ContextTableMessage msg31_l5 = ContextTableMessage(pt31_l5);
  ContextTableMessage msg32_l5 = ContextTableMessage(pt32_l5);
  std::cout << "layer8 to layer9 message, before combination\n" << std::endl;
  std::cout << msg31_l5 << std::endl;
  std::cout << msg32_l5 << std::endl;
  std::cout << "After combine,  the layer8 to layer9 message\n" << std::endl;
  ContextTableMessage msg312_l5 = msg31_l5;
  msg312_l5 += msg32_l5;
  std::cout << msg312_l5 << std::endl;

  std::cout << "The layer 4, vertex, is:\n";
  CONTEXT_TABLE_PTR pt41_l5 = CONTEXT_TABLE_PTR(new ContextTable(9, msg312_l5.parents_));
  pt41_l5->attr.push_back(std::string("v3"));
  pt41_l5->attr.push_back(11);
  std::cout << *pt41_l5 << std::endl;

  std::cout << "The layer 5, vertex, is:\n";
  ContextTableMessage msg51_l5 = ContextTableMessage(pt41_l5);
  CONTEXT_TABLE_PTR pt51_l5 = CONTEXT_TABLE_PTR(new ContextTable(9, msg51_l5.parents_));
  VECTOR_ANY attr51_l5;
  attr51_l5.push_back(std::string("v4"));
  attr51_l5.push_back(51);
  pt51_l5->attr = attr51_l5;
  std::cout << *pt51_l5 << std::endl;


  std::string filter_v3_l5 = "A(v11) A(v31) O(==)";
  GVector<Expression>::T expressions_l5;
  expressions_l5.push_back(Expression(filter_v3_l5, cMeta));
  std::string v31_l5 = "A(v31)";
  std::string v21_l5 = "A(v21)";
  std::string v10_l5 = "A(v11)";
  expressions_l5.push_back(Expression(v31_l5, cMeta));
  expressions_l5.push_back(Expression(v21_l5, cMeta));
  expressions_l5.push_back(Expression(v10_l5, cMeta));

  std::cout << "\nTesting the contextTable constructor\n";
  std::cout << "This is for testing the traversal and filter\n";
  StackNode sNode_l5(pt51_l5, pt51_l5->validPaths);  
  DAGTraversal traversal_l5; 
  uint32_t minLayerIndex_l5 = std::numeric_limits<uint32_t>::max();
  for (uint32_t i = 0; i < expressions_l5.size(); ++i) {
    if (expressions_l5[i].minLayerIndex_ < minLayerIndex_l5) {
      minLayerIndex_l5 = expressions_l5[i].minLayerIndex_;
    }
  }
  traversal_l5.minLayerIndex_ = minLayerIndex_l5;
  traversal_l5.pushValidParentsIntoStack(sNode_l5);
  uint32_t jump_l5 = 0;
  uint32_t j_l5 = 0;
  while(traversal_l5.next(expressions_l5, jump_l5)){
    std::cout << "In the " << j_l5 << "th next() call\n";
    if (expressions_l5[0].pass()) {
      std::cout << "it passes the filter with jump = " << jump_l5 << " and the traversal obtains the following values:\n";
      for (int i = expressions_l5.size() - 1; i >= 1; --i) {
        std::cout << expressions_l5[i].evaluate() << ",";
      }
      std::cout << "\nduplicated number of this traversal is: " << traversal_l5.getDuplicatedNum();
      std::cout << std::endl;
    } else {
      std::cout << "it does not pass the filter with jump = " << jump_l5 << " and the traversal obtains the following values:"<< std::endl;
      for (int i = expressions_l5.size() - 1; i >= 1; --i) {
        std::cout << expressions_l5[i].evaluate() << ",";
      }
      std::cout << "\nduplicated number of this traversal is: " << traversal_l5.getDuplicatedNum();
      std::cout << std::endl;
    }
    j_l5++;
  }

  // ContextTableMessage cm1;
  // cm1.parents_[pt1] = std::make_pair(1, 1);
  // ContextTableMessage cm2;
  // cm2.parents_[pt2] = std::make_pair(1, 1);
  // ContextTableMessage cm3;
  // cm3.parents_[pt3] = std::make_pair(1, 1);
  // ContextTableMessage cm4;
  // cm4.parents_[pt4] = std::make_pair(1, 1);
  // ContextTableMessage cm5;
  // cm5.parents_[pt5] = std::make_pair(1, 1);
  
  // std::cout << cm1 << std::endl;
  // std::cout << cm2 << std::endl;
  // std::cout << cm3 << std::endl;
  // std::cout << cm4 << std::endl;
  // std::cout << cm5 << std::endl;
  
  // ContextTableMessage cm6 = cm1;
  // cm6 += cm2;
  // cm6 += cm3;
  // std::cout << cm6 << std::endl;

  // ContextTableMessage cm7 = cm1;
  // cm7 += cm3;
  // cm7 += cm4;
  // std::cout << cm6 << std::endl;
  
  // cm6 += cm7;
  // std::cout << cm6 << std::endl;

  // cm6 += cm7;
  // std::cout << cm6 << std::endl;
  
  // cm6 += cm2;
  // std::cout << cm6 << std::endl;
  return 0;
}

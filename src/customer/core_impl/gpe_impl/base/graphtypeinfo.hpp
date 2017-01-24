/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.                                          *
 * All rights reserved                                                        *
 * Unauthorized copying of this file, via any medium is strictly prohibited   *
 * Proprietary and confidential                                               *
 ******************************************************************************/
/******************************************************************************
 * Project: GraphSQL-GPath
 *
 *  Created on: 7/1/2015
 *  Author: Renchu
 *  Description: Module Get Graph Meta Data
 ******************************************************************************/

#ifndef SRC_GRAPH_TYPE_INFO_HPP_
#define SRC_GRAPH_TYPE_INFO_HPP_

#include <gpe/serviceimplbase.hpp>
#include <gutil/gsqlcontainer.hpp>

using namespace gperun;

namespace UDIMPL {

enum ATTR_TYPE {INT_ATTR, DOUBLE_ATTR, STRING_ATTR, BOOL_ATTR};

class GraphMetaData {
private:
  // key: attr name -> value: <index, type>
  typedef GUnorderedMap<std::string, std::pair<uint32_t, uint32_t> >::T AttrInfo;
  // vertex type name
  // e.g., (company, member, skill)
  GUnorderedMap<uint32_t, std::string>::T vertexTypeName;
  GUnorderedMap<std::string, uint32_t>::T vertexTypeId;
  // vertex attributes
  GUnorderedMap<uint32_t, AttrInfo >::T vertexAttr;
  // vertex attribute names
  GUnorderedMap<uint32_t, GVector<std::string>::T >::T vertexAttrName;

  // edge type name
  // e.g., (company-member, member-member)
  GUnorderedMap<uint32_t, std::string>::T edgeTypeName;
  GUnorderedMap<std::string, uint32_t>::T edgeTypeId;
  // edge directed / undirected
  GUnorderedMap<uint32_t, bool>::T edgeIsDirected;
  // edge attributes
  GUnorderedMap<uint32_t, AttrInfo >::T edgeAttr;
  // edge attribute names
  GUnorderedMap<uint32_t, GVector<std::string>::T >::T edgeAttrName;
  // edge constraints
  GUnorderedMap<uint32_t, std::pair<std::string, std::string> >::T edgeConstraint;

public:
  void getVertexTypeIndex(const std::string& vertexName, uint32_t& id);
  void getEdgeTypeIndex(const std::string& edgeName, uint32_t& id);
  void getVertexTypeName(uint32_t id, std::string& vertexName);
  void getEdgeTypeName(uint32_t id, std::string& edgeName);
  void getVertexAttributeIndex(uint32_t vertexType, const std::string& attrName, uint32_t& id);
  void getEdgeAttributeIndex(uint32_t edgeType, const std::string& attrName, uint32_t& id);
  void getVertexAttributeType(uint32_t vertexType, const std::string& attrName, uint32_t& type);
  void getEdgeAttributeType(uint32_t edgeType, const std::string& attrName, uint32_t& type);
  void getAllVertexAttributes(uint32_t vertexType, GVector<std::string>::T& attributeNames);
  void getAllEdgeAttributes(uint32_t edgeType, GVector<std::string>::T& attributeNames);

  void getGraphMeta(ServiceAPI& serviceApi);

  friend std::ostream& operator<< (std::ostream& os, GraphMetaData& data) {
    os << "vertex types: [";
    for (uint32_t i = 0; i < data.vertexTypeName.size(); ++i) os << data.vertexTypeName[i] << ", ";
    os << "]\n";

    os << "vertex attributes: [\n";
    for (GUnorderedMap<uint32_t, AttrInfo >::T::const_iterator iter = data.vertexAttr.begin(); iter != data.vertexAttr.end(); ++iter) {
      std::string attrName;
      data.getVertexTypeName(iter->first, attrName);
      os << attrName << " -> {";
      AttrInfo attrlist = iter->second;
      for (AttrInfo::const_iterator it = attrlist.begin(); it != attrlist.end(); ++it) {
        os << "(" << it->first << ", " << it->second.first << ", " << it->second.second << "), "; 
      }
      os << "}\n";
    }
    os << "]\n";
    
    os << "edge <typename, isdrected, from, to>: [";
    for (uint32_t i = 0; i < data.edgeTypeName.size(); ++i) {
      std::string typeName = data.edgeTypeName[i];
      uint32_t typeIndex;
      data.getEdgeTypeIndex(typeName, typeIndex);
      os << "(" << typeName << ", " << data.edgeIsDirected[typeIndex] << ", " << data.edgeConstraint[typeIndex].first << ", " << data.edgeConstraint[typeIndex].second << "), "; 
    }
    os << "]\n";

    os << "edge attributes: [\n";
    for (GUnorderedMap<uint32_t, AttrInfo >::T::const_iterator iter = data.edgeAttr.begin(); iter != data.edgeAttr.end(); ++iter) {
      os << iter->first << " -> {";
      AttrInfo attrlist = iter->second;
      for (AttrInfo::const_iterator it = attrlist.begin(); it != attrlist.end(); ++it) {
        os << "(" << it->first << ", " << it->second.first << ", " << it->second.second << "), "; 
      }
      os << "}\n";
    }
    os << "]\n";

    return os;
  }
};

}
#endif


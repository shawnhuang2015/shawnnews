/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
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
  typedef GMap<std::string, std::pair<uint32_t, uint32_t> >::T attrListType;
  // vertex type name
  // e.g., (company, member, skill)
  GVector<std::string>::T vertexTypeName;
  GMap<std::string, uint32_t>::T vertexTypeId;
  // vertex attributes
  GMap<std::string, attrListType >::T vertexAttr;
  // vertex attribute names
  GMap<std::string, GVector<std::string>::T >::T vertexAttrName;

  // edge type name
  // e.g., (company-member, member-member)
  GVector<std::string>::T edgeTypeName;
  GMap<std::string, uint32_t>::T edgeTypeId;
  // edge directed / undirected
  GMap<std::string, bool>::T edgeIsDirected;
  // edge attributes
  GMap<std::string, attrListType >::T edgeAttr;
  // edge attribute names
  GMap<std::string, GVector<std::string>::T >::T edgeAttrName;
  // edge constraints
  GMap<std::string, std::pair<std::string, std::string> >::T edgeConstraint;

public:
  bool getVertexTypeIndex(std::string vertexName, uint32_t& id);
  bool getEdgeTypeIndex(std::string edgeName, uint32_t& id);
  bool getVertexTypeName(uint32_t id, std::string& vertexName);
  bool getEdgeTypeName(uint32_t id, std::string& edgeName);
  bool getVertexAttributeIndex(std::string vertexName, std::string attrName, uint32_t& id);
  bool getEdgeAttributeIndex(std::string edgeName, std::string attrName, uint32_t& id);
  bool getVertexAttributeType(std::string vertexName, std::string attrName, uint32_t& type);
  bool getEdgeAttributeType(std::string edgeName, std::string attrName, uint32_t& type);
  bool getAllVertexAttributes(std::string vertexName, GVector<std::string>::T& attributeNames);
  bool getAllEdgeAttributes(std::string edgeName, GVector<std::string>::T& attributeNames);

  void getGraphMeta(ServiceAPI& serviceApi);

  friend std::ostream& operator<< (std::ostream& os, GraphMetaData& data) {
    os << "vertex typename: [";
    for (uint32_t i = 0; i < data.vertexTypeName.size(); ++i) os << data.vertexTypeName[i] << ", ";
    os << "]\n";

    os << "vertex attributes: [\n";
    for (GMap<std::string, attrListType >::T::const_iterator iter = data.vertexAttr.begin(); iter != data.vertexAttr.end(); ++iter) {
      os << iter->first << " -> {";
      attrListType attrlist = iter->second;
      for (attrListType::const_iterator it = attrlist.begin(); it != attrlist.end(); ++it) {
        os << "(" << it->first << ", " << it->second.first << ", " << it->second.second << "), "; 
      }
      os << "}\n";
    }
    os << "]\n";
    
    os << "edge <typename, isdrected, from, to>: [";
    for (uint32_t i = 0; i < data.edgeTypeName.size(); ++i) {
      std::string typename_ = data.edgeTypeName[i];
      os << "(" << typename_ << ", " << data.edgeIsDirected[typename_] << ", " << data.edgeConstraint[typename_].first << ", " << data.edgeConstraint[typename_].second << "), "; 
    }
    os << "]\n";

    os << "edge attributes: [\n";
    for (GMap<std::string, attrListType >::T::const_iterator iter = data.edgeAttr.begin(); iter != data.edgeAttr.end(); ++iter) {
      os << iter->first << " -> {";
      attrListType attrlist = iter->second;
      for (attrListType::const_iterator it = attrlist.begin(); it != attrlist.end(); ++it) {
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


/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPath
 *
 *  Created on: 7/1/2015
 *  Author: Renchu
 *  Description: Module Get Graph Meta Data
 ******************************************************************************/

#include "graphtypeinfo.hpp"

namespace UDIMPL {

bool GraphMetaData::getVertexTypeIndex(std::string vertexName, uint32_t& id) {
  if (vertexTypeId.find(vertexName) != vertexTypeId.end()) {
    id = vertexTypeId[vertexName];
    return true;
  } else {
    return false;
  }
}

bool GraphMetaData::getEdgeTypeIndex(std::string edgeName, uint32_t& id) {
  if (edgeTypeId.find(edgeName) != edgeTypeId.end()) {
    id = edgeTypeId[edgeName];
    return true;
  } else {
    return false;
  }
}

bool GraphMetaData::getVertexTypeName(uint32_t id, std::string& vertexName) {
  if (id < vertexTypeName.size()) {
    vertexName = vertexTypeName[id];
    return true;
  } else {
    return false;
  }
}

bool GraphMetaData::getEdgeTypeName(uint32_t id, std::string& edgeName) {
  if (id < edgeTypeName.size()) {
    edgeName = edgeTypeName[id];
    return true;
  } else {
    return false;
  }
}

bool GraphMetaData::getVertexAttributeIndex(std::string vertexName, std::string attrName, uint32_t& id) {
  if (vertexAttr.find(vertexName) != vertexAttr.end()) {
    attrListType& attrList = vertexAttr[vertexName];
    if (attrList.find(attrName) != attrList.end()) {
      id = attrList[attrName].first;
      return true;
    }
  }
  return false;
}

bool GraphMetaData::getEdgeAttributeIndex(std::string edgeName, std::string attrName, uint32_t& id) {
  if (edgeAttr.find(edgeName) != edgeAttr.end()) {
    attrListType& attrList = edgeAttr[edgeName];
    if (attrList.find(attrName) != attrList.end()) {
      id = attrList[attrName].first;
      return true;
    }
  }
  return false;
}

bool GraphMetaData::getVertexAttributeType(std::string vertexName, std::string attrName, uint32_t& type) {
  if (vertexAttr.find(vertexName) != vertexAttr.end()) {
    attrListType& attrList = vertexAttr[vertexName];
    if (attrList.find(attrName) != attrList.end()) {
      type = attrList[attrName].second;
      return true;
    }
  }
  return false;
}

bool GraphMetaData::getEdgeAttributeType(std::string edgeName, std::string attrName, uint32_t& type) {
  if (edgeAttr.find(edgeName) != edgeAttr.end()) {
    attrListType& attrList = edgeAttr[edgeName];
    if (attrList.find(attrName) != attrList.end()) {
      type = attrList[attrName].second;
      return true;
    }
  }
  return false;
}

bool GraphMetaData::getAllVertexAttributes(std::string vertexName, GVector<std::string>::T& attributeNames) {
  if (vertexAttrName.find(vertexName) != vertexAttrName.end()) {
    attributeNames = vertexAttrName[vertexName];
    return true;
  }
  return false;
}

bool GraphMetaData::getAllEdgeAttributes(std::string edgeName, GVector<std::string>::T& attributeNames) {
  if (edgeAttrName.find(edgeName) != edgeAttrName.end()) {
    attributeNames = edgeAttrName[edgeName];
    return true;
  }
  return false;
}

void GraphMetaData::getGraphMeta(ServiceAPI& serviceApi) {
  topology4::TopologyMeta* topoMeta = serviceApi.GetTopologyMeta();
  std::vector<topology4::VertexTypeMeta>& vertexMeta = topoMeta->vertextypemeta_;
  std::vector<topology4::EdgeTypeMeta>& edgeMeta = topoMeta->edgetypemeta_;
  for (uint32_t i = 0; i < vertexMeta.size(); ++i) {
    std::string typeName = vertexMeta[i].typename_;
    vertexTypeName.push_back(typeName);
    vertexTypeId[typeName] = vertexMeta[i].typeid_;
    std::vector<topology4::AttributeMeta> attributes = vertexMeta[i].attributes_.attributelist_;
    //if (attributes.size() == 0) attributes = vertexMeta[i].attributes_ignored_.attributelist_;
    attrListType attributeList;
    GVector<std::string>::T vAttrName;
    for (uint32_t j = 0; j < attributes.size(); ++j) {
      // std::string attrName = topology4::Attribute::type2name(attributes[j].type_);
      // if (attributes[j].enumerator_ != 0 ) attrName += "ENUM";
      vAttrName.push_back(attributes[j].name_);
      attributeList[attributes[j].name_] = std::make_pair(j, attributes[j].type_);    // type_ : e.g., topology4::Attribute::BOOL
    }
    vertexAttr[typeName] = attributeList;
    vertexAttrName[typeName] = vAttrName;
  }
  for (uint32_t i = 0; i < edgeMeta.size(); ++i) {
    std::string typeName = edgeMeta[i].typename_;
    edgeTypeName.push_back(typeName);
    edgeTypeId[typeName] = edgeMeta[i].typeid_;
    edgeIsDirected[typeName] = edgeMeta[i].isdirected_;
    edgeConstraint[typeName] = std::make_pair(edgeMeta[i].fromtypename_, edgeMeta[i].totypename_);
    std::vector<topology4::AttributeMeta> attributes = edgeMeta[i].attributes_.attributelist_;
    //if (attributes.size() == 0) attributes = edgeMeta[i].attributes_ignored_.attributelist_;
    attrListType attributeList;
    GVector<std::string>::T eAttrName;
    for (uint32_t j = 0; j < attributes.size(); ++j) {
      // std::string attrName = topology4::Attribute::type2name(attributes[j].type_);
      // if (attributes[j].enumerator_ != 0 ) attrName += "ENUM";
      eAttrName.push_back(attributes[j].name_);
      attributeList[attributes[j].name_] = std::make_pair(j, attributes[j].type_);    // type_ : e.g., topology4::Attribute::BOOL
    }
    edgeAttr[typeName] = attributeList;
    edgeAttrName[typeName] = eAttrName;
  }
}

} // namespace UDIMPL


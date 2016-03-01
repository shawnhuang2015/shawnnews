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

#include "graphtypeinfo.hpp"

namespace UDIMPL {

void GraphMetaData::getVertexTypeIndex(const std::string& vertexName, uint32_t& id) {
  id = vertexTypeId[vertexName];
}

void GraphMetaData::getEdgeTypeIndex(const std::string& edgeName, uint32_t& id) {
  id = edgeTypeId[edgeName];
}

void GraphMetaData::getVertexTypeName(uint32_t id, std::string& vertexName) {
  vertexName = vertexTypeName[id];
}

void GraphMetaData::getEdgeTypeName(uint32_t id, std::string& edgeName) {
  edgeName = edgeTypeName[id];
}

void GraphMetaData::getVertexAttributeIndex(uint32_t vertexType, const std::string& attrName, uint32_t& id) {
  AttrInfo& attrList = vertexAttr[vertexType];
  id = attrList[attrName].first;
}

void GraphMetaData::getEdgeAttributeIndex(uint32_t edgeType, const std::string& attrName, uint32_t& id) {
  AttrInfo& attrList = edgeAttr[edgeType];
  id = attrList[attrName].first;
}

void GraphMetaData::getVertexAttributeType(uint32_t vertexType, const std::string& attrName, uint32_t& type) {
  AttrInfo& attrList = vertexAttr[vertexType];
  type = attrList[attrName].second;
}

void GraphMetaData::getEdgeAttributeType(uint32_t edgeType, const std::string& attrName, uint32_t& type) {
  AttrInfo& attrList = edgeAttr[edgeType];
  type = attrList[attrName].second;
}

void GraphMetaData::getAllVertexAttributes(uint32_t vertexType, GVector<std::string>::T& attributeNames) {
  attributeNames = vertexAttrName[vertexType];
}

void GraphMetaData::getAllEdgeAttributes(uint32_t edgeType, GVector<std::string>::T& attributeNames) {
  attributeNames = edgeAttrName[edgeType];
}

// TODO: Once system replace all service api container to GContainer, replace them here.
void GraphMetaData::getGraphMeta(ServiceAPI& serviceApi) {
  topology4::TopologyMeta* topoMeta = serviceApi.GetTopologyMeta();
  std::vector<topology4::VertexTypeMeta>& vertexMeta = topoMeta->vertextypemeta_;
  std::vector<topology4::EdgeTypeMeta>& edgeMeta = topoMeta->edgetypemeta_;
  for (uint32_t i = 0; i < vertexMeta.size(); ++i) {
    std::string typeName = vertexMeta[i].typename_;
    uint32_t typeId = vertexMeta[i].typeid_;
    vertexTypeName[typeId] = typeName;
    vertexTypeId[typeName] = typeId;
    std::vector<topology4::AttributeMeta> attributes = vertexMeta[i].attributes_.attributelist_;
    AttrInfo attributeList;
    GVector<std::string>::T vAttrName;
    for (uint32_t j = 0; j < attributes.size(); ++j) {
      vAttrName.push_back(attributes[j].name_);
      // type_ : e.g., topology4::A    ttribute::BOOL
      attributeList[attributes[j].name_] = std::make_pair(j, attributes[j].type_);
    }
    vertexAttr[typeId] = attributeList;
    vertexAttrName[typeId] = vAttrName;
  }
  for (uint32_t i = 0; i < edgeMeta.size(); ++i) {
    std::string typeName = edgeMeta[i].typename_;
    uint32_t typeId = edgeMeta[i].typeid_;
    edgeTypeName[typeId] = typeName;
    edgeTypeId[typeName] = typeId;
    edgeIsDirected[typeId] = edgeMeta[i].isdirected_;
    edgeConstraint[typeId] = std::make_pair(edgeMeta[i].fromtypename_, edgeMeta[i].totypename_);
    std::vector<topology4::AttributeMeta> attributes = edgeMeta[i].attributes_.attributelist_;
    AttrInfo attributeList;
    GVector<std::string>::T eAttrName;
    for (uint32_t j = 0; j < attributes.size(); ++j) {
      eAttrName.push_back(attributes[j].name_);
      // type_ : e.g., topology4::A    ttribute::BOOL
      attributeList[attributes[j].name_] = std::make_pair(j, attributes[j].type_);
    }
    edgeAttr[typeId] = attributeList;
    edgeAttrName[typeId] = eAttrName;
  }
}

} // namespace UDIMPL


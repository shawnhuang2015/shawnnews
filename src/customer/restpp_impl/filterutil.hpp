/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: RESTPP
 * filterutil.cpp: filter utility functions.
 *
 *  Created on: July 8, 2015
 *      Author: jzhong
 ******************************************************************************/
#ifndef FILTER_UTIL_CPP
#define FILTER_UTIL_CPP

#include <restpp/librestpp.hpp>
#include <sstream>

void IdSet::Init(SchemaManager *schema_manager_) {
  schema_manager = schema_manager_;
}

IdSet::~IdSet() {
  for (size_t i = 0; i < string_rep.size(); ++i){delete string_rep[i];}
}

bool IdSet::AddId(std::string type_name, std::string id) {
  int type_id = schema_manager->GetVertexTypeId(type_name);
  return AddId(type_id, id);
}

bool IdSet::AddId(uint32_t type_id, std::string id) {
  const restpp::VertexTypeMeta *vertex_type = schema_manager->GetVertexType(type_id);
  if (vertex_type == NULL){
    return false;
  }
  if (vertex_type->startvid_ != (uint64_t)-1){
    return true;
  }
  AddIdSafe(type_id, id);
  return true;
}

void IdSet::AddIdSafe(uint32_t type_id, std::string &id) {
  TypedId typed_id;
  typed_id.type_id = type_id;
  typed_id.id = id;
  if (id_set.find(typed_id) == id_set.end()) {
    id_set.insert(typed_id);
  }
}

void IdSet::clear() {
  id_set.clear();
  for (size_t i = 0; i < string_rep.size(); ++i) {
    string_rep[i]->str(std::string());
  }
}

uint32_t IdSet::size() { return id_set.size(); }


#endif

/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 *
 *      Author: lichen
 ******************************************************************************/

#ifndef SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_
#define SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_

#include <gpe/serviceimplbase.hpp>
#include "kneighborsize.hpp"
#include "kstepneighborhoodsubgraph.hpp"
#include "querydispatcher.hpp"
#include "export_ontology_tree.hpp"

using namespace gperun;

namespace UDIMPL {

// TODO(@alan): const for semantic layer, move to somewhere else
const std::string OBJ("object");
const std::string ONTO("ontology");
const std::string OBJ_ONTO("object_ontology");
const std::string PROF("profile");
const std::string ONTO_VTYPE_PREF = "__onto_v_";
const std::string ONTO_ETYPE_PREF_UP = "__onto_e_up_";
const std::string ONTO_ETYPE_PREF_DOWN = "__onto_e_down_";
const std::string OBJ_ONTO_ETYPE_PREF = "__obj_onto_e_";
const std::string SEMANTIC_SCHEMA_PATH = "/tmp/semantic.json";
const std::string SCHEMA_CHANGE_SCRIPT_PATH = "/tmp/sc.sh";

class UDFRunner : public ServiceImplBase {
 private:
  Json::Value semantic_schema;
  std::map<std::string, uint32_t> vertex_type_map;
  std::map<uint32_t, std::string> vertex_type_reverse_map;
  std::map<std::string, uint32_t> edge_type_map;
  std::map<uint32_t, std::string> edge_type_reverse_map;

 public:
  void Init(ServiceAPI& serviceapi) {
    // check and load semantic schema
    std::ifstream f(SEMANTIC_SCHEMA_PATH.c_str());
    if (f.good()) {
      Json::Reader reader;
      bool success = false;
      success = reader.parse(f, semantic_schema);
      if (! success) {
        std::cout << "fail to parse semantic schema file " << SEMANTIC_SCHEMA_PATH << std::endl;
      }
      std::cout << "parsed semantic schema file " << SEMANTIC_SCHEMA_PATH << std::endl;
    }

    // create a map: vertex/edge type name to id
    topology4::TopologyMeta* topology = serviceapi.GetTopologyMeta();
    uint32_t num_vertex_types = topology->vertextypemeta_.size();

    for (uint32_t i = 0; i < num_vertex_types; ++i) {
      std::string vertex_type_name = std::string(
          topology->GetVertexType(i).typename_);
      vertex_type_map[vertex_type_name] = i;
      vertex_type_reverse_map[i] = vertex_type_name;
    }

    uint32_t num_edge_types = topology->edgetypemeta_.size();
    for (uint32_t i = 0; i < num_edge_types; ++i) {
      std::string edge_type_name =
        std::string(topology->GetEdgeType(i).typename_);
      edge_type_map[edge_type_name] = i;
      edge_type_reverse_map[i] = edge_type_name;
    }

  }

  bool RunQuery(ServiceAPI& serviceapi, EngineServiceRequest& request) {
    if (request.request_function_ == "kneighborsize") {
      return RunUDF_KNeighborSize(serviceapi, request);
    } else if (request.request_function_ == "queryDispatcher") {
#ifdef RUNQUERY
        std::string queryName = request.jsoptions_["query_name"][0].asString();
        QueryDispatcher qdp(queryName,serviceapi, request);
        return qdp.RunQuery();
#else
        return false;
#endif
    } else if (request.request_function_ == "semantic_def") {
      return SemanticDef(request);
    } else if (request.request_function_ == "ontology_import") {
      return OntologyImport(request);
    } else if (request.request_function_ == "user_search") {
      return RunUDF_UserSearch(serviceapi, request);
    } else if (request.request_function_ == "get_ontology") {
      return RunUDF_GetOntology(serviceapi, request);
    }
    
    return false;  /// not a valid request
  }

 private:
  bool RunUDF_KNeighborSize(ServiceAPI& serviceapi,
                            EngineServiceRequest& request) {
    // sample to convert vid.
    VertexLocalId_t local_start;
    // if (!serviceapi.UIdtoVId(request, request.request_argv_[1], local_start))
    if (!serviceapi.UIdtoVId(request, request.request_data_, local_start))
      return false;
    request.outputwriter_->WriteStartObject();
    request.outputwriter_->WriteName("vertex");
    {
      // sample to use GraphAPI
      boost::shared_ptr<GraphAPI> graphapi =
          serviceapi.CreateGraphAPI(&request);
      VertexAttribute v1;
      graphapi->GetOneVertex(local_start, v1);
      v1.WriteToJson(*request.outputwriter_);
    }
    request.outputwriter_->WriteName("neighborsize");
    // sample to run one UDF
    typedef KNeighborSize UDF_t;
    UDF_t udf(3, local_start, request.outputwriter_);
    serviceapi.RunUDF(&request, &udf);
    if (udf.abortmsg_.size() > 0) {
      request.error_ = true;
      request.message_ += udf.abortmsg_;
      return true;
    }
    request.outputwriter_->WriteEndObject();
    request.output_idservice_vids.push_back(local_start);
    return true;
  }

  bool RunUDF_UserSearch(ServiceAPI& serviceapi, EngineServiceRequest& request) {
    Json::Value& jsoptions = request.jsoptions_;
    std::cout << jsoptions.toStyledString() << std::endl;
    std::cout << request.request_data_ << std::endl;
    uint32_t limit = std::numeric_limits<uint32_t>::max(); 
    if (jsoptions.isMember("limit") && jsoptions["limit"].size() > 0) {
        limit = jsoptions["limit"][0].asUInt();
    }

    return true;
  }

  bool SemanticDef(EngineServiceRequest &request) {
    const Json::Value &jsoptions = request.jsoptions_;

    if (! jsoptions.isMember("payload")) {
      request.error_ = true;
      request.message_ += "Payload missing.";
      return true;
    }

    // copy the payload, modify
    Json::Value payload = jsoptions["payload"];

    // check for "object", should be present
    if (payload.isMember(OBJ) && payload[OBJ].isArray()) {
    } else {
      request.error_ = true;
      request.message_ += OBJ + " missing or not array.";
      return false;
    }

    // check for "ontology" & "object_ontology", might be absent
    if (payload.isMember(ONTO) == payload.isMember(OBJ_ONTO)) {
      if (payload.isMember(ONTO)) {
        if (payload[ONTO].isArray() && payload[OBJ_ONTO].isArray()) {
        } else {
          request.error_ = true;
          request.message_ += ONTO + " or " + OBJ_ONTO + " not array.";
          return false;
        }

        // handle ONTOLOGY, assign vtype/etype names
        Json::Value onto;
        int size = payload[ONTO].size();
        for (int i = 0; i < size; ++i) {
          Json::Value one;
          std::string name(payload[ONTO][i].asString());
          one["name"] = name;
          one["vtype"] = ONTO_VTYPE_PREF + name;

          Json::Value two;
          two["up"] = ONTO_ETYPE_PREF_UP + name;
          two["down"] = ONTO_ETYPE_PREF_DOWN + name;
          one["etype"] = two;

          onto.append(one);
        }
        payload[ONTO] = onto;

        // handle OBJ_ONTOLOGY, assign etype names
        Json::Value obj_onto;
        size = payload[OBJ_ONTO].size();
        for (int i = 0; i < size; ++i) {
          const Json::Value &js = payload[OBJ_ONTO][i];
          Json::Value one;
          std::string obj(js["object"].asString());
          one["object"] = obj;

          int size1 = js["ontology"].size();
          for (int j = 0; j < size1; ++j) {
            std::string name(js["ontology"][j].asString());
            Json::Value two;
            two["name"] = name;
            two["etype"] = OBJ_ONTO_ETYPE_PREF + obj + "_to_" + name;
            one["ontology"].append(two);
          }
          obj_onto.append(one);
        }
        payload[OBJ_ONTO] = obj_onto;
      }
    } else {
      request.error_ = true;
      request.message_ += ONTO + " or " + OBJ_ONTO + " missing.";
      return false;
    }

    // TODO(@alan):
    // diff old version "ontology" vs the above version to 
    // figure out what ontology vertex/edge will be dropped and created
    // or
    // don't do diff here, just dump this new schema to a file
    // let the external script do the diff and run schema change.
 
    // check for "profile", should be present
    if (payload.isMember(PROF) && payload[PROF].isArray()) {
    } else {
      request.error_ = true;
      request.message_ += PROF + " missing or not array.";
      return false;
    }

    semantic_schema = payload;

    // TODO(@alan):
    // persist "payload" as semantic schema to disk or redis
    std::string path(SEMANTIC_SCHEMA_PATH.c_str());
    std::ofstream fp(path.c_str());
    fp << payload;
    fp.close();

    // trigger dynamic schema change job (external script)
    // TODO(@alan):
    // generate/run ddl job via an external script.
    system((SCHEMA_CHANGE_SCRIPT_PATH + " " + path).c_str());
    return true;
  }

  bool OntologyImport(EngineServiceRequest& request) {
    const Json::Value &jsoptions = request.jsoptions_;
    // lookup `obj_onto' map to find (obj, onto) pair
    // lookup 'onto' map to find vtype/etype for onto
    if (! (jsoptions.isMember("object") && jsoptions.isMember("name"))) {
      request.error_ = true;
      request.message_ += "object or name missing.";
      return false;
    }
    const std::string obj(jsoptions["object"][0].asString());
    const std::string name(jsoptions["name"][0].asString());

    // validate (obj, name) pair
    bool valid = false;
    const Json::Value &obj_onto = semantic_schema[OBJ_ONTO];
    int size = obj_onto.size();
    for (int i = 0; i < size; ++i) {
      if (obj_onto[i][OBJ].asString() == obj) {
        const Json::Value &onto = obj_onto[i][ONTO];
        int size1 = onto.size();
        for (int j = 0; j < size1; ++j) {
          if (onto[j]["name"].asString() == name) {
            valid = true;
            break;
          }
        }
        break;
      }
    }
    if (! valid) {
      request.error_ = true;
      request.message_ += "(" + obj + ", " + name + ") not found in " + OBJ_ONTO;
      return false;
    }

    // TODO(@alan): replaced by GetOntologyVEType
    // get vtype/etype for ontology tree
    const Json::Value &onto = semantic_schema[ONTO];
    size = onto.size();
    for (int i = 0; i < size; ++i) {
      if (onto[i]["name"].asString() == name) {
        request.outputwriter_->WriteStartObject();
        request.outputwriter_->WriteName("vtype");
        request.outputwriter_->WriteString(onto[i]["vtype"].asString());

        request.outputwriter_->WriteName("etype");
        request.outputwriter_->WriteStartObject();
        request.outputwriter_->WriteName("up");
        request.outputwriter_->WriteString(onto[i]["etype"]["up"].asString());
        request.outputwriter_->WriteName("down");
        request.outputwriter_->WriteString(onto[i]["etype"]["down"].asString());
        request.outputwriter_->WriteEndObject();

        request.outputwriter_->WriteEndObject();
        break;
      }
    }

    return true;
  }

  int GetOntologyVEType(const std::string &name, std::map<std::string, std::string> &rez) {
    // get vtype/etype for ontology tree
    const Json::Value &onto = semantic_schema[ONTO];
    int size = onto.size();
    for (int i = 0; i < size; ++i) {
      if (onto[i]["name"].asString() == name) {
        rez["vtype"] = onto[i]["vtype"].asString();
        rez["up_etype"] = onto[i]["etype"]["up"].asString();
        rez["down_etype"] = onto[i]["etype"]["down"].asString();
        return 0;
      }
    }
    return -1;
  }

  bool RunUDF_GetOntology(ServiceAPI& serviceapi,
                            EngineServiceRequest& request) {
    if (! request.jsoptions_.isMember("name")) {
      request.error_ = true;
      request.message_ += "name missing.";
      return false;
    }

    JSONWriter* writer_ = request.outputwriter_;
    int size = request.jsoptions_["name"].size();

    writer_->WriteStartArray();
    typedef std::map<VertexLocalId_t, std::vector<VertexLocalId_t> > tree_t;
    tree_t tree;

    for (int i = 0; i < size; ++i) {
      std::string name = request.jsoptions_["name"][i].asString();
      std::map<std::string, std::string> rez;
      if (GetOntologyVEType(name, rez) != 0) {
        request.error_ = true;
        request.message_ += name + " not found in " + ONTO;
        return false;
      }

      if ((vertex_type_map.find(rez["vtype"]) == vertex_type_map.end()) ||
          (vertex_type_map.find(rez["up_etype"]) == vertex_type_map.end()) ||
          (vertex_type_map.find(rez["down_etype"]) == vertex_type_map.end())) {
        request.error_ = true;
        request.message_ += "vtype or etype not found in graph meta.";
        return false;
      }

      uint32_t vtype_id = vertex_type_map[rez["vtype"]];
      uint32_t down_etype_id = edge_type_map[rez["down_etype"]];

      // run udf to get the tree
      typedef ExportOntologyTree UDF_t;

      tree.clear();
      UDF_t udf(1, vtype_id, down_etype_id, tree);
      serviceapi.RunUDF(&request, &udf);

      if (tree.size() > 0) {
        writer_->WriteStartObject();
        writer_->WriteName("name");
        writer_->WriteString(name);
        writer_->WriteName("tree");
        writer_->WriteStartArray();
        for (tree_t::iterator it = tree.begin(); it != tree.end(); ++it) {
          writer_->WriteStartObject();
          writer_->WriteName("parent");
          writer_->WriteMarkVId(it->first);
          writer_->WriteName("children");
          writer_->WriteStartArray();
          for (std::vector<VertexLocalId_t>::iterator it1 = it->second.begin();
               it1 != it->second.end(); ++it1) {
            writer_->WriteMarkVId(*it1);
          }
          writer_->WriteEndArray();
          writer_->WriteEndObject();
        }
        writer_->WriteEndArray();
        writer_->WriteEndObject();
      }
    }
    writer_->WriteEndArray();
    return true;
  }

};
}  // namespace UDIMPL
#endif  // SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_

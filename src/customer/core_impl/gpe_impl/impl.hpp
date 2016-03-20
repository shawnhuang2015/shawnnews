/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 *
 *      Author: lichen
 ******************************************************************************/

#ifndef SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_
#define SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_

//#include <gpe/gpath_base/graphtypeinfo.hpp>
#include <base/graphtypeinfo.hpp>
#include <gpe/serviceimplbase.hpp>
#include "kneighborsize.hpp"
#include "kstepneighborhoodsubgraph.hpp"
#include "querydispatcher.hpp"
#include "export_ontology_tree.hpp"
#include "business/UserSearchMain.hpp"
#include "get_tag.hpp"
#include "locate_tag.hpp"

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
const std::string SCHEMA_DIFF_PATH = "/tmp/diff.json";
const std::string SCHEMA_CHANGE_SCRIPT_PATH = "/tmp/sc.sh";
const std::string CROWD_INDEX("crowdIndex");
const std::string CROWD_INDEX_VTYPE = "__crowd_index";
const std::string USER_CROWD_INDEX_ETYPE = "__user_to_crowd_index";

class UDFRunner : public ServiceImplBase {
 private:
  Json::Value semantic_schema;
  std::map<std::string, uint32_t> vertex_type_map;
  std::map<uint32_t, std::string> vertex_type_reverse_map;
  std::map<std::string, uint32_t> edge_type_map;
  std::map<uint32_t, std::string> edge_type_reverse_map;
  std::map<std::string, std::vector<VertexLocalId_t> > inverted_tag_cache;
  bool refresh_inverted_tag_cache;

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

    // load graph meta
    LoadGraphMeta(serviceapi);

    // init some data member
    refresh_inverted_tag_cache = true;
  }

  void LoadGraphMeta(ServiceAPI &serviceapi) {
    // create a map: vertex/edge type name to id
    topology4::TopologyMeta* topology = serviceapi.GetTopologyMeta();
    uint32_t num_vertex_types = topology->vertextypemeta_.size();

    std::cout << "loading graph meta:" << std::endl;
    for (uint32_t i = 0; i < num_vertex_types; ++i) {
      std::string vertex_type_name = std::string(
          topology->GetVertexType(i).typename_);
      vertex_type_map[vertex_type_name] = i;
      vertex_type_reverse_map[i] = vertex_type_name;
      std::cout << vertex_type_name << " = " << i << std::endl;
    }

    uint32_t num_edge_types = topology->edgetypemeta_.size();
    for (uint32_t i = 0; i < num_edge_types; ++i) {
      std::string edge_type_name =
        std::string(topology->GetEdgeType(i).typename_);
      edge_type_map[edge_type_name] = i;
      edge_type_reverse_map[i] = edge_type_name;
      std::cout << edge_type_name << " = " << i << std::endl;
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
      return SemanticDef(serviceapi, request);
    } else if (request.request_function_ == "ontology_import") {
      return OntologyImport(request);
    } else if (request.request_function_ == "user_search") {
      return RunUDF_UserSearch(serviceapi, request);
    } else if (request.request_function_ == "get_crowd_by_name") {
      return RunUDF_GetCrowdByName(serviceapi, request);
    } else if (request.request_function_ == "delete_crowd_by_name") {
      return RunUDF_DeleteCrowdByName(serviceapi, request);
    } else if (request.request_function_ == "get_ontology") {
      return RunUDF_GetOntology(serviceapi, request);
    } else if (request.request_function_ == "get_profile") {
      return RunUDF_GetProfile(serviceapi, request);
    } else if (request.request_function_ == "pre_set_tag") {
      return PreSetTag(serviceapi, request);
    } else if (request.request_function_ == "get_tag") {
      return RunUDF_GetTag(serviceapi, request);
    } else if (request.request_function_ == "clear_semantic") {
      semantic_schema = Json::Value();
      return true;
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
    UserSearchMain usm(serviceapi, request);
    return usm.RunUDF_UserSearch();
  }

  bool RunUDF_DeleteCrowdByName(ServiceAPI& serviceapi, EngineServiceRequest& request) {
    std::cout << "RunUDF_DeleteCrowdByName" << std::endl;
    Json::Value& jsoptions = request.jsoptions_;
    std::cout << jsoptions.toStyledString() << std::endl;

    //parse the request parameters
    std::string crowdName = jsoptions["name"][0].asString();
    uint32_t typeId = jsoptions["type"]["typeid"].asUInt();
    std::string typeStr = boost::lexical_cast<std::string>(typeId);

    VertexLocalId_t local_start;
    if (!serviceapi.UIdtoVId(request, typeStr + "_" + crowdName, local_start)) {
      request.error_ = true;
      request.message_ += std::string("valid id: ") + typeStr + "_" + crowdName ;
      return true;
    }

    //make sure the result is valid json format
    JSONWriter* writer_ = request.outputwriter_;
    writer_->WriteStartObject();
    writer_->WriteEndObject();

    //delete vertex
    boost::shared_ptr<GraphUpdates> graphupdates_ = serviceapi.CreateGraphUpdates(&request);;
    graphupdates_->DeleteVertex(local_start);
    graphupdates_->PostUpdate();

    return true;
  }

  bool RunUDF_GetCrowdByName(ServiceAPI& serviceapi, EngineServiceRequest& request) {
    std::cout << "RunUDF_GetCrowdByName" << std::endl;
    Json::Value& jsoptions = request.jsoptions_;
    std::cout << jsoptions.toStyledString() << std::endl;

    //parse the request parameters
    std::string crowdName = jsoptions["name"][0].asString();
    uint32_t typeId = jsoptions["type"]["typeid"].asUInt();
    uint32_t limit = std::numeric_limits<uint32_t>::max();  
    if (jsoptions.isMember("limit") && jsoptions["limit"].size() > 0) {
      limit = jsoptions["limit"][0].asUInt();
    }
    //request id
    std::string rid; 
    if (jsoptions.isMember("rid") && jsoptions["rid"].size() > 0) {
      rid = jsoptions["rid"][0].asString();
    }

    std::string typeStr = boost::lexical_cast<std::string>(typeId);
    VertexLocalId_t local_start;
    if (!serviceapi.UIdtoVId(request, typeStr + "_" + crowdName, local_start)) {
      request.error_ = true;
      request.message_ += std::string("valid id: ") + typeStr + "_" + crowdName ;
      return true;
    }

    //get all userIds for this crowd
    std::set<VertexLocalId_t> userIds;
    boost::shared_ptr<gapi4::GraphAPI> graphapi = serviceapi.CreateGraphAPI(&request);
    gapi4::EdgesCollection edges;
    graphapi->GetOneVertexEdges(local_start, NULL, edges);
    while (edges.NextEdge()) {
      VertexLocalId_t vid = edges.GetCurrentToVId();
      userIds.insert(vid);
    }

    //output the final resutls
    JSONWriter* writer_ = request.outputwriter_;
    writer_->WriteStartObject();
    if (rid.length() > 0) {
      writer_->WriteName("requestId");
      writer_->WriteString(rid);
    }
    writer_->WriteName("count");
    writer_->WriteUnsignedInt(userIds.size());
    writer_->WriteName("userIds");
    writer_->WriteStartArray();
    for (std::set<VertexLocalId_t>::iterator it = userIds.begin(); 
            limit > 0 && it != userIds.end(); ++it) {
      --limit;
      writer_->WriteMarkVId(*it);
      request.output_idservice_vids.push_back(*it);
    }
    writer_->WriteEndArray();
    writer_->WriteEndObject();
    return true;
  }

  bool SemanticDef(ServiceAPI &serviceapi, EngineServiceRequest &request) {
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
        std::set<std::string> uniq;
        Json::Value onto;
        int size = payload[ONTO].size();
        for (int i = 0; i < size; ++i) {
          std::string name(payload[ONTO][i].asString());
          if (uniq.find(name) != uniq.end()) {
            continue;
          }
          uniq.insert(name);

          Json::Value one;
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
            if (uniq.find(name) == uniq.end()) {
              request.error_ = true;
              request.message_ += name + " not found in ontology.";
              return false;
            }
            Json::Value two;
            two["name"] = name;
            two["etype"] = OBJ_ONTO_ETYPE_PREF + obj + "_to_" + name;
            one["ontology"].append(two);
          }
          obj_onto.append(one);
        }
        payload[OBJ_ONTO] = obj_onto;

        // create crowdIndex v/etype for crowding service
        payload[PROF][CROWD_INDEX]["vtype"] = CROWD_INDEX_VTYPE;
        payload[PROF][CROWD_INDEX]["etype"] = USER_CROWD_INDEX_ETYPE;
      }
    } else {
      request.error_ = true;
      request.message_ += ONTO + " or " + OBJ_ONTO + " missing.";
      return false;
    }

    // check for "profile", should be present
    if (payload.isMember(PROF)) {
    } else {
      request.error_ = true;
      request.message_ += PROF + " missing or not array.";
      return false;
    }

    // diff old/new version of semantic def, generate dynamic schema change job
    Json::Value delta;
    DiffSemanticSchema(semantic_schema, payload, delta);
    std::ofstream diff_fp(SCHEMA_DIFF_PATH.c_str());
    diff_fp << delta;
    diff_fp.close();

    if (delta.isNull()) {
      semantic_schema = payload;
    }

    // persist the old semantic schema to disk
    std::ofstream fp0(SEMANTIC_SCHEMA_PATH.c_str());
    fp0 << semantic_schema;
    fp0.close();

    // persist the new semantic schema to disk
    std::ofstream fp1((SEMANTIC_SCHEMA_PATH + ".rc").c_str());
    fp1 << payload;
    fp1.close();

    std::cout << "before system call" << std::endl;
    // trigger dynamic schema change job (external script)
    // generate/run ddl job via an external script.
    // if sc job is good, the script will replace old schema file with the new one
    if (! delta.isNull()) {
      const std::string cmd(SCHEMA_CHANGE_SCRIPT_PATH + " " 
          + SCHEMA_DIFF_PATH + " "
          + SEMANTIC_SCHEMA_PATH + " "
          + SEMANTIC_SCHEMA_PATH + ".rc");
      std::cout << "cmd: " << cmd << std::endl;
      int rez = system(cmd.c_str());
      if (rez != 0) {
        request.error_ = true;
        request.message_ += "fail to do schema change, error code is " + 
              boost::lexical_cast<std::string>(rez);
        std::cout << "error: " << strerror(errno) << std::endl;
        return false;
      }
    }
    std::cout << "done system call" << std::endl;

    // code below is useless
    // once schema change, reload graph meta
    LoadGraphMeta(serviceapi);

    return true;
  }

  bool DiffSemanticSchema(const Json::Value &old_schema, 
      const Json::Value &new_schema, 
      Json::Value &delta) {
    // diff ontology
    typedef std::map<std::string, Json::Value> map_t;
    map_t old_onto;
    map_t new_onto;
    const Json::Value &onto0 = old_schema[ONTO];
    const Json::Value &onto1 = new_schema[ONTO];

    int size0 = onto0.size();
    for (int i = 0; i < size0; ++i) {
      old_onto[onto0[i]["name"].asString()] = onto0[i];
    }
    int size1 = onto1.size();
    for (int i = 0; i < size1; ++i) {
      new_onto[onto1[i]["name"].asString()] = onto1[i];
    }

    for (map_t::iterator it = old_onto.begin(); it != old_onto.end(); ++it) {
      if (new_onto.find(it->first) != new_onto.end()) {
        continue;
      }
      Json::Value one, two;
      one["vtype"] = it->second["vtype"].asString();
      delta["drop"]["vertex"].append(one);

      one.clear();
      one["etype"] = it->second["etype"]["up"].asString();
      delta["drop"]["edge"].append(one);

      one.clear();
      one["etype"] = it->second["etype"]["down"].asString();
      delta["drop"]["edge"].append(one);
    }

    for (map_t::iterator it = new_onto.begin(); it != new_onto.end(); ++it) {
      if (old_onto.find(it->first) != old_onto.end()) {
        continue;
      }
      Json::Value one, two;
      one["vtype"] = it->second["vtype"].asString();
      two["name"] = "name";
      two["dtype"] = "string";
      one["attr"].append(two);
      delta["add"]["vertex"].append(one);

      one.clear();
      one["etype"] = it->second["etype"]["up"].asString();
      one["directed"] = true;
      one["source_vtype"] = it->second["vtype"].asString();
      one["target_vtype"] = it->second["vtype"].asString();
      delta["add"]["edge"].append(one);

      one.clear();
      one["etype"] = it->second["etype"]["down"].asString();
      one["directed"] = true;
      one["source_vtype"] = it->second["vtype"].asString();
      one["target_vtype"] = it->second["vtype"].asString();
      delta["add"]["edge"].append(one);
    }

    std::cout << "done ontology diff\n";

    // diff object-ontology
    map_t old_obj_onto;
    map_t new_obj_onto;
    const Json::Value &obj_onto0 = old_schema[OBJ_ONTO];
    const Json::Value &obj_onto1 = new_schema[OBJ_ONTO];

    std::cout << obj_onto0;
    std::cout << obj_onto1;

    size0 = obj_onto0.size();
    for (int i = 0; i < size0; ++i) {
      old_obj_onto[obj_onto0[i]["object"].asString()] = obj_onto0[i]["ontology"];
    }
    size1 = obj_onto1.size();
    for (int i = 0; i < size1; ++i) {
      new_obj_onto[obj_onto1[i]["object"].asString()] = obj_onto1[i]["ontology"];
    }

    // scan old object-ontology
    for (map_t::iterator it = old_obj_onto.begin(); it != old_obj_onto.end(); ++it) {
      // both of old & new contains such object, then diff object's ontology
      if (new_obj_onto.find(it->first) != new_obj_onto.end()) {
        map_t onto0, onto1;
        const Json::Value &js_onto0 = it->second;
        const Json::Value &js_onto1 = new_obj_onto[it->first];
        int size0 = js_onto0.size();
        int size1 = js_onto1.size();
        for (int i = 0; i < size0; ++i) {
          onto0[js_onto0[i]["name"].asString()] = js_onto0[i];
        }
        for (int i = 0; i < size1; ++i) {
          onto1[js_onto1[i]["name"].asString()] = js_onto1[i];
        }

        for (map_t::iterator it1 = onto0.begin(); it1 != onto0.end(); ++it1) {
          if (onto1.find(it1->first) != onto1.end()) {
            continue;
          }
          Json::Value one;
          one["etype"] = it1->second["etype"].asString();
          delta["drop"]["edge"].append(one);
        }
        for (map_t::iterator it1 = onto1.begin(); it1 != onto1.end(); ++it1) {
          if (onto0.find(it1->first) != onto0.end()) {
            continue;
          }
          Json::Value one, two;
          one["etype"] = it1->second["etype"].asString();
          one["directed"] = false;
          one["source_vtype"] = it->first;
          one["target_vtype"] = new_onto[it1->first]["vtype"].asString();
          two["name"] = "weight";
          two["dtype"] = "float";
          two["default"] = "1.0";
          one["attr"].append(two);
          delta["add"]["edge"].append(one);
        }
      } else {
        // otherwise, drop all edges in old
        const Json::Value &js_onto0 = it->second;
        int size0 = js_onto0.size();
        for (int i = 0; i < size0; ++i) {
          Json::Value one;
          one["etype"] = js_onto0[i]["etype"].asString();
          delta["drop"]["edge"].append(one);
        }
      }
    }

    // scan new object-ontology
    for (map_t::iterator it = new_obj_onto.begin(); it != new_obj_onto.end(); ++it) {
      if (old_obj_onto.find(it->first) != old_obj_onto.end()) {
        // this object has already been handled in old object-ontology scan
      } else {
        // otherwise, add all edges in new
        const Json::Value &js_onto1 = it->second;
        int size1 = js_onto1.size();
        for (int i = 0; i < size1; ++i) {
          Json::Value one, two;
          one["etype"] = js_onto1[i]["etype"].asString();
          one["directed"] = false;
          one["source_vtype"] = it->first;
          one["target_vtype"] = new_onto[js_onto1[i]["name"].asString()]["vtype"].asString();
          two["name"] = "weight";
          two["dtype"] = "float";
          two["default"] = "1.0";
          one["attr"].append(two);
          delta["add"]["edge"].append(one);
        }
      }
    }

    // diff profile[crowdIndex], this should be done just once
    const Json::Value &prof0 = old_schema[PROF];
    const Json::Value &prof1 = new_schema[PROF];

    if ((! prof0.isMember("crowdIndex")) && prof1.isMember("crowdIndex")) {
      Json::Value one;
      one["vtype"] = prof1["crowdIndex"]["vtype"].asString();
      delta["add"]["vertex"].append(one);

      one.clear();
      one["etype"] = prof1["crowdIndex"]["etype"].asString();
      one["directed"] = false;
      one["source_vtype"] = prof1["target"].asString();
      one["target_vtype"] = prof1["crowdIndex"]["vtype"].asString();
      delta["add"]["edge"].append(one);
    }

    std::cout << "Delta: " << delta;
    return true;
  }

  bool ValidateObjectOntology(const std::string &obj, const std::string &name) {
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
    return valid;
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
    if (! ValidateObjectOntology(obj, name)) {
      request.error_ = true;
      request.message_ += "(" + obj + ", " + name + ") not found in " + OBJ_ONTO;
      return false;
    }

    // TODO(@alan): replaced by GetOntologyVEType
    // get vtype/etype for ontology tree
    const Json::Value &onto = semantic_schema[ONTO];
    int size = onto.size();
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

  int GetObjectOntologyEType(const std::string &obj, const std::string &name, std::string &etype) {
    // get etype of obj-ontology
    std::cout << semantic_schema << std::endl;
    const Json::Value &obj_onto = semantic_schema[OBJ_ONTO];
    int size = obj_onto.size();
    for (int i = 0; i < size; ++i) {
      if (obj_onto[i]["object"].asString() == obj) {
        const Json::Value &onto = obj_onto[i]["ontology"];
        int size1 = onto.size();
        for (int j = 0; j < size1; ++j) {
          if (onto[j]["name"].asString() == name) {
            etype = onto[j]["etype"].asString();
            return 0;
          }
        }
      }
    }
    return -1;
  }

  int GetOntologyNameByObject(const std::string &obj, std::set<std::string> &rez) {
    const Json::Value &obj_onto = semantic_schema[OBJ_ONTO];
    int size = obj_onto.size();
    for (int i = 0; i < size; ++i) {
      if (obj_onto[i]["object"].asString() == obj) {
        int size1 = obj_onto[i]["ontology"].size();
        for (int j = 0; j < size1; ++j) {
          rez.insert(obj_onto[i]["ontology"][j]["name"].asString());
        }
        return 0;
      }
    }
    return -1;
  }

  int GetOntologyNameByObject(const std::string &obj, Json::Value &rez) {
    const Json::Value &obj_onto = semantic_schema[OBJ_ONTO];
    int size = obj_onto.size();
    for (int i = 0; i < size; ++i) {
      if (obj_onto[i]["object"].asString() == obj) {
        rez = obj_onto[i];
        return 0;
      }
    }
    return -1;
  }

  int GetOntologyNameByVType(const std::string &vtype, std::string &name) {
    const Json::Value &onto = semantic_schema[ONTO];
    int size = onto.size();
    for (int i = 0; i < size; ++i) {
      if (onto[i]["vtype"].asString() == vtype) {
        name = onto[i]["name"].asString();
        return 0;
      }
    }
    return -1;
  }

  bool RunUDF_GetOntology(ServiceAPI& serviceapi,
                EngineServiceRequest& request,
                const std::set<std::string> *save = NULL,
                std::map<VertexLocalId_t, std::vector<VertexLocalId_t> > *out = NULL) {
    if (! request.jsoptions_.isMember("name")) {
      request.error_ = true;
      request.message_ += "name missing.";
      return false;
    }

    JSONWriter* writer_ = request.outputwriter_;
    int size = request.jsoptions_["name"].size();
    size_t threshold = request.jsoptions_["threshold"][0].asUInt64();

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

      uint32_t vtype_id = vertex_type_map[rez["vtype"]];
      uint32_t down_etype_id = edge_type_map[rez["down_etype"]];

      // run udf to get the tree
      typedef ExportOntologyTree UDF_t;

      tree.clear();
      bool large = false;
      UDF_t udf(1, vtype_id, down_etype_id, threshold, tree, large);
      serviceapi.RunUDF(&request, &udf);

      bool flag = false;
      if ((save != NULL) && (save->find(name) != save->end())) {
        flag = true;
      }

      writer_->WriteStartObject();
      writer_->WriteName("name");
      writer_->WriteString(name);
      writer_->WriteName("vtype");
      writer_->WriteString(rez["vtype"]);
      writer_->WriteName("etype");
      writer_->WriteStartObject();
      writer_->WriteName("up");
      writer_->WriteString(rez["up_etype"]);
      writer_->WriteName("down");
      writer_->WriteString(rez["down_etype"]);
      writer_->WriteEndObject();

      writer_->WriteName("large");
      writer_->WriteBool(large);
      writer_->WriteName("tree");
      writer_->WriteStartArray();
      for (tree_t::iterator it = tree.begin(); it != tree.end(); ++it) {
        writer_->WriteStartObject();
        writer_->WriteName("parent");
        writer_->WriteMarkVId(it->first);
        request.output_idservice_vids.push_back(it->first);

        writer_->WriteName("children");
        writer_->WriteStartArray();
        for (std::vector<VertexLocalId_t>::iterator it1 = it->second.begin();
             it1 != it->second.end(); ++it1) {
          writer_->WriteMarkVId(*it1);
          request.output_idservice_vids.push_back(*it1);

          if ((out != NULL) && flag) {
            (*out)[it->first].push_back(*it1);
          }
        }
        writer_->WriteEndArray();
        writer_->WriteEndObject();
      }
      writer_->WriteEndArray();
      writer_->WriteEndObject();
    }
    writer_->WriteEndArray();
    return true;
  }

  // find all entities in profile,
  // return all ontologies of entities as well as behaviour def
  bool RunUDF_GetProfile(ServiceAPI& serviceapi,
                            EngineServiceRequest& request) {
    int threshold = request.jsoptions_["threshold"][0].asUInt64();

    const Json::Value &prof = semantic_schema[PROF];
    std::set<std::string> obj;

    JSONWriter *writer = request.outputwriter_;
    writer->WriteStartObject();

    // collect all entities in profile
    if (! prof.isMember("target")) {
      request.error_ = true;
      request.message_ += "target missing.";
      return false;
    }
    obj.insert(prof["target"].asString());

    // get target
    writer->WriteName("target");
    writer->WriteString(prof["target"].asString());

    // get crowdIndex
    if (! prof.isMember("crowdIndex")) {
      request.error_ = true;
      request.message_ += "crowdIndex missing.";
      return false;
    }
    writer->WriteName("crowdIndex");
    writer->WriteStartObject();
    writer->WriteName("vtype");
    writer->WriteString(prof["crowdIndex"]["vtype"].asString());
    writer->WriteName("etype");
    writer->WriteString(prof["crowdIndex"]["etype"].asString());
    writer->WriteEndObject();

    if (prof.isMember("behaviour")) {
      const Json::Value &beh = prof["behaviour"];
      int size = beh.size();
      for (int i = 0; i < size; ++i) {
        if (beh[i].isMember("object")) {
          int size1 = beh[i]["object"].size();
          for (int j = 0; j < size1; ++j) {
            obj.insert(beh[i]["object"][j]["name"].asString());
          }
        }
      }
    }

    // get ontologies of all entities
    std::set<std::string> onto;
    for (std::set<std::string>::iterator it = obj.begin();
        it != obj.end(); ++it) {
      GetOntologyNameByObject(*it, onto);
    }

    // get obj-ontology
    writer->WriteName("object_ontology");
    writer->WriteStartArray();
    const Json::Value &obj_onto = semantic_schema[OBJ_ONTO];
    for (std::set<std::string>::iterator it = obj.begin();
        it != obj.end(); ++it) {
      Json::Value one;

      if (GetOntologyNameByObject(*it, one) == 0) {
        writer->WriteStartObject();
        writer->WriteName("object");
        writer->WriteString(*it);
        writer->WriteName("ontology");
        std::string s(one["ontology"].toStyledString());
        writer->WriteJSONContent(s);
        writer->WriteEndObject();
      }
    }
    writer->WriteEndArray();

    // get ontology trees
    Json::Value jsoptions;
    for (std::set<std::string>::iterator it = onto.begin();
        it != onto.end(); ++it) {
      jsoptions["name"].append(*it);
    }
    jsoptions["threshold"].append(threshold);
    request.jsoptions_ = jsoptions;

    // since "tag" ontology needs more peculiar treatment, so save it
    const std::string tag_onto_name("tag");
    std::set<std::string> save;
    save.insert(tag_onto_name);
    std::map<VertexLocalId_t, std::vector<VertexLocalId_t> > tree;

    writer->WriteName("ontology");
    RunUDF_GetOntology(serviceapi, request, &save, &tree);

    // get interest_indent
    const Json::Value &interest_prof = prof["interest_intent"];
    writer->WriteName("interest_intent");
    writer->WriteStartArray();
    int interest_size = interest_prof.size();
    for (int i = 0; i < interest_size; ++i) {
      writer->WriteStartObject();
      writer->WriteName("ontology");
      writer->WriteString(interest_prof[i]["ontology"].asString());
      writer->WriteEndObject();
    }
    writer->WriteEndArray();

    // get tag
    const Json::Value &tag_prof = prof["tag"];
    writer->WriteName("tag");
    writer->WriteStartArray();

    std::map<std::string, std::string> vetype;
    if (GetOntologyVEType(tag_onto_name, vetype) != 0) {
      std::cout << "fail to get vetype for " + tag_onto_name << std::endl;
    } else {
      std::vector<std::pair<std::string, std::string> > uids;
      int size = tag_prof.size();
      // find internal vid of each item in profile's "tag"
      for (int i = 0; i < size; ++i) {
        uids.push_back(std::pair<std::string, std::string>(vetype["vtype"], 
              tag_prof[i]["name"].asString()));
      }
      std::vector<VertexLocalId_t> vids = serviceapi.UIdtoVId(uids);
      std::map<std::string, VertexLocalId_t> uid_to_vid;
      for (int i = 0; i < size; ++i) {
        if (vids[i] == (VertexLocalId_t)-1) {
          continue;
        }
        uid_to_vid[uids[i].second] = vids[i];
      }

      for (int i = 0; i < size; ++i) {
        // find the children of level-1 tag
        std::string name(tag_prof[i]["name"].asString());

        writer->WriteStartObject();
        writer->WriteName("name");
        writer->WriteString(name);

        std::string dtype = tag_prof[i]["datatype"].asString();
        writer->WriteName("datatype");
        writer->WriteString(dtype);

        writer->WriteName("vtype");
        writer->WriteString(vetype["vtype"]);

        writer->WriteName("element");
        writer->WriteStartArray();

        if (uid_to_vid.find(name) != uid_to_vid.end()) {
          VertexLocalId_t vid = uid_to_vid[name];
          if (tree.find(vid) != tree.end()) {
            const std::vector<VertexLocalId_t> &children = tree[vid];

            int size = children.size();
            for (int j = 0; j < size; ++j) {
              writer->WriteMarkVId(children[j]);
            }
          }
        }
        writer->WriteEndArray();
        writer->WriteEndObject();
      }
    }

    writer->WriteEndArray();

    // get behaviour meta
    writer->WriteName("behaviour");
    const Json::Value &beh = prof["behaviour"];
    std::string s(beh.toStyledString()); 
    writer->WriteJSONContent(s);

    writer->WriteEndObject();
    return true;
  }

  bool GetObjectOntologyVEType(const std::string &obj, const std::string &name,
      std::map<std::string, std::string> &rez) {
    // validate (obj, name) pair
    if (! ValidateObjectOntology(obj, name)) {
      std::cout << "(" + obj + ", " + name + ") not found in " + OBJ_ONTO << std::endl;
      return false;
    }

    // obj-ontology etype
    std::string etype;
    if (GetObjectOntologyEType(obj, name, etype) != 0) {
      std::cout << "fail to get object-ontology etype" << std::endl;
      return false;
    }
    rez["object_ontology_etype"] = etype;

    // ontolgy tag-tag etype/vtype
    if (GetOntologyVEType(name, rez) != 0) {
      std::cout << name + " not found in " + ONTO << std::endl;
      return false;
    }

    return true;
  }

  // TODO(@alan): obsolete
  bool GetObjectOntologyVEType(EngineServiceRequest& request,
      std::map<std::string, std::string> *out = NULL) {
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
    if (! ValidateObjectOntology(obj, name)) {
      request.error_ = true;
      request.message_ += "(" + obj + ", " + name + ") not found in " + OBJ_ONTO;
      return false;
    }

    std::map<std::string, std::string> rez;
    // obj-ontology etype
    std::string etype;
    if (GetObjectOntologyEType(obj, name, etype) != 0) {
      request.message_ += "fail to get object-ontology etype";
      return false;
    }
    rez["object_ontology_etype"] = etype;

    // ontolgy tag-tag etype/vtype
    if (GetOntologyVEType(name, rez) != 0) {
      request.message_ += name + " not found in " + ONTO;
      return false;
    }

    request.outputwriter_->WriteStartObject();

    request.outputwriter_->WriteName("object_ontology");
    request.outputwriter_->WriteStartObject();
    request.outputwriter_->WriteName("etype");
    request.outputwriter_->WriteString(rez["object_ontology_etype"]);
    request.outputwriter_->WriteEndObject();

    request.outputwriter_->WriteName("ontology");
    request.outputwriter_->WriteStartObject();
    request.outputwriter_->WriteName("vtype");
    request.outputwriter_->WriteString(rez["vtype"]);
    request.outputwriter_->WriteName("etype");
    request.outputwriter_->WriteStartObject();
    request.outputwriter_->WriteName("up");
    request.outputwriter_->WriteString(rez["up_etype"]);
    request.outputwriter_->WriteName("down");
    request.outputwriter_->WriteString(rez["down_etype"]);
    request.outputwriter_->WriteEndObject();
    request.outputwriter_->WriteEndObject();

    request.outputwriter_->WriteEndObject();

    if (out != NULL) {
      *out = rez;
    }

    return true;
  }

  bool PreSetTag(ServiceAPI& serviceapi,
                          EngineServiceRequest& request) {
    const Json::Value &jsoptions = request.jsoptions_;
    if (! (jsoptions.isMember("object") && jsoptions.isMember("name"))) {
      request.error_ = true;
      request.message_ += "object or name missing.";
      return false;
    }
    const std::string obj(jsoptions["object"][0].asString());
    const std::string name(jsoptions["name"][0].asString());

    JSONWriter *writer = request.outputwriter_;
    writer->WriteStartObject();

    // first get v/etype
    std::map<std::string, std::string> vetype;
    if (! GetObjectOntologyVEType(obj, name, vetype)) {
      request.error_ = true;
      request.message_ += "fail to get vetype.";
      return false;
    }

    writer->WriteName("object_ontology");
    writer->WriteStartObject();
    writer->WriteName("etype");
    writer->WriteString(vetype["object_ontology_etype"]);
    writer->WriteEndObject();

    writer->WriteName("ontology");
    writer->WriteStartObject();
    writer->WriteName("vtype");
    writer->WriteString(vetype["vtype"]);
    writer->WriteName("etype");
    writer->WriteStartObject();
    writer->WriteName("up");
    writer->WriteString(vetype["up_etype"]);
    writer->WriteName("down");
    writer->WriteString(vetype["down_etype"]);
    writer->WriteEndObject();
    writer->WriteEndObject();

    // then find tags' primary id if exist in graph
    if (jsoptions.isMember("tags")) {
      // try to find each tag's primary id using name, i.e. tag == primary_id.name
      if (refresh_inverted_tag_cache) {
        typedef LocateTagUDF UDF_t;
        std::vector<UDF_t::tag_t> tag_vec;
        uint32_t vtype_id = vertex_type_map[vetype["vtype"]];
        UDF_t udf(1, vtype_id, tag_vec);
        serviceapi.RunUDF(&request, &udf);
  
        // cache the inverted tag->path
        inverted_tag_cache.clear();
        for (std::vector<UDF_t::tag_t>::iterator it = tag_vec.begin();
            it != tag_vec.end(); ++it) {
          inverted_tag_cache[it->name].push_back(it->id);
        }
        refresh_inverted_tag_cache = false;
      }

      // try to convert each tag to internal primary id, i.e. tag == primary_id
      int size = jsoptions["tags"].size();
      std::set<std::string> uniq;
      std::vector<std::pair<std::string, std::string> > uids;
      for (int i = 0; i < size; ++i) {
        const std::string &t = jsoptions["tags"][i].asString();
        if (uniq.find(t) != uniq.end()) {
          continue;
        }
        uids.push_back(std::pair<std::string, std::string>(vetype["vtype"], t));
      }
      std::vector<VertexLocalId_t> vids = serviceapi.UIdtoVId(uids);

      // merge vid_found into inverted_tag_cache
      size = uids.size();
      std::cout << "merge\n";
      for (int i = 0; i < size; ++i) {
        if (vids[i] == (VertexLocalId_t)-1) {
          std::cout << uids[i].second << " fail to convert\n";
          continue;
        }
        inverted_tag_cache[uids[i].second].clear();
        inverted_tag_cache[uids[i].second].push_back(vids[i]);
        std::cout << uids[i].second << ", " << vids[i] << std::endl;
      }

      std::cout << "cache.size = " << inverted_tag_cache.size() << std::endl;
      std::cout << jsoptions["tags"] << std::endl;

      writer->WriteName("inverted_tags");
      writer->WriteStartObject();

      uniq.clear();
      size = jsoptions["tags"].size();
      std::map<std::string, std::vector<VertexLocalId_t> >::iterator it;
      for (int i = 0; i < size; ++i) {
        const std::string &t = jsoptions["tags"][i].asString();
        if (uniq.find(t) != uniq.end()) {
          continue;
        }
        it = inverted_tag_cache.find(t);
        if (it != inverted_tag_cache.end()) {
          writer->WriteName(t.c_str());
          writer->WriteStartArray();
          int size1 = it->second.size();
          for (int j = 0; j < size1; ++j) {
            writer->WriteMarkVId(it->second[j]);
            request.output_idservice_vids.push_back(it->second[j]);
          }
          writer->WriteEndArray();
          uniq.insert(t);
        } else {
          // FIXME: when system is up, we should load all tags for once,
          // later, if new tags come in, they will be inserted or not (depends on `more`),
          // but we will not reload the tags, since it might be very time-consuming.
          //refresh_inverted_tag_cache = true;
        }
      }
      writer->WriteEndObject();

    }
    writer->WriteEndObject();
    return true;
  }

  bool RunUDF_GetTag(ServiceAPI& serviceapi,
                            EngineServiceRequest& request) {
    const Json::Value &jsoptions = request.jsoptions_;
    const std::string &obj = jsoptions["object"]["typename"].asString();
    if (vertex_type_map.find(obj) == vertex_type_map.end()) {
      request.error_ = true;
      request.message_ += obj + " not found in graph meta";
      return false;
    }
    uint32_t vtype_id = vertex_type_map[obj];

    const std::string &id = boost::lexical_cast<std::string>(vtype_id) +
      "_" +jsoptions["id"][0].asString();
    VertexLocalId_t start;
    if (! serviceapi.UIdtoVId(request, id, start)) {
      request.error_ = true;
      request.message_ += "fail to get vertex local id, " + id;
      return false;
    }

    std::set<std::string> names;
    int size = jsoptions["name"].size();
    if (size == 0) {
      GetOntologyNameByObject(obj, names);
    } else {
      for (int i = 0; i < size; ++i) {
        names.insert(jsoptions["name"][i].asString());
      }
    }

    std::set<uint32_t> etype_id;
    for (std::set<std::string>::iterator it = names.begin();
        it != names.end(); ++it) {
      std::string etype;
      if (GetObjectOntologyEType(obj, *it, etype) != 0) {
        request.error_ = true;
        request.message_ += "fail to get object-ontology etype";
        return false;
      }
      if (edge_type_map.find(etype) == edge_type_map.end()) {
        request.error_ = true;
        request.message_ += etype + " not found in graph meta";
        return false;
      }
      etype_id.insert(edge_type_map[etype]);
    }

    typedef GetTagUDF UDF_t;
    typedef std::vector<UDF_t::tag_t> tag_vec_t;

    tag_vec_t tags;
    UDF_t udf(1, etype_id, start, tags);
    serviceapi.RunUDF(&request, &udf);

    // ontology vtypeid -> tags
    std::map<uint32_t, tag_vec_t> tags_by_onto;
    for (tag_vec_t::iterator it = tags.begin(); it != tags.end(); ++it) {
      tags_by_onto[it->vtype_id].push_back(*it);
    }

    bool verbose = jsoptions["verbose"][0].asBool();

    JSONWriter *writer = request.outputwriter_;
    writer->WriteStartObject();
    for (std::map<uint32_t, tag_vec_t>::iterator it = tags_by_onto.begin();
        it != tags_by_onto.end(); ++it) {
      if (vertex_type_reverse_map.find(it->first) == vertex_type_reverse_map.end()) {
        request.error_ = true;
        request.message_ += it->first + " not found in reverse vertex map of graph meta";
        return false;
      }
      std::string name;
      GetOntologyNameByVType(vertex_type_reverse_map[it->first], name);
      writer->WriteName(name.c_str());
      writer->WriteStartArray();
      for (tag_vec_t::iterator it1 = it->second.begin(); it1 != it->second.end(); ++it1) {
        writer->WriteStartObject();
        writer->WriteName("tag");
        writer->WriteString(it1->name);
        if (verbose) {
          writer->WriteName("weight");
          writer->WriteFloat(it1->weight);
        }
        writer->WriteEndObject();
      }
      writer->WriteEndArray();
    }
    writer->WriteEndObject();
    return true;
  }

  bool AutoTag(ServiceAPI& serviceapi,
                EngineServiceRequest& request) {
    return true;
  }

};
}  // namespace UDIMPL
#endif  // SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_

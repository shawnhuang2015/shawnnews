/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: REST++
 * Brief: template for writing an endpoint filter.
 *
 * Created on: Feb 5, 2015
 *      Author: jzhong
 ******************************************************************************/

/******************************************************************************
 * Do NOT edit on this file. Follow this file to create you own filters in a 
 * new file. Directly editting on this file may lead to merge conflicts with
 * poc4.3_base in the future.
 ******************************************************************************/

#include <restpp/librestpp.hpp>

#include <sstream>
#include <cstdio>
#include <map>
#include <string>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

  /*
 curl -X POST 'http://localhost:9000/user_search' -d '{"selector":[{"ontology":[{"type":"demographic","factor":"Age10~20","operator":"=","weight":"1.0","sequence":1},{"type":"demographic","factor":"Male","operator":"=","weight":"1.0","sequence":2},{"type":"interest","factor":"shopping/shows","operator":">=","weight":"0.5","sequence":3},{"type":"interest","factor":"interest/TV","operator":"<","weight":"0.8","sequence":4}],"behavior":[{"action":"browse","objectCategory":"theme","objectType":"Item","ontologyType":"","objectId":"12345","operator":">","value":18,"startTime":1,"endTime":3,"timeType":"absolute","sequence":5},{"action":"browse","objectCategory":"theme","objectType":"Item","ontologyType":"","objectId":"54321","operator":">","value":18,"startTime":1,"endTime":3,"timeType":"absolute","sequence":5},{"action":"browse","objectCategory":"theme","objectType":"Category","ontologyType":"pic_tag","objectId":"shoes","operator":"<","value":20,"startTime":123,"endTime":456,"timeType":"relative","sequence":6}]}]}'
   */
  std::string errorMsg(std::string msg) {
    Json::Value root;
    root["error"] = true;
    root["message"] = msg;
    return root.toStyledString();
  }
  // Just put payload into jsoptions such that GPE could access it.
  void UserSearchFilter(
           FilterHelper *filter_helper,
           UserRequest *user_request,
           GsqlRequest *gsql_request) {
    std::string target_str = "target";
    std::string selector_str = "selector";
    std::string ontology_str = "ontology";
    std::string behavior_str = "behavior";
    std::string type_str = "type";
    std::string factor_str = "factor";
    std::string objectType_str = "objectType";
    std::string objectCategory_str = "objectCategory";
    std::string ontologyType_str = "ontologyType";
    std::string objectId_str = "objectId";
    std::string is_sub_str = "is_sub";

    try {
      if (user_request->data_length == 0) {
        gsql_request->Respond(errorMsg("payload missing"));
        return;
      } else {
        std::string payload(user_request->data, user_request->data_length);

        Json::Value &jsoptions = gsql_request->jsoptions;
        Json::Value root;
        Json::Reader reader;

        if (reader.parse(payload, root)) {
          Json::Value& selector = root[selector_str];
          for (uint32_t idx = 0; idx < selector.size(); ++idx) {
            Json::Value& ontology = selector[idx][ontology_str];
            Json::Value& behavior = selector[idx][behavior_str];
            for (uint32_t k = 0; k < ontology.size(); ++k) {
              std::string type = ontology[k][type_str].asString();
              std::string fact = ontology[k][factor_str].asString();
              bool is_sub = ontology[k][is_sub_str].asBool();
              if (is_sub) continue;
              gsql_request->AddId(type, fact);
            }
            for (uint32_t k = 0; k < behavior.size(); ++k) {
              std::string enumStr = behavior[k][objectType_str].asString();
              bool is_sub = ontology[k][is_sub_str].asBool();
              if (is_sub) continue;
              if (enumStr == "Item") {
                std::string itemT = behavior[k][objectCategory_str].asString();
                std::string itemId = behavior[k][objectId_str].asString();
                gsql_request->AddId(itemT, itemId);
              } else if (enumStr == "Category") {
                std::string ontoT = behavior[k][ontologyType_str].asString();
                std::string ontoId = behavior[k][objectId_str].asString();
                gsql_request->AddId(ontoT, ontoId);
              } else {
                gsql_request->Respond(errorMsg("payload invalid"));
                return;
              }
            }
          }
          jsoptions[target_str] = root[target_str];
          jsoptions[selector_str] = root[selector_str];
        } else {
          gsql_request->Respond(errorMsg("payload invalid"));
        }
      }
    } catch (std::exception &e) {
      gsql_request->Respond(errorMsg(std::string("Exception: ") + e.what()));
    }
  }

#ifdef __cplusplus
}
#endif



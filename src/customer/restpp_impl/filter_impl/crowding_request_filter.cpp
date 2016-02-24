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
   curl -X POST 'http://localhost:9000/user_search?limit=1000' -d '{"selector":[{"ontology":[{"type":"demographic","factor":"Agebetween10and20","operator":">=","weight":"0.5","sequence":1},{"type":"interest","factor":"shopping/shows","operator":">=","weight":"0.5","sequence":3}],"behavior":[{"action":"buy","itemType":"item","itemId":"12345","cateType":"cateType","cateId":"cateId","operator":">","value":18,"startTime":1,"endTime":3,"timeType":"absolute","sequence":5},{"action":"click","itemType":"category","itemId":"shoes","cateType":"cateType","cateId":"cateId","operator":"<","value":20,"startTime":123,"endTime":456,"timeType":"relative","sequence":6}]}]}'
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
    std::string selector_str = "selector";
    std::string ontology_str = "ontology";
    std::string behavior_str = "behavior";
    std::string type_str = "type";
    std::string factor_str = "factor";
    std::string itemT_str = "itemType";
    std::string itemId_str = "itemId";
    std::string cateT_str = "cateType";
    std::string cateId_str = "cateId";

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
              gsql_request->AddId(type, fact);
            }
            for (uint32_t k = 0; k < behavior.size(); ++k) {
              std::string itemType = behavior[k][itemT_str].asString();
              std::string itemId = behavior[k][itemId_str].asString();
              if (itemId.length() > 0) {
                gsql_request->AddId(itemType, itemId);
              }
              std::string cateType = behavior[k][cateT_str].asString();
              std::string cateId = behavior[k][cateId_str].asString();
              if (cateId.length() > 0) {
                gsql_request->AddId(cateT_str, cateId_str);
              }
            }
          }
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



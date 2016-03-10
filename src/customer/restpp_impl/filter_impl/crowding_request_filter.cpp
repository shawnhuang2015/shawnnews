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

  std::string errorMsg(std::string msg) {
    Json::Value root;
    root["error"] = true;
    root["message"] = msg;
    return root.toStyledString();
  }

  void searchCondParse(
          Json::Value &jsoptions, 
          Json::Value &condRoot,
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

    std::cout << condRoot[selector_str].toStyledString() << std::endl;
    Json::Value& selector = condRoot[selector_str];
    for (uint32_t idx = 0; idx < selector.size(); ++idx) {
      Json::Value& ontology = selector[idx][ontology_str];
      Json::Value& behavior = selector[idx][behavior_str];
      for (uint32_t k = 0; k < ontology.size(); ++k) {
        std::string type = ontology[k][type_str].asString();
        std::string fact = ontology[k][factor_str].asString();
        gsql_request->AddId(type, fact);
      }
      for (uint32_t k = 0; k < behavior.size(); ++k) {
        std::string enumStr = behavior[k][objectType_str].asString();
        if (enumStr == "Item") {
          std::string itemT = behavior[k][objectCategory_str].asString();
          std::string itemId = behavior[k][objectId_str].asString();
          gsql_request->AddId(itemT, itemId);
        } else if (enumStr == "Category") {
          std::string ontoT = behavior[k][ontologyType_str].asString();
          std::string ontoId = behavior[k][objectId_str].asString();
          gsql_request->AddId(ontoT, ontoId);
        } else if (enumStr == "Contains") {
          //substring
        } else {
          gsql_request->Respond(errorMsg("payload invalid"));
          return;
        }
      }
    }
    jsoptions[target_str] = condRoot[target_str];
    jsoptions[selector_str] = condRoot[selector_str];
    std::cout << condRoot[selector_str].toStyledString() << std::endl;
  }

  // Just put payload into jsoptions such that GPE could access it.
  void UserSearchFilter(
           FilterHelper *filter_helper,
           UserRequest *user_request,
           GsqlRequest *gsql_request) {
    try {
      if (user_request->data_length == 0) {
        gsql_request->Respond(errorMsg("payload missing"));
        return;
      } else {
        std::string payload(user_request->data, user_request->data_length);
        std::cout << "payload: " << payload << std::endl;

        Json::Value &jsoptions = gsql_request->jsoptions;
        Json::Value root;
        Json::Reader reader;

        if (reader.parse(payload, root)) {
          searchCondParse(jsoptions, root, gsql_request);
        } else {
          gsql_request->Respond(errorMsg("payload invalid"));
        }
      }
    } catch (std::exception &e) {
      gsql_request->Respond(errorMsg(std::string("Exception: ") + e.what()));
    }
  }

  void CreateCrowdRequestFilter(
          FilterHelper *filter_helper,
          UserRequest *user_request,
          GsqlRequest *gsql_request) {
      if (user_request->data_length == 0) {
        gsql_request->Respond(errorMsg("payload missing"));
        return;
      }

      std::string searchCond_str = "searchCond";
      try {
        std::string payload(user_request->data, user_request->data_length);

        Json::Value &jsoptions = gsql_request->jsoptions;
        Json::Value root;
        Json::Reader reader;
        if (reader.parse(payload, root)) {
          Json::Value& condRoot = root[searchCond_str];
          searchCondParse(jsoptions, condRoot, gsql_request);
        } else {
          gsql_request->Respond(errorMsg("payload invalid"));
        }

      } catch (std::exception &e) {
        gsql_request->Respond(errorMsg(std::string("Exception: ") + e.what()));
      }
  }

#ifdef __cplusplus
}
#endif



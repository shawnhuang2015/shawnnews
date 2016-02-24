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
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

#ifdef __cplusplus
extern "C" {
#endif

  typedef std::map<std::string, std::vector<std::string> > param_t;

  // Just put payload into jsoptions such that GPE could access it.
  void WrapPayloadFilter(
           FilterHelper *filter_helper,
           UserRequest *user_request,
           GsqlRequest *gsql_request) {
    if (user_request->data_length == 0) {
      std::string msg("Payload missing.");
      gsql_request->Respond(msg);
      return;
    }
    std::string payload(user_request->data, user_request->data_length);

    Json::Value &jsoptions = gsql_request->jsoptions;
    Json::Value js_payload;
    Json::Reader reader;

    if (reader.parse(payload, js_payload)) {
      jsoptions["payload"] = js_payload;
    } else {
      std::string msg("Payload is not Json.");
      gsql_request->Respond(msg);
    }
  }

  void ImportTree(
           FilterHelper *filter_helper,
           UserRequest *user_request,
           GsqlRequest *gsql_request) {
    if (user_request->data_length == 0) {
      std::string msg("Payload missing.");
      gsql_request->Respond(msg);
      return;
    }
    std::string payload(user_request->data, user_request->data_length);

    param_t &params = user_request->params;
    std::string name = params["name"][0];
    std::string vtype = params["vtype"][0];
    std::string up_etype = params["up_etype"][0];
    std::string down_etype = params["down_etype"][0];
    char sep = params["sep"][0][0];
    char eol = params["eol"][0][0];

    boost::char_separator<char> line_sep(&eol);
    boost::tokenizer<boost::char_separator<char> > lines(payload, line_sep);
    BOOST_FOREACH (const std::string &l, lines) {
      std::cout << l << std::endl;
      std::cout << name << std::endl;
      std::cout << vtype << std::endl;

      boost::char_separator<char> field_sep(&sep);
      boost::tokenizer<boost::char_separator<char> > fields(l, field_sep);
      AttributesDelta attr;
      std::string msg;

      // upsert the root of tree
      attr.SetString("name", name);
      std::string id = name;
      if (! gsql_request->UpsertVertex(attr, vtype, id, msg)) {
        gsql_request->Respond("fail to upsert vertex, " + msg);
        return;
      }

      // upsert tree
      std::string pref = "";
      BOOST_FOREACH (const std::string &f, fields) {
        attr.Clear();
        attr.SetString("name", f);
        std::string id = pref + f;
        if (! gsql_request->UpsertVertex(attr, vtype, id, msg)) {
          gsql_request->Respond("fail to upsert vertex, " + msg);
          return;
        }
        pref += f;
        pref += ".";
      }
    }

    std::string msg;
    if (! gsql_request->FlushDelta(msg)) {
      gsql_request->Respond("fail to flush delta, " + msg);
      return;
    }

    gsql_request->Respond("done.");
  }


#ifdef __cplusplus
}
#endif



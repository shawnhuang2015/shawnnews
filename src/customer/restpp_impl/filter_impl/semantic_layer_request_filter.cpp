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
#include <set>
#include <string>
#include <vector>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

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

    std::cout << "eol = " << eol << "@" << std::endl;

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
      std::string root_id = name;
      if (! gsql_request->UpsertVertex(attr, vtype, root_id, msg)) {
        gsql_request->Respond("fail to upsert vertex, " + msg);
        return;
      }

      // upsert tree
      std::string pref = "";
      std::string parent_id = root_id;
      BOOST_FOREACH (const std::string &f, fields) {
        // upsert vertex
        attr.Clear();
        attr.SetString("name", f);
        std::string id = pref + f;
        if (! gsql_request->UpsertVertex(attr, vtype, id, msg)) {
          gsql_request->Respond("fail to upsert vertex, " + msg);
          return;
        }

        // upsert edge
        attr.Clear();
        if (! gsql_request->UpsertEdge(attr, vtype, parent_id, down_etype, vtype, id, msg) ||
            ! gsql_request->UpsertEdge(attr, vtype, id, up_etype, vtype, parent_id, msg)) {
          gsql_request->Respond("fail to upsert edge, " + msg);
          return;
        }

        parent_id = id;
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

  void PostUserTag(
           FilterHelper *filter_helper,
           UserRequest *user_request,
           GsqlRequest *gsql_request) {
    if (user_request->data_length == 0) {
      std::string msg("Payload missing.");
      gsql_request->Respond(msg);
      return;
    }

    // NOTE: payload format
    // userid;tag1|0.1,tag2|0.2,...
    std::string payload(user_request->data, user_request->data_length);

    param_t &params = user_request->params;
    const std::string &name = params["name"][0];
    const std::string &object_vtype = params["object_vtype"][0];
    const std::string &ontology_vtype = params["ontology_vtype"][0];
    const std::string &ontology_up_etype = params["ontology_up_etype"][0];
    const std::string &ontology_down_etype = params["ontology_down_etype"][0];
    const std::string &object_ontology_etype = params["object_ontology_etype"][0];
    const char sep = params["sep"][0][0];
    const char tag_sep = params["tag_sep"][0][0];
    const char weight_sep = params["weight_sep"][0][0];
    const char eol = params["eol"][0][0];

    // TODO(@alan): 2 flags for now, one for path-tag, the other for single tag
    int flag = 1;

    boost::tokenizer<boost::char_separator<char> > lines(
        payload, boost::char_separator<char>(&eol));
    BOOST_FOREACH (const std::string &l, lines) {
      std::vector<std::string> fields;
      boost::split(fields, l, boost::is_any_of(std::string(1, sep)));
      int size = fields.size();
      if (size != 2) {
        std::cout << "invalid data: " << l << std::endl;
        continue;
      }

      // get user id
      const std::string &user_id = fields[0];
      boost::tokenizer<boost::char_separator<char> > tags(
          fields[1], boost::char_separator<char>(&tag_sep));

      AttributesDelta attr;
      std::string msg;
      std::vector<std::string> tw;
      BOOST_FOREACH(const std::string &tag, tags) {
        // get tag/weight
        boost::split(tw, tag, boost::is_any_of(std::string(1, weight_sep)));
        int size = tw.size();
        if (size == 0) {
          std::cout << "skip empty tag" << std::endl;
          continue;
        }
        std::string t(tw[0]);
        float w = 1.0;
        if (size == 2) {
          w = ::atof(tw[1].c_str());
        }

        // TODO(@alan): check if tag exist in the tree, otherwise upsert

        // upsert edge
        attr.Clear();
        attr.SetFloat("weight", w);
        if (! gsql_request->UpsertEdge(attr, object_vtype, user_id, 
              object_ontology_etype, ontology_vtype, t, msg)) {
          gsql_request->Respond("fail to upsert edge, " + msg);
          return;
        }
      }
    }
    std::string msg;
    if (! gsql_request->FlushDelta(msg)) {
      gsql_request->Respond("fail to flush delta, " + msg);
      return;
    }

    gsql_request->Respond("done.");
  }

  void PreSetTagRequest(
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
    const std::string &name = params["name"][0];
    const char sep = params["sep"][0][0];
    const char tag_sep = params["tag_sep"][0][0];
    const char weight_sep = params["weight_sep"][0][0];
    const char eol = params["eol"][0][0];

    boost::tokenizer<boost::char_separator<char> > lines(
        payload, boost::char_separator<char>(&eol));
    std::set<std::string> uniq_tags;
    Json::Value &jsoptions = gsql_request->jsoptions;

    BOOST_FOREACH (const std::string &l, lines) {
      std::vector<std::string> fields;
      boost::split(fields, l, boost::is_any_of(std::string(1, sep)));
      int size = fields.size();
      if (size != 2) {
        std::cout << "invalid data: " << l << std::endl;
        continue;
      }

      // get all tags, pack them into a json array "tags"
      boost::tokenizer<boost::char_separator<char> > tags(
          fields[1], boost::char_separator<char>(&tag_sep));

      std::vector<std::string> tw;
      BOOST_FOREACH(const std::string &tag, tags) {
        boost::split(tw, tag, boost::is_any_of(std::string(1, weight_sep)));
        int size = tw.size();
        if (size == 0) {
          std::cout << "skip empty tag" << std::endl;
          continue;
        }
        if (uniq_tags.find(tw[0]) == uniq_tags.end()) {
          jsoptions["tags"].append(tw[0]);
          uniq_tags.insert(tw[0]);
        }
      }
    }
  }

#ifdef __cplusplus
}
#endif



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
    const std::string name = params["name"][0];
    const std::string vtype = params["vtype"][0];
    const std::string up_etype = params["up_etype"][0];
    const std::string down_etype = params["down_etype"][0];
    const std::string sep = std::string(1, params["sep"][0][0]);
    const std::string eol = std::string(1, params["eol"][0][0]);

    std::cout << "sep = " << sep << "@, eol = " << eol << "@" << std::endl;

    boost::char_separator<char> line_sep(eol.c_str());
    boost::tokenizer<boost::char_separator<char> > lines(payload, line_sep);
    BOOST_FOREACH (const std::string &l, lines) {
      std::cout << l << std::endl;
      std::cout << name << std::endl;
      std::cout << vtype << std::endl;

      boost::char_separator<char> field_sep(sep.c_str());
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

  void PostTag(
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
    const std::string sep = std::string(1, params["sep"][0][0]);
    const std::string tag_sep = std::string(1, params["tag_sep"][0][0]);
    const std::string weight_sep = std::string(1, params["weight_sep"][0][0]);
    const std::string eol = std::string(1, params["eol"][0][0]);
    const bool more = (params["more"][0] == "1");

    std::cout << "sep=" << sep << "@, tag_sep=" << tag_sep
        << "@, weight_sep=" << weight_sep << "@, eol="
        << eol << "@, more=" << more << std::endl;

    // parse inverted_tags
    Json::Value inverted_tags;
    Json::Reader reader;
    if (! reader.parse(params["inverted_tags"][0], inverted_tags)) {
      gsql_request->Respond("fail to parse inverted_tags.");
      return;
    }

    std::cout << "parsed inverted_tags = " << inverted_tags << std::endl;

    boost::tokenizer<boost::char_separator<char> > lines(
        payload, boost::char_separator<char>(eol.c_str()));
    std::vector<std::string> fields;
    BOOST_FOREACH (const std::string &l, lines) {
      std:: cout << "line: " << l << std::endl;
      fields.clear();
      boost::split(fields, l, boost::is_any_of(sep));
      int size = fields.size();
      if (size != 2) {
        std::cout << "invalid data: " << l << std::endl;
        continue;
      }

      // get object id
      const std::string &obj_id = fields[0];
      boost::tokenizer<boost::char_separator<char> > tags(
          fields[1], boost::char_separator<char>(tag_sep.c_str()));

      AttributesDelta attr;
      std::string msg;
      std::vector<std::string> tw;
      BOOST_FOREACH(const std::string &tag, tags) {
        // get tag/weight
        // FIXME: assume each tag only has one attr, which is weight,
        // in the future, might be more attrs
        tw.clear();
        boost::split(tw, tag, boost::is_any_of(weight_sep));
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

        // check if `t` exists in inverted_tags, then find it's primary id
        bool flag = true;
        if (inverted_tags.isMember(t)) {
          if (inverted_tags[t].size() == 1) {
            t = inverted_tags[t][0].asString();
          } else {
            // error, tag not unique
            gsql_request->Respond("tag (" + t + ") is not unique.");
            return;
          }
        } else if (more) {
          // add this new tag into ontology tree, insert vertex/edge
          attr.Clear();
          attr.SetString("name", t);
          // upsert vertex, new tags are level-1 tags, so primay id = name
          if (! gsql_request->UpsertVertex(attr, ontology_vtype, t, msg)) {
            gsql_request->Respond("fail to upsert vertex, " + msg);
            return;
          }
          // upsert edge
          attr.Clear();
          if (! gsql_request->UpsertEdge(attr, ontology_vtype, name, 
                ontology_down_etype, ontology_vtype, t, msg) ||
              ! gsql_request->UpsertEdge(attr, ontology_vtype, t, 
                ontology_up_etype, ontology_vtype, name, msg)) {
            gsql_request->Respond("fail to upsert edge, " + msg);
            return;
          }
        } else {
          flag = false;
        }

        // upsert edge
        if (flag) {
          attr.Clear();
          attr.SetFloat("weight", w);
          if (! gsql_request->UpsertEdge(attr, object_vtype, obj_id, 
                object_ontology_etype, ontology_vtype, t, msg)) {
            gsql_request->Respond("fail to upsert edge, " + msg);
            return;
          }
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

  // parse payload to get all tags
  // set tags to gpe in order to find the primary ids
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
    const std::string sep = std::string(1, params["sep"][0][0]);
    const std::string tag_sep = std::string(1, params["tag_sep"][0][0]);
    const std::string weight_sep = std::string(1, params["weight_sep"][0][0]);
    const std::string eol = std::string(1, params["eol"][0][0]);

    boost::tokenizer<boost::char_separator<char> > lines(
        payload, boost::char_separator<char>(eol.c_str()));
    std::set<std::string> uniq_tags;
    Json::Value &jsoptions = gsql_request->jsoptions;

    std::vector<std::string> fields;
    BOOST_FOREACH (const std::string &l, lines) {
      fields.clear();
      boost::split(fields, l, boost::is_any_of(sep));
      int size = fields.size();
      if (size != 2) {
        std::cout << "invalid data: " << l << std::endl;
        continue;
      }

      // get all tags, pack them into a json array "tags"
      boost::tokenizer<boost::char_separator<char> > tags(
          fields[1], boost::char_separator<char>(tag_sep.c_str()));

      std::vector<std::string> tw;
      BOOST_FOREACH(const std::string &tag, tags) {
        tw.clear();
        boost::split(tw, tag, boost::is_any_of(weight_sep));
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



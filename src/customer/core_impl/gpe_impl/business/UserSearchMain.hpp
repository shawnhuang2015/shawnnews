#ifndef GPERUN_USER_SEARCH_MAIN_HPP_
#define GPERUN_USER_SEARCH_MAIN_HPP_

#include <gpelib4/udfs/udfinclude.hpp>
#include <gpe/serviceimplbase.hpp>
#include <time.h>
#include "UserSearchUDF.hpp"

namespace UDIMPL {
  #define AnyType 65536

  //UserSearchMain is the main class to handle the user_search Endpoint
  class UserSearchMain {
  public:
    UserSearchMain(ServiceAPI& serviceapi_, EngineServiceRequest& request_)
      :serviceapi(serviceapi_), request(request_) {
      graphapi = serviceapi.CreateGraphAPI(&request);
      graphInfo.getGraphMeta(serviceapi);
    }

    std::string getCondBehaviorKey(Json::Value &cond) {
      std::cout << "Func getCondBehaviorKey" << std::endl;
      std::string action = cond["action"].asString();
      std::string objectCategory = cond["objectCategory"].asString();
      std::string objectType = cond["objectType"].asString();
      std::string ontologyType = cond["ontologyType"].asString();

      return objectType + "_" + action + "_" + objectCategory + "_" + ontologyType;
    }

    //use condition to generate search path
    std::vector<uint32_t> getPathKey(Json::Value &cond) {
      std::cout << "Func getPathKey" << std::endl;
      std::string action = cond["action"].asString();
      std::string objectCategory = cond["objectCategory"].asString();
      std::string objectType = cond["objectType"].asString();
      std::string ontologyType = cond["ontologyType"].asString();

      std::vector<uint32_t> res;
      uint32_t itemT;
      uint32_t behrT;
      graphInfo.getVertexTypeIndex(objectCategory, itemT);
      graphInfo.getVertexTypeIndex(action, behrT);
      if (objectType == "Item") {
        res.push_back(itemT);
        res.push_back(behrT);
        res.push_back(targetId);
      } else if (objectType == "Category") {
        uint32_t ontoT;
        graphInfo.getVertexTypeIndex(ontologyType, ontoT);
        res.push_back(ontoT);
        res.push_back(itemT);
        res.push_back(behrT);
        res.push_back(targetId);
      }
      
      return res;
    }

    //If the search condition is to match the ontology vertices that contions substring, 
    //SubstrVertices will find all the matched vertices
    void SubstrVertices(uint32_t typeId, std::string sub, std::vector<uint64_t> &res) {
      gapi4::VerticesFilter_ByOneType typefilter(typeId);
      gapi4::VerticesCollection vs;
      graphapi->GetAllVertices((gapi4::BaseVerticesFilter*)(&typefilter), vs);
      while (vs.NextVertex()) {
        topology4::VertexAttribute* attr = vs.GetCurrentVertexAttribute();
        std::string name = attr->GetString("name"); 
        if (name.find(sub) != std::string::npos) {
          res.push_back(vs.GetCurrentVId());
        }
      }
    }

    //Calc the intersection of two sets
    void IntersectSet(std::set<VertexLocalId_t>& userIds, std::set<VertexLocalId_t>& tmpUserIds) {
      std::cout << "Func IntersectSet" << std::endl;
      std::set<VertexLocalId_t> res;
      for (std::set<VertexLocalId_t>::iterator it = userIds.begin(); it != userIds.end(); ++it) {
        if (tmpUserIds.find(*it) != tmpUserIds.end()) {
          res.insert(*it);
        }
      }
      userIds = res;
    }

    Operator toOperator(std::string op_str) {
      std::cout << "Func toOperator" << std::endl;
      if (op_str == ">") return GT;
      if (op_str == "<") return LT;
      if (op_str == ">=") return GE;
      if (op_str == "<=") return LE;
      if (op_str == "<>") return NE;
      if (op_str == "=") return EQ;
      if (op_str == "like") return LIKE;
      if (op_str == "dislike") return DIS_LIKE;
      return UNKNOWN_OP;
    }

    //Convert the item-behavior-user condition to the struct that can be used in UDF
    bool generateItemBehaviorCond(Json::Value &jsCond, ItemBehaviorCond& cond) {
      std::cout << "Func generateItemBehaviorCond" << std::endl;
      cond.sp = PathItemUser;
      cond.op = toOperator(jsCond["operator"].asString());
      graphInfo.getVertexTypeIndex(jsCond["objectCategory"].asString(), cond.itemT);
      graphInfo.getVertexTypeIndex(jsCond["ontologyType"].asString(), cond.ontoT);
      cond.times = jsCond["value"].asUInt();
      std::string objectId = jsCond["objectId"].asString();
      std::string type = boost::lexical_cast<std::string>(cond.itemT);
      VertexLocalId_t localId;
      std::vector<uint64_t> startIds;
      if (jsCond["is_sub"].asBool() == true) {
        SubstrVertices(cond.itemT, objectId, startIds);
      } else if (!serviceapi.UIdtoVId(request, type + "_" + objectId, localId)) {
        request.error_ = true;
        request.message_ += std::string("valid id: ") + type + "_" + objectId;
        return false;
      } else {
        startIds.push_back(localId);
      }
      cond.startId = startIds;
      if (jsCond["timeType"].asString() == "absolute") {
        cond.timeStart = jsCond["startTime"].asUInt();
        cond.timeEnd = jsCond["endTime"].asUInt();
      } else if (jsCond["timeType"].asString() == "relative") {
        time_t cur = time(NULL);
        cond.timeStart = cur - jsCond["startTime"].asUInt();
        cond.timeEnd = cur - jsCond["endTime"].asUInt();
      }
      return true;
    }

    //Convert the onto-item-behavior-user condition to the struct that can be used in UDF
    bool generateOntoBehaviorCond(Json::Value &jsCond, OntoBehaviorCond& cond) {
      std::cout << "Func generateOntoBehaviorCond" << std::endl;
      cond.sp = PathOntoItemUser;
      cond.op = toOperator(jsCond["operator"].asString());
      graphInfo.getVertexTypeIndex(jsCond["objectCategory"].asString(), cond.itemT);
      graphInfo.getVertexTypeIndex(jsCond["ontologyType"].asString(), cond.ontoT);
      cond.times = jsCond["value"].asUInt();
      std::string objectId = jsCond["objectId"].asString();
      std::string type = boost::lexical_cast<std::string>(cond.ontoT);
      VertexLocalId_t localId;
      std::vector<uint64_t> startIds;
      if (jsCond["is_sub"].asBool() == true) {
        SubstrVertices(cond.ontoT, objectId, startIds);
      } else if (!serviceapi.UIdtoVId(request, type + "_" + objectId, localId)) {
        request.error_ = true;
        request.message_ += std::string("valid id: ") + type + "_" + objectId;
        return false;
      } else {
        startIds.push_back(localId);
      }
      cond.startId = startIds;
      if (jsCond["timeType"].asString() == "absolute") {
        cond.timeStart = jsCond["startTime"].asUInt();
        cond.timeEnd = jsCond["endTime"].asUInt();
      } else if (jsCond["timeType"].asString() == "relative") {
        time_t cur = time(NULL);
        cond.timeStart = cur - jsCond["startTime"].asUInt();
        cond.timeEnd = cur - jsCond["endTime"].asUInt();
      }
      return true;
    }

    //calc all the matched userIds for a single crowd group
    bool calcCrowdCondUserIds(ServiceAPI& serviceapi, EngineServiceRequest& request, 
            Json::Value& cond, std::set<VertexLocalId_t>& userIds) {
      std::cout << "Func calcCrowdCondUserIds" << std::endl;

      //doIt - whether we have run the UDF or not
      bool doIt = false; 

      Json::Value& condOnto = cond["ontology"];
      Json::Value& condBehr = cond["behavior"];
      std::cout << "condOnto " << condOnto.toStyledString() << std::endl;
      std::cout << "condBehr " << condBehr.toStyledString() << std::endl;

      typedef UserSearchUDF UDF_t;

      //search path: onto - user
      std::vector<OntologyCond> ontoCondVec;
      for (uint32_t k = 0; k < condOnto.size(); ++k) {
        Json::Value& condOntoItem = condOnto[k];
        std::string type = condOntoItem["type"].asString();
        std::string factor = condOntoItem["factor"].asString();
        uint32_t typeId;
        graphInfo.getVertexTypeIndex(type, typeId);
        OntologyCond cond;
        cond.op = toOperator(condOntoItem["operator"].asString());
        cond.weight = condOntoItem["weight"].asDouble();
        VertexLocalId_t localId;
        std::vector<uint64_t> startIds;
        if (condOntoItem["is_sub"].asBool()) {
          SubstrVertices(typeId, factor, startIds);
        } else if (!serviceapi.UIdtoVId(request, boost::lexical_cast<std::string>(typeId) + "_" + factor, localId)) {
          request.error_ = true;
          request.message_ += std::string("valid id: ") + type + "_" + factor;
          return false;
        } else {
          startIds.push_back(localId);
        }
        cond.startId = startIds;
        ontoCondVec.push_back(cond);
      }
      if (!ontoCondVec.empty()) {
        std::vector<uint32_t> pathOntoCond;
        pathOntoCond.push_back(AnyType); //65536 - infinite typeId
        pathOntoCond.push_back(targetId);
        UDF_t udf(ontoCondVec, pathOntoCond);
        serviceapi.RunUDF(&request, &udf);
        udf.getUserSet(userIds);
        doIt = true;
      }

      //search path: item - behavior - user
      std::map<std::vector<uint32_t>, std::vector<ItemBehaviorCond> > ItemBehrCond;
      for (uint32_t k = 0; k < condBehr.size(); ++k) {
        Json::Value& condItem = condBehr[k];
        std::string objectType = condItem["objectType"].asString();
        if (objectType == "Item") {
          std::vector<uint32_t> key = getPathKey(condItem);
          ItemBehaviorCond itemCond;
          if (!generateItemBehaviorCond(condItem, itemCond)) return false;
          ItemBehrCond[key].push_back(itemCond);
          std::cout << "key = " << getCondBehaviorKey(condItem) << ", cond = " << itemCond << std::endl;
        }
      }
      for (std::map<std::vector<uint32_t>, std::vector<ItemBehaviorCond> >::iterator it = ItemBehrCond.begin();
              it != ItemBehrCond.end(); ++it) {
        UDF_t udf(it->second, it->first);
        serviceapi.RunUDF(&request, &udf);
        std::set<VertexLocalId_t> tmpUserIds;
        udf.getUserSet(tmpUserIds);
        if (doIt) {
          IntersectSet(userIds, tmpUserIds);
        } else {
          doIt = true;
          userIds = tmpUserIds;
        }
      }

      //search path: onto - item - behavior - user
      std::map<std::vector<uint32_t>, std::vector<OntoBehaviorCond> > OntoBehrCond;
      for (uint32_t k = 0; k < condBehr.size(); ++k) {
        Json::Value& condItem = condBehr[k];
        std::string objectType = condItem["objectType"].asString();
        if (objectType == "Category") {
          std::vector<uint32_t> key = getPathKey(condItem);
          OntoBehaviorCond ontoCond;
          if (!generateOntoBehaviorCond(condItem, ontoCond)) return false;
          OntoBehrCond[key].push_back(ontoCond);
          std::cout << "key = " << getCondBehaviorKey(condItem) << ", cond = " << ontoCond << std::endl;
        }
      }
      for (std::map<std::vector<uint32_t>, std::vector<OntoBehaviorCond> >::iterator it = OntoBehrCond.begin();
              it != OntoBehrCond.end(); ++it) {
        UDF_t udf(it->second, it->first);
        serviceapi.RunUDF(&request, &udf);
        std::set<VertexLocalId_t> tmpUserIds;
        udf.getUserSet(tmpUserIds);
        if (doIt) {
          IntersectSet(userIds, tmpUserIds);
        } else {
          doIt = true;
          userIds = tmpUserIds;
        }
      }
      return true;
    }

    //main function for user_search Endpoint
    bool RunUDF_UserSearch() {
      std::cout << "Func RunUDF_UserSearch" << std::endl;
      Json::Value& jsoptions = request.jsoptions_;
      Json::Value& selector = jsoptions["selector"];
      std::string targetStr = jsoptions["target"].asString();

      std::cout << jsoptions.toStyledString() << std::endl;
      std::cout << request.request_data_ << std::endl;

      //limit should be positive interger. Zero means Infinite
      uint32_t limit = std::numeric_limits<uint32_t>::max();  
      if (jsoptions.isMember("limit") && jsoptions["limit"].size() > 0) {
        limit = jsoptions["limit"][0].asUInt();
      }
      graphInfo.getVertexTypeIndex(targetStr, targetId);

      std::set<VertexLocalId_t> userIds;
      //iterator all the crowd group conditions and use UNION to merge results
      for (uint32_t index = 0; index < selector.size(); ++index) {
        Json::Value& jsCond = selector[index];
        std::set<VertexLocalId_t> userTemp;
        if (!calcCrowdCondUserIds(serviceapi, request, jsCond, userTemp)) {
          return true;
        }
        userIds.insert(userTemp.begin(), userTemp.end());
      }

      //output the final resutls
      JSONWriter* writer_ = request.outputwriter_;
      writer_->WriteStartObject();
      writer_->WriteName("count");
      writer_->WriteUnsignedInt(userIds.size());
      writer_->WriteName("userIds");
      writer_->WriteStartArray();
      for (std::set<VertexLocalId_t>::iterator it = userIds.begin(); it != userIds.end(); ++it) {
        --limit;
        writer_->WriteMarkVId(*it);
        request.output_idservice_vids.push_back(*it);
        if (limit == 0) {
          break;
        }
      }
      writer_->WriteEndArray();
      writer_->WriteEndObject();

      return true;
    }
  private:
    ServiceAPI& serviceapi;
    EngineServiceRequest& request;
    boost::shared_ptr<gapi4::GraphAPI> graphapi;
    GraphMetaData graphInfo;
    uint32_t targetId;
  };
}  // namepsace UDIMPL

#endif /* GPERUN_USER_SEARCH_MAIN_HPP_ */

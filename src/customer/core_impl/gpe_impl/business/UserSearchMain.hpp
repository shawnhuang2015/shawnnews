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
    #define D_Category_enum       "Category"
    #define D_Item_enum           "Item"
    #define D_Contains_enum       "Contains"
    #define D_Behavior_enum       "Behavior"

    #define D_relative_enum       "relative" 
    #define D_absolute_enum       "absolute"

    #define D_factor_key          "factor"
    #define D_operator_key        "operator"
    #define D_weight_key          "weight"
    #define D_type_key            "type"
    #define D_action_key          "action"
    #define D_objectType_key      "objectType"
    #define D_objectCategory_key  "objectCategory"
    #define D_ontologyType_key    "ontologyType"
    #define D_objectId_key        "objectId"
    #define D_operator_key        "operator"
    #define D_value_key           "value"
    #define D_startTime_key       "startTime"
    #define D_endTime_key         "endTime"
    #define D_timeType_key        "timeType"

    #define D_ontology_key        "ontology"
    #define D_behavior_key        "behavior"
    #define D_selector_key        "selector"
    #define D_target_key          "target"

    #define D_name_attr           "name"

  public:
    UserSearchMain(ServiceAPI& serviceapi_, EngineServiceRequest& request_)
      :serviceapi(serviceapi_), request(request_) {
      graphapi = serviceapi.CreateGraphAPI(&request);
      graphInfo.getGraphMeta(serviceapi);
    }

    std::string getCondBehaviorKey(Json::Value &cond) {
      std::cout << "Func getCondBehaviorKey" << std::endl;
      std::string action = cond[D_action_key].asString();
      std::string objectCategory = cond[D_objectCategory_key].asString();
      std::string objectType = cond[D_objectType_key].asString();
      std::string ontologyType = cond[D_ontologyType_key].asString();

      return objectType + "_" + action + "_" + objectCategory + "_" + ontologyType;
    }

    //remove the duplicated elements
    void unique(std::vector<uint64_t> &vec) {
      std::sort(vec.begin(), vec.end());
      vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
    }

    //use condition to generate search path
    std::vector<uint32_t> getPathKey(Json::Value &cond) {
      std::cout << "Func getPathKey" << std::endl;
      std::string action = cond[D_action_key].asString();
      std::string objectCategory = cond[D_objectCategory_key].asString();
      std::string objectType = cond[D_objectType_key].asString();
      std::string ontologyType = cond[D_ontologyType_key].asString();

      std::vector<uint32_t> res;
      uint32_t itemT;
      uint32_t behrT;
      graphInfo.getVertexTypeIndex(objectCategory, itemT);
      graphInfo.getVertexTypeIndex(action, behrT);
      if (objectType == D_Item_enum) {
        res.push_back(itemT);
        res.push_back(behrT);
        res.push_back(targetId);
      } else if (objectType == D_Category_enum || objectType == D_Contains_enum) {
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
        std::string name = attr->GetString(D_name_attr); 
        if (name.find(sub) != std::string::npos) {
          res.push_back(vs.GetCurrentVId());
        }
      }
      unique(res);
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
      cond.op = toOperator(jsCond[D_operator_key].asString());
      graphInfo.getVertexTypeIndex(jsCond[D_objectCategory_key].asString(), cond.itemT);
      graphInfo.getVertexTypeIndex(jsCond[D_ontologyType_key].asString(), cond.ontoT);
      cond.times = jsCond[D_value_key].asUInt();
      std::string objectId = jsCond[D_objectId_key].asString();
      std::string type = boost::lexical_cast<std::string>(cond.itemT);
      VertexLocalId_t localId;
      std::vector<uint64_t> startIds;
      if (!serviceapi.UIdtoVId(request, type + "_" + objectId, localId)) {
        request.error_ = true;
        request.message_ += std::string("valid id: ") + type + "_" + objectId;
        return false;
      } else {
        startIds.push_back(localId);
      }
      cond.startId = startIds;
      if (jsCond[D_timeType_key].asString() == D_absolute_enum) {
        cond.timeStart = jsCond[D_startTime_key].asUInt();
        cond.timeEnd = jsCond[D_endTime_key].asUInt();
      } else if (jsCond[D_timeType_key].asString() == D_relative_enum) {
        time_t cur = time(NULL);
        cond.timeStart = cur - jsCond[D_startTime_key].asUInt();
        cond.timeEnd = cur - jsCond[D_endTime_key].asUInt();
      }
      return true;
    }

    //Convert the onto-item-behavior-user condition to the struct that can be used in UDF
    bool generateOntoBehaviorCond(Json::Value &jsCond, OntoBehaviorCond& cond, bool isSub) {
      std::cout << "Func generateOntoBehaviorCond" << std::endl;
      cond.sp = PathOntoItemUser;
      cond.op = toOperator(jsCond[D_operator_key].asString());
      graphInfo.getVertexTypeIndex(jsCond[D_objectCategory_key].asString(), cond.itemT);
      graphInfo.getVertexTypeIndex(jsCond[D_ontologyType_key].asString(), cond.ontoT);
      cond.times = jsCond[D_value_key].asUInt();
      std::string objectId = jsCond[D_objectId_key].asString();
      std::string type = boost::lexical_cast<std::string>(cond.ontoT);
      VertexLocalId_t localId;
      std::vector<uint64_t> startIds;
      if (isSub) {
        SubstrVertices(cond.ontoT, objectId, startIds);
      } else if (!serviceapi.UIdtoVId(request, type + "_" + objectId, localId)) {
        request.error_ = true;
        request.message_ += std::string("valid id: ") + type + "_" + objectId;
        return false;
      } else {
        startIds.push_back(localId);
      }
      cond.startId = startIds;
      if (jsCond[D_timeType_key].asString() == D_absolute_enum) {
        cond.timeStart = jsCond[D_startTime_key].asUInt();
        cond.timeEnd = jsCond[D_endTime_key].asUInt();
      } else if (jsCond[D_timeType_key].asString() == D_relative_enum) {
        time_t cur = time(NULL);
        cond.timeStart = cur - jsCond[D_startTime_key].asUInt();
        cond.timeEnd = cur - jsCond[D_endTime_key].asUInt();
      }
      return true;
    }

    //calc all the matched userIds for a single crowd group
    bool calcCrowdCondUserIds(ServiceAPI& serviceapi, EngineServiceRequest& request, 
            Json::Value& cond, std::set<VertexLocalId_t>& userIds) {
      std::cout << "Func calcCrowdCondUserIds" << std::endl;

      //doIt - whether we have run the UDF or not
      bool doIt = false; 

      Json::Value& condOnto = cond[D_ontology_key];
      Json::Value& condBehr = cond[D_behavior_key];
      std::cout << "condOnto " << condOnto.toStyledString() << std::endl;
      std::cout << "condBehr " << condBehr.toStyledString() << std::endl;

      typedef UserSearchUDF UDF_t;

      //search path: onto - user
      std::vector<OntologyCond> ontoCondVec;
      for (uint32_t k = 0; k < condOnto.size(); ++k) {
        Json::Value& condOntoItem = condOnto[k];
        std::string type = condOntoItem[D_type_key].asString();
        std::string factor = condOntoItem[D_factor_key].asString();
        uint32_t typeId;
        graphInfo.getVertexTypeIndex(type, typeId);
        OntologyCond cond;
        cond.op = toOperator(condOntoItem[D_operator_key].asString());
        cond.weight = condOntoItem[D_weight_key].asDouble();
        VertexLocalId_t localId;
        std::vector<uint64_t> startIds;
        if (!serviceapi.UIdtoVId(request, boost::lexical_cast<std::string>(typeId) + "_" + factor, localId)) {
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
        std::string objectType = condItem[D_objectType_key].asString();
        if (objectType == D_Item_enum) {
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
        std::string objectType = condItem[D_objectType_key].asString();
        if (objectType == D_Category_enum || objectType == D_Contains_enum) {
          std::vector<uint32_t> key = getPathKey(condItem);
          OntoBehaviorCond ontoCond;
          if (!generateOntoBehaviorCond(condItem, ontoCond, 
                      (objectType == D_Contains_enum))) {
            return false;
          }
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
      Json::Value& selector = jsoptions[D_selector_key];
      std::string targetStr = jsoptions[D_target_key].asString();

      std::cout << jsoptions.toStyledString() << std::endl;
      std::cout << request.request_data_ << std::endl;

      //limit should be positive interger. Zero means Infinite
      uint32_t limit = std::numeric_limits<uint32_t>::max();  
      if (jsoptions.isMember("limit") && jsoptions["limit"].size() > 0) {
        limit = jsoptions["limit"][0].asUInt();
      }
      //request id
      std::string rid; 
      if (jsoptions.isMember("rid") && jsoptions["rid"].size() > 0) {
        rid = jsoptions["rid"][0].asString();
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
  private:
    ServiceAPI& serviceapi;
    EngineServiceRequest& request;
    boost::shared_ptr<gapi4::GraphAPI> graphapi;
    GraphMetaData graphInfo;
    uint32_t targetId;
  };
}  // namepsace UDIMPL

#endif /* GPERUN_USER_SEARCH_MAIN_HPP_ */

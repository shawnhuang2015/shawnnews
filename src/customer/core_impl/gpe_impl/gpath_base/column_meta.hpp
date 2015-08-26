/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: Generic path query, attribute meta
 *
 *      Author: Renchu
 *      Date: Aug 7th 2015
 ******************************************************************************/

#ifndef SRC_CUSTOMER_COREIMPL_GPEIMPL_GPATH_BASE_ATTRIBUTE_META_HPP_
#define SRC_CUSTOMER_COREIMPL_GPEIMPL_GPATH_BASE_ATTRIBUTE_META_HPP_

#include <gutil/gsqlcontainer.hpp>
#include <gpe/serviceimplbase.hpp>
#include <gutil/gjsonhelper.hpp>
#include "gpathTypeDefines.hpp"
// #include "graphtypeinfo.hpp"

namespace UDIMPL {
  namespace GPATH_TOOLS {
    // Attribute Meta
    class ColumnMeta {
    public:
      // Column Number
      uint32_t colNumber;
      // DAG Layer
      uint32_t dagLayerNumber;
      // Map from internal name to index (do once at initialize)
      GUnorderedMap<std::string, uint32_t>::T internalNameToIndex;
      // The internal name of each index (do once at endrun for output)
      GVector<std::string>::T indexToInternalName;
      // DAG layer of each index
      GVector<uint32_t>::T indexToDAGLayer;
      // In DAG bias of each index
      GVector<uint32_t>::T indexToDAGBias;
      // Index of each layer's first element
      GVector<uint32_t>::T indexLayerFirstElement;

      ColumnMeta(): colNumber(0), dagLayerNumber(0) {
        indexLayerFirstElement.push_back(0);
      }

      // Add a DAG layer attribute names
      void addDAGLayer(GVector<std::string>::T& dagLayer) {
        uint32_t bias = 0;
        for (uint32_t i = 0; i < dagLayer.size(); ++i) {
          internalNameToIndex[dagLayer[i]] = colNumber;
          indexToInternalName.push_back(dagLayer[i]);
          indexToDAGBias.push_back(bias);
          bias++;
          indexToDAGLayer.push_back(dagLayerNumber);
          colNumber++;
        }
        indexLayerFirstElement.push_back(
          indexLayerFirstElement[dagLayerNumber] + dagLayer.size());
        dagLayerNumber++;
      }

      // Get layer bias
      uint32_t getLayerBias(uint32_t layer) {
        if (layer < indexLayerFirstElement.size() - 1) {
          return indexLayerFirstElement[layer];
        }
        return 0;
      }

      // Get attribute index of an attribute name
      void getAttrIndex(std::string& attrName, uint32_t& attrIndex) {
        attrIndex = internalNameToIndex[attrName];
      }

      // Get attribute layer number and bias of an attribute name
      void getAttrLayerAndBias(std::string& attrName, uint32_t& layer, uint32_t& bias) {
        uint32_t attrIndex = internalNameToIndex[attrName];
        layer = indexToDAGLayer[attrIndex];
        bias = indexToDAGBias[attrIndex];
      }

      // Construct global context table "." from Json
      ColumnMeta& buildBasicCtMeta(Json::Value& value/*, GraphMetaData& graphMetaData*/) {
        GVector<std::string>::T attrToAdd;
        // Anchor
        std::string prefix = value[ ANCHOR_NODE ][ V_NAME_INTERNAL ].asString() + ".";
        std::string anchorVType = value[ ANCHOR_NODE ][ V_TYPE_CHECK ].asString();
        Json::Value& anchorAddList = value[ ANCHOR_NODE ][ V_ADD_LIST ];
        for (uint32_t i = 0; i < anchorAddList.size(); ++i) {
          if (anchorAddList[i] == "*") {
            // GVector<std::string>::T vAttrs;
            // graphMetaData.getAllVertexAttributes(anchorVType, vAttrs);
            // for (uint32_t j = 0; j < vAttrs.size(); ++j) {
            //   attrToAdd.push_back(prefix + vAttrs[j]);
            // }
          }
          else {
            attrToAdd.push_back(prefix + anchorAddList[i].asString());
          }
        }
        addDAGLayer(attrToAdd);
        // Steps
        Json::Value& steps = value[ STEPS ];
        for (uint32_t lp = 0; lp < steps.size(); ++lp) {
          Json::Value& step  = steps[lp];
          // Edge
          GVector<std::string>::T eAttrToAdd;
          std::string prefix = step[ E_NAME_INTERNAL ].asString() + ".";
          std::string eType = step[ E_TYPE_CHECK ].asString();
          Json::Value& eAddList = step[ E_ADD_LIST ];
          for (uint32_t i = 0; i < eAddList.size(); ++i) {
            if (eAddList[i] == "*") {
              // GVector<std::string>::T eAttrs;
              // graphMetaData.getAllEdgeAttributes(eType, eAttrs);
              // for (uint32_t j = 0; j < eAttrs.size(); ++j) {
              //   eAttrToAdd.push_back(prefix + eAttrs[j]);
              // }
            }
            else {
              eAttrToAdd.push_back(prefix + eAddList[i].asString());
            }
          }
          addDAGLayer(eAttrToAdd);
          // Vertex
          GVector<std::string>::T vAttrToAdd;
          prefix = step[ V_NAME_INTERNAL ].asString() + ".";
          std::string vType = step[ V_TYPE_CHECK ].asString();
          Json::Value& vAddList = step[ V_ADD_LIST ];
          for (uint32_t i = 0; i < vAddList.size(); ++i) {
            if (vAddList[i] == "*") {
              // GVector<std::string>::T vAttrs;
              // graphMetaData.getAllVertexAttributes(vType, vAttrs);
              // for (uint32_t j = 0; j < vAttrs.size(); ++j) {
              //   vAttrToAdd.push_back(prefix + vAttrs[j]);
              // }
            }
            else {
              vAttrToAdd.push_back(prefix + vAddList[i].asString());
            }
          }
          addDAGLayer(vAttrToAdd);
        }

        return *this;
      }

      friend std::ostream& operator<<(std::ostream& os, const ColumnMeta& data) {
        os << "Col Number: " << data.colNumber << std::endl;
        os << "Dag Layer Number: " << data.dagLayerNumber << std::endl;
        os << "InternalName to Index" << std::endl;
        for (GUnorderedMap<std::string, uint32_t>::T::const_iterator it = data.internalNameToIndex.begin();
          it != data.internalNameToIndex.end(); ++it) {
          os << it->first << " : " << it->second << std::endl;
        }
        os << "Index to InternalName" << std::endl;
        for (uint32_t i = 0; i < data.indexToInternalName.size(); ++i) {
          os << i << " : " << data.indexToInternalName[i] << std::endl;
        }
        os << "Index to DAG layer" << std::endl;
        for (uint32_t i = 0; i < data.indexToDAGLayer.size(); ++i) {
          os << i << " : " << data.indexToDAGLayer[i] << std::endl;
        }
        os << "Index to DAG bias" << std::endl;
        for (uint32_t i = 0; i < data.indexToDAGBias.size(); ++i) {
          os << i << " : " << data.indexToDAGBias[i] << std::endl;
        }
        os << "Layer bias" << std::endl;
        for (uint32_t i = 0; i < data.indexLayerFirstElement.size() - 1; ++i) {
          os << i << " : " << data.indexLayerFirstElement[i] << std::endl;
        }
        return os;
      }
    };

  }
}

#endif  //SRC_CUSTOMER_COREIMPL_GPEIMPL_GPATH_BASE_ATTRIBUTE_META_HPP_

/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPath
 *
 *  Created on: 8/4/2015
 *  Author: Renchu
 *  Description: RA Graph Structure
 ******************************************************************************/
#include <gutil/gsqlcontainer.hpp>
#include <gutil/gjsonhelper.hpp>
#include "gpathTypeDefines.hpp"
#include <boost/lexical_cast.hpp>
#include "expression.hpp"
#include "column_meta.hpp"

#ifndef SRC_CUSTOMER_COREIMPL_RAGRAPH_HPP_
#define SRC_CUSTOMER_COREIMPL_RAGRAPH_HPP_

namespace UDIMPL {
  namespace GPATH_TOOLS {
  
    // Base RA Node class, only contains RA type
    class RANode {
    public:
      bool isBarrier_;                // Whether this kind of node is a barrier node that need to be complete in endrun
      std::string raType_;            // raType_ saves a string like: select, project, group, union
      std::string nodeName_;          // Internal used Node name. Has "A", "B" for output nodes, and "A.0", "A.1" for intermediate nodes
      uint32_t pathNumber_;           // The path index in RA json
      bool isOutput_;                 // Whether this node need to be output
      bool pushGlobalMap_;            // Whether this node need to be pushed to global map
      GVector<uint32_t>::T children_; // Children by index in RAGraph
      GVector<uint32_t>::T parents_;  // Parents by index in RAGraph

      GVector<uint32_t>::T parentsIndex_;     // Parents with their pathName_
      GVector<std::string>::T parentsName_;   // Parents with their nodeNames_
      uint32_t inDegree_;                     // Incoming degree
      ColumnMeta oldColumnMeta_;           // The column meta of its parent RANode
      ColumnMeta newColumnMeta_;              // ColumnMeta of this RA node (used for getting column names)
      Json::Value describer_;                 // Json describing this RA node

      virtual ~RANode() {}      

      RANode() {}
      // Construct RANode from Json
      RANode(Json::Value& value) {
        isBarrier_ = false;
        pushGlobalMap_ = false;
        raType_ = value[ RA_TYPE ].asString();
        describer_ = value;
      }

      static RANode* RANodeFactory(Json::Value& value);

      // Reset incoming degree for traversal
      void resetInDegree() {
        inDegree_ = parents_.size();
      }
      
      // Return parents path numbers
      GVector<uint32_t>::T& parentsIndex() {
        return parentsIndex_;
      }

      // Return parents node names
      GVector<std::string>::T& parentsName() {
        return parentsName_;
      }

      // Base add parents
      virtual void addParents(GVector<RANode*>::T& raNodes_, GMap<std::string, uint32_t>::T& raIndex, std::string parentName) {
        uint32_t parentIndex = raIndex[parentName];
        uint32_t selfIndex = raIndex[nodeName_];
        parents_.push_back(parentIndex);
        raNodes_[parentIndex]->children_.push_back(selfIndex);
  
        parentsIndex_.push_back(raNodes_[parentIndex]->pathNumber_);
        parentsName_.push_back(parentName);
      }

      // Set column meta, need to be called after constructing the topology
      virtual void setColumnMeta(GVector<RANode*>::T& raNodes) = 0;
      
      friend std::ostream& operator<< (std::ostream& os, RANode& data) {
        os << "raType_: " << data.raType_ << std::endl;
        os << "nodeName_: " << data.nodeName_ << std::endl;
        os << "pathNumber: " << data.pathNumber_ << std::endl;
        os << "Is output: " << data.isOutput_ << std::endl;
        os << "Children: ";
        for (uint32_t i = 0; i < data.children_.size(); ++i) {
          os << data.children_[i] << " ";
        }
        os << std::endl << "Parents: ";
        for (uint32_t i = 0; i < data.parents_.size(); ++i) {
          os << data.parents_[i] << " ";
        }
        os << std::endl << "Parents Index: ";
        for (uint32_t i = 0; i < data.parentsIndex_.size(); ++i) {
          os << data.parentsIndex_[i] << " ";
        }
        os << std::endl << "Parents Name: ";
        for (uint32_t i = 0; i < data.parentsName_.size(); ++i) {
          os << data.parentsName_[i] << " ";
        }
        os << std::endl << std::endl;
        return os;
      }
    };
    
    // Base RA Node, contains nothing
    class BaseRANode : public RANode {
    public:
      BaseRANode(Json::Value& value) {
        isBarrier_ = false;
        pushGlobalMap_ = false;
        raType_ = NOTHING;
        nodeName_ = BASIC_CT;
        pathNumber_ = 0;
        oldColumnMeta_.buildBasicCtMeta(value);
        newColumnMeta_ = oldColumnMeta_;
        parentsIndex_.push_back(0);
        parentsName_.push_back(nodeName_);
      }

      void setColumnMeta(GVector<RANode*>::T& raNodes) { }
    };

    // Select RA Node, contains filter
    class SelectRANode : public RANode {
    public:
      Expression filter_;
      SelectRANode(Json::Value& value) : RANode(value), 
        filter_(value[ RA_OPTIONS ][ SELECT_FILTER ].asString()) {
      }
      // Select just copy parent's column meta
      void setColumnMeta(GVector<RANode*>::T& raNodes) {
        oldColumnMeta_ = newColumnMeta_ = raNodes[parents_[0]]->newColumnMeta_;
        filter_.setColumnMeta(oldColumnMeta_);
      }
    };
    
    // Project RA Node, contains vector of expression and matched name
    class ProjectRANode : public RANode {
    public:
      GVector<Expression>::T projectExpressions_;
      GVector<std::string>::T projectNames_;
      ProjectRANode(Json::Value& value) : RANode(value) {
        Json::Value& projExprJson = value[ RA_OPTIONS ][ PROJECTION_EXPR ];
        for (uint32_t i = 0; i < projExprJson.size(); ++i) {
          Expression expr(projExprJson[i].asString());
          projectExpressions_.push_back(expr);
        }
        Json::Value& projNameJson = value[ RA_OPTIONS ][ PROJECTION_NAME ];
        for (uint32_t i = 0; i < projNameJson.size(); ++i) {
          projectNames_.push_back(projNameJson[i].asString());
        }
      }
      // Project construct a two-layer column meta
      void setColumnMeta(GVector<RANode*>::T& raNodes) {
        oldColumnMeta_ = raNodes[parents_[0]]->newColumnMeta_;
        newColumnMeta_.addDAGLayer(projectNames_);
        GVector<std::string>::T attr;
        newColumnMeta_.addDAGLayer(attr);
        for (uint32_t i = 0; i < projectExpressions_.size(); ++i) {
          projectExpressions_[i].setColumnMeta(oldColumnMeta_);
        }
      }
    };

    // Group RA Node, contains vector of expression and matched name, and aggregation specification
    class GroupRANode : public ProjectRANode {
    public:
      std::string aggregateFuncName_;
      Expression aggregateExpression_;
      std::string aggregateName_;
      GroupRANode(Json::Value& value) : ProjectRANode(value), 
        aggregateExpression_(value[ RA_OPTIONS ][ AGGREGATION ][0][ AGGREGATION_EXPR ].asString()) {
        isBarrier_ = true;
        Json::Value& aggregation = value[ RA_OPTIONS ][ AGGREGATION ][0];
        aggregateFuncName_ = aggregation[ AGGREGATION_FUNC ].asString();
        aggregateName_ = aggregation[ AGGREGATION_NAME ].asString();
      }
      // Group construct a two-layer column meta. First layer is key, second layer is value
      void setColumnMeta(GVector<RANode*>::T& raNodes) {
        oldColumnMeta_ = raNodes[parents_[0]]->newColumnMeta_;
        GVector<std::string>::T columns = projectNames_;
        columns.push_back(aggregateName_);
        newColumnMeta_.addDAGLayer(columns);
        GVector<std::string>::T attr;
        newColumnMeta_.addDAGLayer(attr);
        for (uint32_t i = 0; i < projectExpressions_.size(); ++i) {
          projectExpressions_[i].setColumnMeta(oldColumnMeta_);
        }
        aggregateExpression_.setColumnMeta(oldColumnMeta_);
      }
    };

    // Union RA Node, contains union count
    class UnionRANode : public RANode {
    public:
      std::string unionContextTable_;
      UnionRANode(Json::Value& value) : RANode(value) {
        unionContextTable_ = value[ ALSO_DEPENDS_ON ].asString();
      }

      void addParents(GVector<RANode*>::T& raNodes_, GMap<std::string, uint32_t>::T& raIndex, std::string parentName) {
        RANode::addParents(raNodes_, raIndex, parentName);
        uint32_t pNode = raIndex[unionContextTable_];
        uint32_t selfNode = raIndex[nodeName_];
        parents_.push_back(pNode);
        raNodes_[pNode]->children_.push_back(selfNode);

        parentsIndex_.push_back(raNodes_[pNode]->pathNumber_);
        parentsName_.push_back(raNodes_[pNode]->nodeName_);
      } 

      // Union just copy parent's column meta
      void setColumnMeta(GVector<RANode*>::T& raNodes) {
        oldColumnMeta_ = newColumnMeta_ = raNodes[parents_[0]]->newColumnMeta_;
      }
    };

    // RAGraph structure, specifying the topological structure of RA nodes
    class RAGraph {
    public:
      GVector<RANode*>::T raNodes_;               // RA node list
      uint32_t pathNumberRange_;                  // RA path number range
      GVector<RANode*>::T reduceNodes_;           // RA nodes can be processed in reduce
      GVector<RANode*>::T endRunFrontierNodes_;   // RA nodes can be processed in endrun on frontier that need global map data
      GVector<RANode*>::T endRunNodes_;           // RA nodes can be processed in endrun exclude frontier nodes
      GVector<std::string>::T directOutputFromGlobalMap_;     // Node names directly output to json writer from global map
      GUnorderedMap<std::string, RANode*>::T nameToRANode_;   // Map from context table name to RANode

      RAGraph(Json::Value& value) {
        Json::Value& raJson = value[ RA ];
        GMap<std::string, uint32_t>::T raIndex;
        // Add RA nodes as a vector
        RANode* baseNode = new BaseRANode(value);
        nameToRANode_["."] = baseNode;
        raNodes_.push_back(baseNode);
        for (uint32_t i = 0; i < raJson.size(); ++i) {
          Json::Value& raSeq = raJson[i];
          std::string returnName = raSeq[ RETURN_NAME ].asString();
          bool isOutput = raSeq[ OUTPUT ].asBool();
          Json::Value& seqJson = raSeq[ OPTIONS ];
          uint32_t stepNumber = seqJson.size();
          for (uint32_t j = 0; j < stepNumber; ++j) {
            RANode* node = RANode::RANodeFactory(seqJson[j]);
            node->nodeName_ = returnName;
            // If it is not the last step of the RA, we append the step number at the end of the name
            if (j < stepNumber - 1) {
              node->nodeName_ += "." + boost::lexical_cast<std::string>(j);
              node->isOutput_ = false;
            }
            else {
              node->isOutput_ = isOutput;
            }
            node->pathNumber_ = i + 1;
            raNodes_.push_back(node);
            nameToRANode_[node->nodeName_] = node;
          }
        }
        
        // Map node to index
        for (uint32_t i = 0; i < raNodes_.size(); ++i) {
          raIndex[raNodes_[i]->nodeName_] = i;
        }

        // Construct parents
        uint32_t counter = 0;
        for (uint32_t i = 0; i < raJson.size(); ++i) {
          Json::Value& raSeq = raJson[i];
          std::string returnName = raSeq[ RETURN_NAME ].asString();
          Json::Value& seqJson = raSeq[ OPTIONS ];
          std::string inputName = raSeq[ INPUT_NAME ].asString();
          uint32_t stepNumber = seqJson.size();
          for (uint32_t j = 0; j < stepNumber; ++j) {
            counter++;
            // If it is not the last step of the RA, we append the step number at the end of the name
            if (j == 0) {
              raNodes_[counter]->addParents(raNodes_, raIndex, inputName);
            }
            else {
              std::string pName = returnName + "." + boost::lexical_cast<std::string>(j - 1);
              raNodes_[counter]->addParents(raNodes_, raIndex, pName);
            }
          }
        }

        // Reset incoming degree
        for (uint32_t i = 0; i < raNodes_.size(); ++i) {
          raNodes_[i]->resetInDegree();
        }
        // Set Path Number Range
        pathNumberRange_ = raJson.size() + 1;

        /*// TODO: Debug output
        for (uint32_t i = 0; i < raNodes_.size(); ++i) {
          //std::cout << "Node: " << i << std::endl;
          //std::cout << *raNodes_[i];
        }*/

        // Traverse the graph. Get the vectors for reduce and endrun
        GDeque<RANode*>::T queue;         // Queue for traversal
        GVector<RANode*>::T barriers;     // Barrier nodes
        // Reduce part
        queue.push_back(raNodes_[0]);     // Add . into queue
        reduceNodes_.push_back(raNodes_[0]);  // TODO: discuss whether need this
        while (!queue.empty()) {
          RANode* head = queue.front();
          queue.pop_front();
          if (!head->isBarrier_) {
            // Eliminate children's in-degree, and add new src nodes
            for (uint32_t i = 0; i < head->children_.size(); ++i) {
              RANode* child = raNodes_[head->children_[i]];
              child->inDegree_--;
              if (child->inDegree_ == 0) {
                queue.push_back(child);         // Add to queue tail
                reduceNodes_.push_back(child);  // Add to reduce nodes
                if (child->isOutput_) {         // Add this node's name to direct output from global map
                  directOutputFromGlobalMap_.push_back(child->nodeName_);
                  child->pushGlobalMap_ = true; // This node need to be pushed to global map
                }
              }
            }
          }
          else {
            // Add to barriers
            barriers.push_back(head);
          }
        }
        // Frontier part
        for (uint32_t i = 0; i < barriers.size(); ++i) {
          RANode* head = barriers[i];
          for (uint32_t j = 0; j < head->children_.size(); ++j) {
            RANode* child = raNodes_[head->children_[j]];
            child->inDegree_--;
            if (child->inDegree_ == 0) {
              queue.push_back(child);                 // Add to queue tail
              endRunFrontierNodes_.push_back(child);  // Add to endrun frontier nodes
            }
          }
        }
        for (uint32_t i = 0; i < endRunFrontierNodes_.size(); ++i) {
          for (uint32_t j = 0; j < endRunFrontierNodes_[i]->parents_.size(); ++j) {
            raNodes_[endRunFrontierNodes_[i]->parents_[j]]->pushGlobalMap_ = true;
          }
        }
        // EndRun part
        while (!queue.empty()) {
          RANode* head = queue.front();
          queue.pop_front();
          // Eliminate children's in-degree, and add new src nodes
          for (uint32_t i = 0; i < head->children_.size(); ++i) {
            RANode* child = raNodes_[head->children_[i]];
            child->inDegree_--;
            if (child->inDegree_ == 0) {
              queue.push_back(child);         // Add to queue tail
              endRunNodes_.push_back(child);  // Add to endrun nodes
            }
          }
        }

        // Set ColumnMeta
        // std::cout << pathNumberRange_ << std::endl;
        for (uint32_t i = 0; i < reduceNodes_.size(); ++i) {
          // std::cout << reduceNodes_[i]->nodeName_ << (reduceNodes_[i]->pushGlobalMap_ ? " (push)" : "") << " ";
          reduceNodes_[i]->setColumnMeta(raNodes_);
        }
        // std::cout << std::endl;
        for (uint32_t i = 0; i < endRunFrontierNodes_.size(); ++i) {
          // std::cout << endRunFrontierNodes_[i]->nodeName_ << " ";
          endRunFrontierNodes_[i]->setColumnMeta(raNodes_);
        }
        // std::cout << std::endl;
        for (uint32_t i = 0; i < endRunNodes_.size(); ++i) {
          // std::cout << endRunNodes_[i]->nodeName_ << " ";
          endRunNodes_[i]->setColumnMeta(raNodes_);
        }
        // std::cout << std::endl;
        // for (uint32_t i = 0; i < directOutputFromGlobalMap_.size(); ++i) {
        //   std::cout << directOutputFromGlobalMap_[i] << " ";
        // }
        // std::cout << std::endl;
      }

      RANode* getRANodeByName(std::string& name) {
        return nameToRANode_[name];
      }

      friend std::ostream& operator<<(std::ostream& os, const RAGraph& data) {
        os << data.pathNumberRange_ << std::endl;
        for (uint32_t i = 0; i < data.reduceNodes_.size(); ++i) {
          os << data.reduceNodes_[i]->nodeName_ << (data.reduceNodes_[i]->pushGlobalMap_ ? " (push)" : "") << " ";
        }
        os << std::endl;
        for (uint32_t i = 0; i < data.endRunFrontierNodes_.size(); ++i) {
          os << data.endRunFrontierNodes_[i]->nodeName_ << " ";
        }
        os << std::endl;
        for (uint32_t i = 0; i < data.endRunNodes_.size(); ++i) {
          os << data.endRunNodes_[i]->nodeName_ << " ";
        }
        os << std::endl;
        for (uint32_t i = 0; i < data.directOutputFromGlobalMap_.size(); ++i) {
          os << data.directOutputFromGlobalMap_[i] << " ";
        }
        os << std::endl;
        os << "Column Metas" << std::endl;
        for (uint32_t i = 0; i < data.raNodes_.size(); ++i) {
          os << data.raNodes_[i]->nodeName_ << std::endl;
          os << "Old" << std::endl;
          os << data.raNodes_[i]->oldColumnMeta_ << std::endl;
          os << "New" << std::endl;
          os << data.raNodes_[i]->newColumnMeta_ << std::endl;
        }
        os << "Name to raNode" << std::endl;
        for (GUnorderedMap<std::string, RANode*>::T::const_iterator it = data.nameToRANode_.begin();
             it != data.nameToRANode_.end(); ++it) {
          os << it->first << " : " << it->second->nodeName_ << std::endl;
        }
        return os;
      }

      // Path Number Range. Used for Reduce holding local RA vector size.
      uint32_t pathNumberRange() {
        return pathNumberRange_;
      }

      // Nodes can be processed in reduce
      GVector<RANode*>::T& getReduceNodes() {
        return reduceNodes_;
      }

      // Frontier nodes can be processed in endrun
      GVector<RANode*>::T& getEndRunFrontierNodes() {
        return endRunFrontierNodes_;
      }

      // Nodes can be processed in endrun excluding frontier nodes
      GVector<RANode*>::T& getEndRunNodes() {
        return endRunNodes_;
      }

      // Nodes need to be output directly from global map
      GVector<std::string>::T& getGlobalOutputNodes() {
        return directOutputFromGlobalMap_;
      }

      ~RAGraph() {
        for (uint32_t i = 0; i < raNodes_.size(); ++i) {
          delete raNodes_[i];
        }
        raNodes_.clear();
        reduceNodes_.clear();
        endRunFrontierNodes_.clear();
        endRunNodes_.clear();
        directOutputFromGlobalMap_.clear();
      }

    };
  }
}

#endif


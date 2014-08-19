/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: Neo4J Benchmarking
 *
 * Created on: 7/22/2014
 * Author: Arne Leitert
 * Description: This UDF implements a bidirectional single pair shortest path
 *              algorithm. The algorithm is based on the Dijkstra algorithm.
 *              However, it explores the graph from the source and the target
 *              vertex. The goal is to lower the number of iterations.
 ******************************************************************************/

#include <topology4/edgeattribute.hpp>
#include <topology4/vertexattribute.hpp>
#include <gpelib4/enginebase/baseudf.hpp>
#include <gpelib4/enginebase/context.hpp>
#include <gpelib4/enginedriver/enginedriver.hpp>

#include "../../core_ext/gpe/registry.hpp"
//#include "global_definitions.hpp"
//#include "global_num_map.hpp"

namespace UDIMPL {
  /**************STRUCTS FOR VERTEX AND MESSAGE VALUES************/
  // You can store any fixed-size structure as UDF data. define them here.

  static const uint64_t MAX_LENGTH = std::numeric_limits<uint64_t>::max();

  /**
   * Represents the message and vertex value. It stores the information if
   * a vertext was dicoverd by a search tree, what is parent in the tree is,
   * and the distance to the root (source or target vertex).
   */
  struct BFSTreeInfo {

      /**
       * The parent vertex in the s-tree.
       */
      VertexLocalId_t sPrev;

      /**
       * The parent vertex in the t-tree.
       */
      VertexLocalId_t tPrev;

      /**
       * Default constructor.
       */
      BFSTreeInfo() {
        sPrev = -1;
        tPrev = -1;
      }

      /**
       * Creates a new BFSTreeInfo.
       */
      BFSTreeInfo(VertexLocalId_t sPrev, VertexLocalId_t tPrev) {
        this->sPrev = sPrev;
        this->tPrev = tPrev;
      }

      /**
       * Aggregator for messages.
       */
      BFSTreeInfo& operator+=(const BFSTreeInfo& other) {

        // s-Tree
        if (this->sPrev == (VertexLocalId_t)-1) {
          this->sPrev = other.sPrev;
        }

        // t-Tree
        if (this->tPrev == (VertexLocalId_t)-1) {
          this->tPrev = other.tPrev;
        }

        return *this;
      }

  };

  /**************Enumerations and Constants************/

  /** BidirectionalBFS
   *
   * Calculates a shortest path from a source vertex s to a target vertex t.
   * The search is made bidirectional.
   *
   */
  class BidirectionalBFS: public gpelib4::BaseUDF {
    public:
      // UDF Settings: Computation Modes
      static const gpelib4::EngineProcessingMode EngineMode = gpelib4::EngineProcessMode_ActiveVertices;
      static const gpelib4::ValueTypeFlag ValueTypeMode_ = gpelib4::Mode_SingleValue;
      static const gpelib4::UDFDefineInitializeFlag InitializeMode_ = gpelib4::Mode_Initialize_Globally;
      static const gpelib4::UDFDefineFlag AggregateReduceMode_ = gpelib4::Mode_Defined;
      static const gpelib4::UDFDefineFlag CombineReduceMode_ = gpelib4::Mode_NotDefined;
      static const gpelib4::UDFDefineFlag VertexMapMode_ = gpelib4::Mode_Defined;
      static const int PrintMode_ = gpelib4::Mode_Print_NotDefined;

      // These typedefs are required by the engine.
      typedef topology4::VertexAttribute V_ATTR;
      typedef topology4::EdgeAttribute_Writable E_ATTR;
      typedef BFSTreeInfo V_VALUE;
      typedef BFSTreeInfo MESSAGE;

      /**************Class Constructor************/
      BidirectionalBFS(VertexLocalId_t sourceId, VertexLocalId_t targetId, size_t weightIndex) :
          gpelib4::BaseUDF(EngineMode, std::numeric_limits<size_t>::max()) {
        sourceId_ = sourceId;
        targetId_ = targetId;
        weightIndex_ = (size_t)weightIndex;
      }

      /**************Class Destructor************/
      ~BidirectionalBFS() {
      }

      /**************Initialize Globals************/
      void Initialize_GlobalVariables(gpelib4::GlobalVariables* globalvariables) {

        // ToDo: Remove after testing.
        globalvariables->Register(GV_MAP_COUNT, new gpelib4::LongSumVariable((int64_t)0));
        globalvariables->Register(GV_REDUCE_COUNT, new gpelib4::LongSumVariable((int64_t)0));

        GMAP* glMap = new GMAP();
        (*glMap)[-1] = sourceId_;
        (*glMap)[targetId_] = -1;

        globalvariables->Register(GV_PATH_MAP, glMap);
        globalvariables->Register(GV_FINAL_SEARCH_ITERATION, new gpelib4::StateVariable<size_t>(std::numeric_limits<size_t>::max()));

        // Register variable for strict preferece mas of source.
        globalvariables->Register(GV_INTERSECTION_VERTEX, new BFSIntInfoVariable());

      }

      /**************
       *
       *
       * Initialize Function (depends on settings)
       *
       *
       ************/
      ALWAYS_INLINE void Initialize(gpelib4::GlobalSingleValueContext<V_VALUE> * context) {
        context->WriteAll(V_VALUE(-1, -1), false);
        context->Write(sourceId_, V_VALUE(sourceId_, -1));
        context->Write(targetId_, V_VALUE(-1, targetId_));
        context->SetAllActiveFlag(false);
        context->SetActiveFlag(sourceId_);
        context->SetActiveFlag(targetId_);
      }

      /**
       * StartRun is only called before the first iteration begins (before the
       * first BeginIteration call).
       * It is not required to be defined.
       *
       * @param context Can be used to set active flags for vertices.
       */
      void StartRun(gpelib4::MasterContext* context) {
      }

      /**
       * BeforeIteration is called before each iteration begins.
       * It is not required to be defined.
       *
       * @param context Can be used to set active flags for vertices.
       */
      void BeforeIteration(gpelib4::MasterContext* context) {
      }

      /**************
       *
       *
       * EdgeMap Function (depends on settings)
       *
       *
       ************/
      ALWAYS_INLINE void EdgeMap(const VertexLocalId_t& srcId, V_ATTR* srcAttr, const V_VALUE& srcValue,
          const VertexLocalId_t& tarId, V_ATTR* tarAttr, const V_VALUE& tarValue, E_ATTR* edgeAttr,
          gpelib4::SingleValueMapContext<MESSAGE>* context) {

        // ToDo: Remove after testing.
        context->GlobalVariable_Reduce(GV_MAP_COUNT, (int64_t)1);

        // s-Tree
        if (srcValue.sPrev != (VertexLocalId_t)-1 && tarValue.sPrev == (VertexLocalId_t)-1) {
          context->Write(tarId, MESSAGE(srcId, -1));
        }

        // t-Tree
        if (srcValue.tPrev != (VertexLocalId_t)-1 && tarValue.tPrev == (VertexLocalId_t)-1) {
          context->Write(tarId, MESSAGE(-1, srcId));
        }

      }

      /**************
       *
       *
       * VertexMap Function (depends on settings)
       *
       *
       ************/
      ALWAYS_INLINE void VertexMap(const VertexLocalId_t& verId, V_ATTR* verAttr, const V_VALUE& verValue, gpelib4::SingleValueMapContext<MESSAGE>* context) {

        size_t finalIteration = context->GlobalVariable_GetValue<size_t>(GV_FINAL_SEARCH_ITERATION);
        size_t currIteration = context->Iteration();


        if (currIteration <= finalIteration) {
          // This should not happen.
          return;
        }

        if (currIteration - 1 == finalIteration) {

          // After last iteration the search was stopped. Hoever, there can still be some active branches.
          // Therefore, ignore all vertices except the intersecting vertex.

          VertexLocalId_t intVerId = context->GlobalVariable_GetValue<BFSIntInfo>(GV_INTERSECTION_VERTEX).vertexId;

          if (verId != intVerId) {
            return;
          }

        }

        // Vertex is a vertex on the path.

        if (verValue.sPrev != verId && verValue.sPrev != (VertexLocalId_t)-1) {

          // Send message to activate previous vertex for reduce.
          // The message contains an indicator to detect if the s or t path is reconstructed.
          context->Write(verValue.sPrev, MESSAGE(verId, -1));

          // Store edge in global memory.
          // edge: verValue.sPrev -> verId
          (*((GMAP*)context->GetGlobalVariable(GV_PATH_MAP)))[verValue.sPrev] = verId;
        }

        if (verValue.tPrev != verId && verValue.tPrev != (VertexLocalId_t)-1) {

          // Send empty message to activate previous vertex for reduce.
          context->Write(verValue.tPrev, MESSAGE(-1, verId));

          // Store edge in global memory.
          // edge: verId -> verValue.tPrev
          (*((GMAP*)context->GetGlobalVariable(GV_PATH_MAP)))[verId] = verValue.tPrev;
        }

      }


      /**
       * BetweenMapAndReduce is called between the map and reduce phases
       * of each iteration.
       * It is not required to be defined.
       *
       * @param context Can be used to set active flags for vertices.
       */
      void BetweenMapAndReduce(gpelib4::MasterContext* context) {
      }

      /**************
       *
       *
       * CombineReduce Function (depends on settings)
       *
       *
       ************/

      /**************
       *
       *
       * Reduce Function (depends on settings)
       *
       *
       ************/

      ALWAYS_INLINE void Reduce(const VertexLocalId_t& verId, V_ATTR* verAttr, const V_VALUE& verValue,
          const MESSAGE& msg, gpelib4::SingleValueContext<V_VALUE>* context) {

        // ToDo: Remove after testing.
        context->GlobalVariable_Reduce(GV_REDUCE_COUNT, (int64_t)1);

        // local copy
        V_VALUE vVal = verValue;

        // Test if graph is still searching
        size_t finalIteration = context->GlobalVariable_GetValue<size_t>(GV_FINAL_SEARCH_ITERATION);
        size_t currIteration = context->Iteration();

        if (currIteration > finalIteration) {

          // Vertex is on the path.

          // Remove other pointer. This way the next map will only explore in one direction.
          if (msg.sPrev != (VertexLocalId_t)-1) {
            vVal.tPrev = -1;
          }

          if (msg.tPrev != (VertexLocalId_t)-1) {
            vVal.sPrev = -1;
          }

          context->Write(verId, vVal);
          context->SetActiveFlag(verId);

          return;
        }

        vVal += msg;
        context->Write(verId, vVal);
        context->SetActiveFlag(verId);

        // Check if an intersection was found.
        if (vVal.sPrev != (VertexLocalId_t)-1 && vVal.tPrev != (VertexLocalId_t)-1) {
          // Yes, intersection found.

          uint64_t length = context->Iteration() * 2;

          // If vertext was already part of one of the BFS-trees, then the intersection is on an
          // edge. Thus, the length of the path is odd.
          if (verValue.sPrev != (VertexLocalId_t)-1 || verValue.tPrev != (VertexLocalId_t)-1 ) {
            length--;
          }

          context->GlobalVariable_Reduce<BFSIntInfo>(GV_INTERSECTION_VERTEX,
                        BFSIntInfo(verId, length));
        }

      }

      /**
       * AfterIteration is called after each iteration.
       * It is not required to be defined.
       *
       * @param context Can be used to set active flags for vertices.
       */
      void AfterIteration(gpelib4::MasterContext* context) {

        size_t finalIteration = context->GlobalVariable_GetValue<size_t>(GV_FINAL_SEARCH_ITERATION);
        size_t currIteration = context->Iteration();

        if (finalIteration <= currIteration) {
          // Nothing to do.
          return;
        }

        // Check for intersection. If found, end search.
        BFSIntInfo intInfo = context->GlobalVariable_GetValue<BFSIntInfo>(GV_INTERSECTION_VERTEX);
        if (intInfo.distance < MAX_LENGTH) {
          reinterpret_cast<gpelib4::StateVariable<size_t>*>(context->GetGlobalVariable(GV_FINAL_SEARCH_ITERATION))->Set(context->Iteration());
          context->set_UDFMapRun(gpelib4::UDFMapRun_VertexMap);
          context->SetAllActiveFlag(false);
          context->SetActiveFlag(intInfo.vertexId);
        }

      }

      /**
       * EndRun is called after the final iteration ends (after the
       * last AfterIteration call).
       * It is not required to be defined.
       *
       * @param context Can be used to set active flags for vertices.
       */
      void EndRun(gpelib4::BasicContext* context) {
        BFSIntInfo intInfo = context->GlobalVariable_GetValue<BFSIntInfo>(GV_INTERSECTION_VERTEX);

        globalIntDist_ = intInfo.distance;
        foundPath_ = vector<VertexLocalId_t>();

        if (globalIntDist_ == MAX_LENGTH) {
          // Graph is diconnected.
          return;
        }

        // ToDo: Remove after testing.
        cout << "best intersection: " << globalIntDist_ << "\n";
        int64_t mapCount = context->GlobalVariable_GetValue<int64_t>(GV_MAP_COUNT);
        int64_t redCount = context->GlobalVariable_GetValue<int64_t>(GV_REDUCE_COUNT);
        cout << "        Map Calls: " << mapCount << "\n";
        cout << "     Reduce Calls: " << redCount << "\n";

        foundPath_ = vector<VertexLocalId_t>();
        GMAP glMap = context->GlobalVariable_GetValue<GMAP>(GV_PATH_MAP);

        VertexLocalId_t vId = glMap[-1];

        cout << "\n\033[35mPath:";
        do {
          cout << " " << vId;
          foundPath_.push_back(vId);
          vId = glMap[vId];
        } while (vId != (VertexLocalId_t)-1);

        cout << "\033[0m\n";
      }

      uint64_t getDistance() {
        return globalIntDist_;
      }

      vector<VertexLocalId_t> getPath() {
        return foundPath_;
      }

      /**************
       *
       *
       * Write Functions (depend on settings)
       *
       *
       ************/

      ALWAYS_INLINE void Write(gutil::GOutputStream& ostream, const VertexLocalId_t& vid, V_ATTR* vertexattr,
          const V_VALUE& singlevalue, gpelib4::BasicContext* context) {
      }

      /**
       * Output
       *
       **/
      std::string Output(gpelib4::BasicContext* context) {
        return "";
      }

    private:
      // define constants here, make sure to initialize them in the constructor

      typedef IntersectionInfo<uint64_t> BFSIntInfo;
      typedef IntersectionInfoVariable<uint64_t> BFSIntInfoVariable;

      /**
       * The id of the source vertex. Only used to initialise the UDF
       */
      VertexLocalId_t sourceId_;

      /**
       * The id of the source vertex. Only used to initialise the UDF
       */
      VertexLocalId_t targetId_;

      /**
       * The id of the source vertex. Only used to initialise the UDF
       */
      size_t weightIndex_;

      /**
       * The id of the source vertex. Only used to initialise the UDF
       */
      vector<VertexLocalId_t> foundPath_;

      uint64_t globalIntDist_;

  };

/********************UDF Runner Registry Object *************************/
////template <typename VID_t, typename V_ATTR_t, typename E_ATTR_t>
  class BidirectionalBFSRunner: public gperun::UDFRunner {
    public:
      typedef BidirectionalBFS UDF_t;
      BidirectionalBFSRunner() {
        requires<VertexLocalId_t>("sourceId", "s", "The vertex id of the source vertex.", &sourceId, 0);
        requires<VertexLocalId_t>("targetId", "t", "The vertex id of the target vertex.", &targetId, 0);
        requires<VertexLocalId_t>("atrrIndex", "i", "The index of the edge attribute containing the edge weight.", &atrrIndex, 0);
      }
      ~BidirectionalBFSRunner() {
      }
      void createUDF() {
        udf = new UDF_t(sourceId, targetId, atrrIndex);
      }
      void runUDF(std::string engineConfigFile, std::string topologyConfigFile, std::string outputFile) {
        gpelib4::EngineDriver driver(engineConfigFile, topologyConfigFile);
        driver.RunSingleVersion<UDF_t>((UDF_t*) udf, outputFile);
      }
      static void registerUDF(gperun::UDFRegistry& registry) {
        registry.add("BidirectionalBFS", new BidirectionalBFSRunner());
      }
    private:
      VertexLocalId_t sourceId;
      VertexLocalId_t targetId;
      unsigned int atrrIndex;
  };
}//namespace UDIMPL

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

namespace UDIMPL {
  /**************STRUCTS FOR VERTEX AND MESSAGE VALUES************/
  // You can store any fixed-size structure as UDF data. define them here.
  /**
   * Defines the type for edge weigts and distances.
   */
  typedef unsigned int WEIGHT;

  static const WEIGHT MAX_WEIGHT = std::numeric_limits<WEIGHT>::max();

  /**
   * Represents the message and vertex value. It stores the information if
   * a vertext was dicoverd by a search tree, what is parent in the tree is,
   * and the distance to the root (source or target vertex).
   */
  struct SearchTreeInfo {

      /**
       * The parent vertex in the s-tree.
       */
      VertexLocalId_t sPrev;

      /**
       * The parent vertex in the t-tree.
       */
      VertexLocalId_t tPrev;

      /**
       * The distance to s.
       */
      WEIGHT sDist;

      /**
       * The distance to t.
       */
      WEIGHT tDist;

      /**
       * Default constructor.
       */
      SearchTreeInfo() {
        sPrev = -1;
        sDist = MAX_WEIGHT;

        tPrev = -1;
        tDist = MAX_WEIGHT;
      }

      /**
       * Creates a new SearchTreeInfo.
       */
      SearchTreeInfo(VertexLocalId_t sPrev, WEIGHT sDist, VertexLocalId_t tPrev, WEIGHT tDist) {
        this->sPrev = sPrev;
        this->sDist = sDist;

        this->tPrev = tPrev;
        this->tDist = tDist;
      }

      /**
       * Aggregator for messages.
       */
      SearchTreeInfo& operator+=(const SearchTreeInfo& other) {

        // s-Tree
        if (other.sDist < this->sDist) {
          this->sDist = other.sDist;
          this->sPrev = other.sPrev;
        }

        // t-Tree
        if (other.tDist < this->tDist) {
          this->tDist = other.tDist;
          this->tPrev = other.tPrev;
        }

        return *this;
      }
  };

  /**
   * Represents the best known intersection of both search trees. It stores
   * the vertex id and the total distance of the pass throw this vertex.
   */
  struct IntersectionInfo {

      /**
       * The id of the vertex where both trees intersect.
       */
      VertexLocalId_t vertexId;

      /**
       * The total distance from s to t over this vertex.
       */
      WEIGHT distance;

      /**
       * Default constructor.
       */
      IntersectionInfo() {
        vertexId = -1;
        distance = MAX_WEIGHT;
      }

      /**
       * Creates a new IntersectionInfo.
       */
      IntersectionInfo(VertexLocalId_t vertexId, WEIGHT distance) {
        this->vertexId = vertexId;
        this->distance = distance;
      }

      /**
       * Comparer for aggregation.
       */
      bool operator<(const IntersectionInfo& other) const {
        return this->distance < other.distance;
      }

  };

  /**
   * Class for a global variable which stores the best known intersection of both search trees. Its
   * implementation is based on the MinVariable class (src/core/gpe/gpelib4/enginebase/variable.hpp).
   * The purpuse is to provide a minimum-aggregator for a global IntersectionInfo variable.
   */
  class IntersectionInfoVariable: public BasicTypeVariableObject<IntersectionInfo> {
    public:
      IntersectionInfoVariable() {
        this->value_ = IntersectionInfo();
      }

      explicit IntersectionInfoVariable(IntersectionInfo value) {
        this->value_ = value;
      }

    protected:
      BaseVariableObject* MakeLocalCopy() const {
        return new IntersectionInfoVariable();
      }

      void Reduce(const void* newvalue) {
        const IntersectionInfo* value = reinterpret_cast<const IntersectionInfo*>(newvalue);
        if ((*value) < this->value_)
          this->value_ = (*value);
      }

      void Combine(BaseVariableObject* localcopy) {
        IntersectionInfoVariable* obj = (IntersectionInfoVariable*) localcopy;
        if (obj->value_ < this->value_)
          this->value_ = obj->value_;
      }
  };

  /**************Enumerations and Constants************/

  /**
   * Global variabels.
   */
  enum {

    /**
     * Represents the best known intersection of both search trees.
     */
    GV_INTERSECTION_VERTEX,

    /**
     * The index of the edge attribute containing the edge weight.
     */
    GV_WEIGHT_INDEX,

    /**
     * The minimal distance of active vertices of the s-tree.
     * The distance has been calculated in the last map or reduce phase.
     */
    GV_CURRENT_MIN_ACTIVE_WEIGHT_S,

    /**
     * The minimal distance of active vertices of the s-tree.
     * Variable to correctly aggregate the distance for the next map or reduce phase.
     */
    GV_NEXT_MIN_ACTIVE_WEIGHT_S,

    /**
     * The minimal distance of active vertices of the t-tree.
     * The distance has been calculated in the last map or reduce phase.
     */
    GV_CURRENT_MIN_ACTIVE_WEIGHT_T,

    /**
     * The minimal distance of active vertices of the t-tree.
     * Variable to correctly aggregate the distance for the next map or reduce phase.
     */
    GV_NEXT_MIN_ACTIVE_WEIGHT_T,

    /**
     * Flag which shows if the s-tree has living branches.
     */
    GV_IS_S_ALIVE,

    /**
     * Flag which shows if the t-tree has living branches.
     */
    GV_IS_T_ALIVE,

  };

  /** BidirectionalShortestPath
   *
   * Calculates a shortest path from a source vertex s to a target vertex t.
   * The search is made bidirectional.
   *
   */
  class BidirectionalShortestPath: public gpelib4::BaseUDF {
    private:
      // define constants here, make sure to initialize them in the constructor

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
      unsigned long int weightIndex_;

    public:
      // UDF Settings: Computation Modes
      static const gpelib4::EngineProcessingMode EngineMode = gpelib4::EngineProcessMode_ActiveVertices;
      static const gpelib4::ValueTypeFlag ValueTypeMode_ = gpelib4::Mode_SingleValue;
      static const gpelib4::UDFDefineInitializeFlag InitializeMode_ = gpelib4::Mode_Initialize_Globally;
      static const gpelib4::UDFDefineFlag AggregateReduceMode_ = gpelib4::Mode_Defined;
      static const gpelib4::UDFDefineFlag CombineReduceMode_ = gpelib4::Mode_NotDefined;
      static const gpelib4::UDFDefineFlag VertexMapMode_ = gpelib4::Mode_NotDefined;
      static const int PrintMode_ = gpelib4::Mode_Print_NotDefined;

      // These typedefs are required by the engine.
      typedef topology4::VertexAttribute V_ATTR;
      typedef topology4::EdgeAttribute_Writable E_ATTR;
      typedef SearchTreeInfo V_VALUE;
      typedef SearchTreeInfo MESSAGE;

      /**************Class Constructor************/
      BidirectionalShortestPath(VertexLocalId_t sourceId, VertexLocalId_t targetId, unsigned long int weightIndex) :
          gpelib4::BaseUDF(EngineMode, std::numeric_limits<size_t>::max()) {
        sourceId_ = sourceId;
        targetId_ = targetId;
        weightIndex_ = weightIndex;
      }

      /**************Class Destructor************/
      ~BidirectionalShortestPath() {
      }

      /**************Initialize Globals************/
      void Initialize_GlobalVariables(gpelib4::GlobalVariables* globalvariables) {
        // Register variable for strict preferece mas of source.
        globalvariables->Register(GV_INTERSECTION_VERTEX, new IntersectionInfoVariable());
        globalvariables->Register(GV_WEIGHT_INDEX, new gpelib4::LongStateVariable(weightIndex_));

        // Start values are 0, because the distance from s and t to itself is 0.
        globalvariables->Register(GV_CURRENT_MIN_ACTIVE_WEIGHT_S, new gpelib4::StateVariable<WEIGHT>(0));
        globalvariables->Register(GV_CURRENT_MIN_ACTIVE_WEIGHT_T, new gpelib4::StateVariable<WEIGHT>(0));

        globalvariables->Register(GV_NEXT_MIN_ACTIVE_WEIGHT_S, new gpelib4::MinVariable<WEIGHT>(0));
        globalvariables->Register(GV_NEXT_MIN_ACTIVE_WEIGHT_T, new gpelib4::MinVariable<WEIGHT>(0));

        // BoolVariable has an or aggregator.
        globalvariables->Register(GV_IS_S_ALIVE, new gpelib4::BoolVariable(true));
        globalvariables->Register(GV_IS_T_ALIVE, new gpelib4::BoolVariable(true));
      }

      /**************
       *
       *
       * Initialize Function (depends on settings)
       *
       *
       ************/
      ALWAYS_INLINE void Initialize(gpelib4::GlobalSingleValueContext<V_VALUE> * context) {
        context->WriteAll(V_VALUE(), false);
        context->Write(sourceId_, V_VALUE(-1, 0, -1, MAX_WEIGHT));
        context->Write(targetId_, V_VALUE(-1, MAX_WEIGHT, -1, 0));
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

        unsigned long int weightIndex = context->GlobalVariable_GetValue<unsigned long int>(GV_WEIGHT_INDEX);
        WEIGHT edgeWeight = edgeAttr->GetUInt32(weightIndex, 0);

        WEIGHT intDist = context->GlobalVariable_GetValue<IntersectionInfo>(GV_INTERSECTION_VERTEX).distance;
        WEIGHT currMinSDist = context->GlobalVariable_GetValue<WEIGHT>(GV_CURRENT_MIN_ACTIVE_WEIGHT_S);
        WEIGHT currMinTDist = context->GlobalVariable_GetValue<WEIGHT>(GV_CURRENT_MIN_ACTIVE_WEIGHT_T);

        // s-Tree
        if (srcValue.sDist < MAX_WEIGHT) {
          //Vertex has been dicovered by s-tree
          WEIGHT newSDist = srcValue.sDist + edgeWeight;

          bool expandSTree = ShouldVertexEpandTree(srcValue, currMinTDist, intDist, true);

          if (srcValue.sPrev != tarId && expandSTree && tarValue.sDist > newSDist && newSDist <= intDist) {
            context->Write(tarId, MESSAGE(srcId, newSDist, -1, MAX_WEIGHT));
            context->GlobalVariable_Reduce(GV_NEXT_MIN_ACTIVE_WEIGHT_S, newSDist);
            context->GlobalVariable_Reduce(GV_IS_S_ALIVE, true);
          }
        }

        // t-Tree
        if (srcValue.tDist < MAX_WEIGHT) {
          //Vertex has been dicovered by t-tree
          WEIGHT newTDist = srcValue.tDist + edgeWeight;

          bool expandTTree = ShouldVertexEpandTree(srcValue, currMinSDist, intDist, false);

          if (srcValue.tPrev != tarId && expandTTree && tarValue.tDist > newTDist && newTDist <= intDist) {
            context->Write(tarId, MESSAGE(-1, MAX_WEIGHT, srcId, newTDist));
            context->GlobalVariable_Reduce(GV_NEXT_MIN_ACTIVE_WEIGHT_T, newTDist);
            context->GlobalVariable_Reduce(GV_IS_T_ALIVE, true);
          }
        }

      }

      /**************
       *
       *
       * VertexMap Function (depends on settings)
       *
       *
       ************/

      /**
       * BetweenMapAndReduce is called between the map and reduce phases
       * of each iteration.
       * It is not required to be defined.
       *
       * @param context Can be used to set active flags for vertices.
       */
      void BetweenMapAndReduce(gpelib4::MasterContext* context) {
        UpdateMinActiveDistance(context);
        CheckIfTreesAreAlive(context);

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

        // Update vertex value.
        V_VALUE vVal = verValue;

        // Determine which distance will be updated.
        bool newSDistance = vVal.sDist > msg.sDist;
        bool newTDistance = vVal.tDist > msg.tDist;

        vVal += msg;
        context->Write(verId, vVal, false);

        WEIGHT localIntDist = MAX_WEIGHT;

        // Check if sum is smaller than 'infinite'. tDist is substracted from MAX_WEIGHT to avoid overflow.
        if (vVal.sDist < MAX_WEIGHT - vVal.tDist) {
          localIntDist = vVal.sDist + vVal.tDist;
        }

        WEIGHT globalIntDist = context->GlobalVariable_GetValue<IntersectionInfo>(GV_INTERSECTION_VERTEX).distance;

        if (localIntDist < globalIntDist) {
          globalIntDist = localIntDist;
          context->GlobalVariable_Reduce<IntersectionInfo>(GV_INTERSECTION_VERTEX,
              IntersectionInfo(verId, localIntDist));
        }

        WEIGHT currMinSDist = context->GlobalVariable_GetValue<WEIGHT>(GV_CURRENT_MIN_ACTIVE_WEIGHT_S);
        WEIGHT currMinTDist = context->GlobalVariable_GetValue<WEIGHT>(GV_CURRENT_MIN_ACTIVE_WEIGHT_T);

        bool expandSTree = ShouldVertexEpandTree(vVal, currMinTDist, globalIntDist, true);
        bool expandTTree = ShouldVertexEpandTree(vVal, currMinSDist, globalIntDist, false);

        bool activateVertex = expandSTree || expandTTree;
        context->SetActiveFlag(activateVertex);

        if (activateVertex && newSDistance) {
          context->GlobalVariable_Reduce(GV_NEXT_MIN_ACTIVE_WEIGHT_S, vVal.sDist);
          context->GlobalVariable_Reduce(GV_IS_S_ALIVE, expandSTree);
        }

        if (activateVertex && newTDistance) {
          context->GlobalVariable_Reduce(GV_NEXT_MIN_ACTIVE_WEIGHT_T, vVal.tDist);
          context->GlobalVariable_Reduce(GV_IS_T_ALIVE, expandTTree);
        }

      }

      /**
       * AfterIteration is called after each iteration.
       * It is not required to be defined.
       *
       * @param context Can be used to set active flags for vertices.
       */
      void AfterIteration(gpelib4::MasterContext* context) {
        UpdateMinActiveDistance(context);
        CheckIfTreesAreAlive(context);
      }

      /**
       * EndRun is called after the final iteration ends (after the
       * last AfterIteration call).
       * It is not required to be defined.
       *
       * @param context Can be used to set active flags for vertices.
       */
      void EndRun(gpelib4::BasicContext* context) {
        globalIntDist_ = context->GlobalVariable_GetValue<IntersectionInfo>(GV_INTERSECTION_VERTEX).distance;

      }

      WEIGHT getDistance() {
        return globalIntDist_;
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
      WEIGHT globalIntDist_;

      /**
       * Determins if exploring the tree from the given vertex can lead to a shorter path than the
       * best currently known.
       */
      bool ShouldVertexEpandTree(const V_VALUE& verValue, WEIGHT currMinActiveBackDist, WEIGHT bestKnownInterDist,
          bool isSTree) {

        WEIGHT forwardDist = isSTree ? verValue.sDist : verValue.tDist;
        WEIGHT backDist = isSTree ? verValue.tDist : verValue.sDist;

        // Lower bound for the distance in the back-tree.
        // ToDo: Implement the case that v is in (ii) of back-tree,
        //       i.e. backDist > currMinActiveBackDist but we know that it is optimal.
        WEIGHT bestBackDist = std::min(currMinActiveBackDist, backDist);

        return forwardDist + bestBackDist < bestKnownInterDist;

      }

      /**
       * Checks if both trees are still alive. If not, the program stops. Method is called by
       * BetweenMapAndReduce() and AfterIteration().
       */
      void CheckIfTreesAreAlive(gpelib4::MasterContext* context) {

        bool sIsAlive = context->GlobalVariable_GetValue<bool>(GV_IS_S_ALIVE);
        bool tIsAlive = context->GlobalVariable_GetValue<bool>(GV_IS_T_ALIVE);

        reinterpret_cast<gpelib4::BoolVariable*>(context->GetGlobalVariable(GV_IS_S_ALIVE))->Set(false);
        reinterpret_cast<gpelib4::BoolVariable*>(context->GetGlobalVariable(GV_IS_T_ALIVE))->Set(false);

        // If one of the trees dies, the shortest path is known.
        if (!sIsAlive || !tIsAlive) {
          context->Stop();
        }

      }

      /**
       * Updates the current variable with the minimal active distance of the search trees and
       * restes the aggregator for the next one. If the sum is larger or equal than the best known
       * path, stop the program, the best known path is optimal. Method is called by
       * BetweenMapAndReduce() and AfterIteration().
       */
      void UpdateMinActiveDistance(gpelib4::MasterContext* context) {

        WEIGHT nextMinSDist = context->GlobalVariable_GetValue<WEIGHT>(GV_NEXT_MIN_ACTIVE_WEIGHT_S);
        WEIGHT nextMinTDist = context->GlobalVariable_GetValue<WEIGHT>(GV_NEXT_MIN_ACTIVE_WEIGHT_T);

        context->GlobalVariable_Reduce(GV_CURRENT_MIN_ACTIVE_WEIGHT_S, nextMinSDist);
        context->GlobalVariable_Reduce(GV_CURRENT_MIN_ACTIVE_WEIGHT_T, nextMinTDist);

        reinterpret_cast<gpelib4::MinVariable<WEIGHT>*>(context->GetGlobalVariable(GV_NEXT_MIN_ACTIVE_WEIGHT_S))->Set(
            MAX_WEIGHT);
        reinterpret_cast<gpelib4::MinVariable<WEIGHT>*>(context->GetGlobalVariable(GV_NEXT_MIN_ACTIVE_WEIGHT_T))->Set(
            MAX_WEIGHT);


        WEIGHT globalIntDist = context->GlobalVariable_GetValue<IntersectionInfo>(GV_INTERSECTION_VERTEX).distance;

        GASSERT(nextMinSDist < MAX_WEIGHT, "The minimal active distance of s is 'infinit'.");
        GASSERT(nextMinTDist < MAX_WEIGHT, "The minimal active distance of t is 'infinit'.");

        // First comparison prevents overflow if sum is to large.
        if (nextMinSDist >= MAX_WEIGHT - nextMinTDist || nextMinSDist + nextMinTDist >= globalIntDist) {
          context->Stop();
        }
      }

  };

/********************UDF Runner Registry Object *************************/
////template <typename VID_t, typename V_ATTR_t, typename E_ATTR_t>
//  class BidirectionalShortestPathRunner: public UDFRunner {
//    public:
//      typedef BidirectionalShortestPath UDF_t;
//      BidirectionalShortestPathRunner() {
//        requires<VertexLocalId_t>("sourceId", "s", "The vertex id of the source vertex.", &sourceId, 0);
//        requires<VertexLocalId_t>("targetId", "t", "The vertex id of the target vertex.", &targetId, 0);
//        requires<VertexLocalId_t>("atrrIndex", "i", "The index of the edge attribute containing the edge weight.", &atrrIndex, 0);
//      }
//      ~BidirectionalShortestPathRunner() {
//      }
//      void createUDF() {
//        udf = new UDF_t(sourceId, targetId, atrrIndex);
//      }
//      void runUDF(std::string engineConfigFile, std::string topologyConfigFile, std::string outputFile) {
//        gpelib4::EngineDriver driver(engineConfigFile, topologyConfigFile);
//        driver.RunSingleVersion<UDF_t>((UDF_t*) udf, outputFile);
//      }
//      static void registerUDF(UDFRegistry& registry) {
//        registry.add("BidirectionalShortestPath", new BidirectionalShortestPathRunner());
//      }
//    private:
//      VertexLocalId_t sourceId;
//      VertexLocalId_t targetId;
//      unsigned int atrrIndex;
//  };
}//namespace gperun

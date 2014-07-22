/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: Neo4J Benchmarking
 *
 * Created on: 6/4/2014
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

namespace gperun {
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
       * Aggregator.
       */
      IntersectionInfo& operator+=(const IntersectionInfo& other) {

        if (other.distance < this->distance) {
          this->distance = other.distance;
          this->vertexId = other.vertexId;
        }

        return *this;
      }

      /**
       * Aggregator.
       */
      IntersectionInfo& operator=(const int other) {

        GASSERT(other == 0, "Assigned integer is unequal 0.");

        this->distance = MAX_WEIGHT;
        this->vertexId = -1;

        return *this;
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
        globalvariables->Register(GV_INTERSECTION_VERTEX, new gpelib4::SumVariable<IntersectionInfo>());
        globalvariables->Register(GV_WEIGHT_INDEX, new gpelib4::LongStateVariable(weightIndex_));
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
          const VertexLocalId_t& tarId, V_ATTR* tarAttr, const V_VALUE& tarValue,
          E_ATTR* edgeAttr, gpelib4::SingleValueMapContext<MESSAGE>* context) {

        unsigned long int weightIndex = context->GlobalVariable_GetValue<unsigned long int>(GV_WEIGHT_INDEX);
        WEIGHT edgeWeight = edgeAttr->GetUInt32(weightIndex, 0);

        WEIGHT intDist = context->GlobalVariable_GetValue<IntersectionInfo>(GV_INTERSECTION_VERTEX).distance;

        // s-Tree
        WEIGHT newSDist = srcValue.sDist + edgeWeight;
        if (srcValue.sPrev != tarId && tarValue.sDist > newSDist && newSDist <= intDist)
          context->Write(tarId, MESSAGE(srcId, newSDist, -1, MAX_WEIGHT));

        // t-Tree
        WEIGHT newTDist = srcValue.tDist + edgeWeight;
        if (srcValue.tPrev != tarId && tarValue.tDist > newTDist && newTDist <= intDist)
          context->Write(tarId, MESSAGE(-1, MAX_WEIGHT, srcId, newTDist));

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
        vVal += msg;
        context->Write(verId, vVal, false);

        WEIGHT localIntDist = MAX_WEIGHT;

        // Check if sum is smaller than 'infinite'.
        if (vVal.sDist < MAX_WEIGHT - vVal.tDist) {
          localIntDist = vVal.sDist + vVal.tDist;
        }

        WEIGHT globalIntDist = context->GlobalVariable_GetValue<IntersectionInfo>(GV_INTERSECTION_VERTEX).distance;

        if (localIntDist < globalIntDist) {
          globalIntDist = localIntDist;
          context->GlobalVariable_Reduce<IntersectionInfo>(GV_INTERSECTION_VERTEX, IntersectionInfo(verId, localIntDist));
        }

        context->SetActiveFlag(verId);

      }

      /**
       * AfterIteration is called after each iteration.
       * It is not required to be defined.
       *
       * @param context Can be used to set active flags for vertices.
       */
      void AfterIteration(gpelib4::MasterContext* context) {
      }

      /**
       * EndRun is called after the final iteration ends (after the
       * last AfterIteration call).
       * It is not required to be defined.
       *
       * @param context Can be used to set active flags for vertices.
       */
      void EndRun(gpelib4::BasicContext* context) {
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

        WEIGHT globalIntDist = context->GlobalVariable_GetValue<IntersectionInfo>(GV_INTERSECTION_VERTEX).distance;

        ostream << "globalIntDist: " << globalIntDist << "\n";
      }

      /**
       * Output
       *
       **/
      std::string Output(gpelib4::BasicContext* context) {
        return "";
      }

  };

  /********************UDF Runner Registry Object *************************/
//template <typename VID_t, typename V_ATTR_t, typename E_ATTR_t>
  class BidirectionalShortestPathRunner: public UDFRunner {
    public:
      typedef BidirectionalShortestPath UDF_t;

      BidirectionalShortestPathRunner() {
        requires<VertexLocalId_t>("sourceId", "s", "The vertex id of the source vertex.", &sourceId, 0);
        requires<VertexLocalId_t>("targetId", "t", "The vertex id of the target vertex.", &targetId, 0);
        requires<VertexLocalId_t>("atrrIndex", "i", "The index of the edge attribute containing the edge weight.", &atrrIndex, 0);
      }

      ~BidirectionalShortestPathRunner() {
      }

      void createUDF() {
        udf = new UDF_t(sourceId, targetId, atrrIndex);
      }

      void runUDF(std::string engineConfigFile, std::string topologyConfigFile, std::string outputFile) {
        gpelib4::EngineDriver driver(engineConfigFile, topologyConfigFile);
        driver.RunSingleVersion<UDF_t>((UDF_t*) udf, outputFile);
      }

      static void registerUDF(UDFRegistry& registry) {
        registry.add("BidirectionalShortestPath", new BidirectionalShortestPathRunner());
      }

    private:
      VertexLocalId_t sourceId;
      VertexLocalId_t targetId;
      unsigned int atrrIndex;

  };
} //namespace gperun

#include "global_num_map.hpp"

namespace UDIMPL {
  typedef GlobalNumericMap<VertexLocalId_t,VertexLocalId_t> GMAP;

  /**
   * Represents the best known intersection of both search trees. It stores
   * the vertex id and the total distance of the pass throw this vertex.
   */
  template <typename DistType>
  struct IntersectionInfo {

      /**
       * The id of the vertex where both trees intersect.
       */
      VertexLocalId_t vertexId;

      /**
       * The total distance from s to t over this vertex.
       */
      DistType distance;

      /**
       * Default constructor.
       */
      IntersectionInfo() {
        vertexId = -1;
        distance = std::numeric_limits<DistType>::max();;
      }

      /**
       * Creates a new IntersectionInfo.
       */
      IntersectionInfo(VertexLocalId_t vertexId, uint64_t distance) {
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
  template <typename DistType>
  class IntersectionInfoVariable: public BasicTypeVariableObject<IntersectionInfo<DistType> > {
    public:
      IntersectionInfoVariable() {
        this->value_ = IntersectionInfo<DistType>();
      }

      explicit IntersectionInfoVariable(IntersectionInfo<DistType> value) {
        this->value_ = value;
      }

    protected:
      BaseVariableObject* MakeLocalCopy() const {
        return new IntersectionInfoVariable();
      }

      void Reduce(const void* newvalue) {
        const IntersectionInfo<DistType>* value = reinterpret_cast<const IntersectionInfo<DistType>*>(newvalue);
        if ((*value) < this->value_)
          this->value_ = (*value);
      }

      void Combine(BaseVariableObject* localcopy) {
        IntersectionInfoVariable* obj = (IntersectionInfoVariable*) localcopy;
        if (obj->value_ < this->value_)
          this->value_ = obj->value_;
      }
  };

  /**
   * Global variabels.
   */
  enum {

    /**
     * Just for testing. Counts the number of EdgeMap calls
     */
    GV_MAP_COUNT,        // ToDo: Remove after testing.

    /**
     * Just for testing. Counts the number of Reduce calls.
     */
    GV_REDUCE_COUNT,     // ToDo: Remove after testing.

    /**
     * Hashmap which contains the edges of the found shortest path. Each edge is represented as
     * key-value pair. Additionally, the pairs (-1,s) and (t,-1) are stored to determin the start
     * and the end of the path.
     */
    GV_PATH_MAP,

    /**
     * Stores the number of the iteration which stopped the search. This allows to determine if
     * the search phase is over aand if an EdgeMap call is supposed to find the shortest path or
     * to explore the graph.
     */
    GV_FINAL_SEARCH_ITERATION,

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
     * The minimal distance of active vertices of the t-tree.
     * The distance has been calculated in the last map or reduce phase.
     */
    GV_CURRENT_MIN_ACTIVE_WEIGHT_T,

    /**
     * The minimal distance of active vertices of the s-tree.
     * Variable to correctly aggregate the distance for the next map or reduce phase.
     */
    GV_NEXT_MIN_ACTIVE_WEIGHT_S,

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



}

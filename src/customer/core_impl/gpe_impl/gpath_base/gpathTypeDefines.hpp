#ifndef _GPATH_TYPE_DEFINE_HPP_
#define _GPATH_TYPE_DEFINE_HPP_

#include "boost/variant.hpp"
#include <gpelib4/udfs/singleactiveudfbase.hpp>

namespace UDIMPL{
  namespace GPATH_TOOLS{
/*=======================contextTable.hpp and contextTuple.hpp==============*/
    enum VALUE_TYPE {INT, DOUBLE, STRING, BOOL};
    typedef GVector<int>::T VECTOR_INT;
    typedef GVector<double>::T VECTOR_DOUBLE;
    typedef GVector<std::string>::T VECTOR_STRING;
    typedef GVector<bool>::T VECTOR_BOOL;
    typedef boost::variant<int, double, std::string, bool, const char*> VAL;
    typedef GVector<VAL>::T VECTOR_ANY;
    typedef GUnorderedMap<std::string, VECTOR_ANY>::T UMAP;
    typedef GUnorderedMap<std::string, VAL>::T UValMAP;
/*========================Json Keywords=====================*/
    static const char* V_TYPE_CHECK = "vtypecheck";
    static const char* V_NAME_INTERNAL = "vname_internal";
    static const char* V_FILTER = "vfilter";
    static const char* V_ADD_LIST = "v_add_list";
    static const char* V_DROP_LIST = "v_drop_list";
    static const char* E_TYPE_CHECK = "etypecheck";
    static const char* E_NAME_INTERNAL = "ename_internal";
    static const char* E_FILTER = "efilter";
    static const char* E_ADD_LIST = "e_add_list";
    static const char* E_DROP_LIST = "e_drop_list";
    static const char* MEMBERSHIP_LIST = "membership_vid_list";
    static const char* _INTERNAL_ID = "_internal_id";
    static const char* _EXTERNAL_ID = "id";
    static const char* _ANY = "_ANY";
    static const char* _TYPE = "type";
    static const char* RA = "ra";
    static const char* RETURN_NAME = "return_name";
    static const char* OPTIONS = "ops";
    static const char* RA_TYPE = "ra_type";
    static const char* ALSO_DEPENDS_ON = "also_depends_on";
    static const char* RA_OPTIONS = "ra_options";
    static const char* PROJECTION_EXPR = "projection_expressions";
    static const char* PROJECTION_NAME = "projection_names";
    static const char* AGGREGATION = "aggregation";
    static const char* AGGREGATION_FUNC = "function";
    static const char* AGGREGATION_EXPR = "projection_expression";
    static const char* AGGREGATION_NAME = "projection_name";
    static const char* UNION_CT = "argument"; // union_ct
    static const char* STEPS = "steps";
    static const char* ANCHOR_NODE = "anchor_node";
    static const char* SELECT_FILTER = "filter";
    static const char* SELECT = "select";
    static const char* PROJECT = "project";
    static const char* GROUP = "group";
    static const char* UNION = "union";
    static const char* NOTHING = "base_type";
    static const uint32_t TYPE_ANY = 2147483647;
    static const char* AGG_MAX = "Max";
    static const char* AGG_MIN = "Min";
    static const char* AGG_SUM = "Sum";
    static const char* AGG_CNT = "Count";
    static const char* AGG_AVG = "Avg";
    static const char* AGG_DISTINCT = "Distinct";
    static const char* BASIC_CT = ".";
    static const char* INPUT_NAME = "input_name";
    static const char* OUTPUT = "output";
  }//GPATH_TOOLS
}//UDIMPL

#endif

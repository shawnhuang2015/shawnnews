
// ali_cm_graph_info.hpp
// define the basic graph schema configure info

#ifndef ALI_CM_GRAPH_INFO_HPP
#define ALI_CM_GRAPH_INFO_HPP

#include <string>
#include <sstream>
#include <vector>
#include <iostream>

namespace ALI_CM_GRAPH_LOADING {

    // define the two sepeartor here
    const char ALI_COLUMN_SEPERATOR = '\0';
    const char ALI_FIELD_SEPERATOR = '^';
    const char ALI_ATTR_SEPERATOR = ':';
    
    // some default value
    // NOTE: 1 will alway be the enumerator value of "normal"
    static const char *task_flag_default_val = "normal";

    // define the global vertex type id info here
    enum {V_T_ACCOUNT=0, V_T_MEDIA, V_T_SSN, V_T_MOBILE, V_T_BANK_CARD, V_T_TASK, \
          V_T_EVENT0, V_T_EVENT1, V_T_EVENT2, V_T_EVENT3, V_T_EVENT4, \
          V_T_EVENT5, V_T_EVENT6, V_T_EVENT7, V_T_EVENT8, V_T_EVENT9, \
          V_T_EVENT10, V_T_EVENT11, V_T_EVENT12, V_T_EVENT13, V_T_EVENT14, \
          V_T_EVENT15, V_T_EVENT16, V_T_EVENT17, V_T_EVENT18, V_T_EVENT19, \
          V_T_EVENT20, V_T_EVENT21, V_T_EVENT22, V_T_EVENT23, V_T_EVENT24, \
          V_T_EVENT25, V_T_EVENT26, V_T_EVENT27, V_T_EVENT28, V_T_EVENT29, \
          V_T_EVENT30, V_T_EVENT31, V_T_EVENT32, V_T_EVENT33, V_T_EVENT34};

    // edge type id info
    enum {E_T_TRUST_MEDIA=0, E_T_TRUST, E_T_BIND, E_T_REGISTER, E_T_USE,  \
          E_T_INCOME_CARD, E_T_OUTCOME_CARD, E_T_ACCT_RESULT, E_T_MEDIA_RESULT, \
          E_T_BANK_CARD_RESULT, E_T_SSN_RESULT, E_T_MOBILE_RESULT, E_T_HIST_TRANS_CARD};

    // enumerator id info
    enum {ENUM_PROVINCE=1, ENUM_TASK_FLAG, ENUM_MEDIA_TYPE, ENUM_IP_CITY, \
          ENUM_IP_LIST_ITEM, ENUM_EVENT_NAME, ENUM_EVENT_LABEL, ENUM_IS_SAVED_CARD};

    enum {BASE_YEAR = 2000, BASE_MONTH = 1, BASE_DAY = 1};

    enum {MAX_ROUND_DAYS = 31};

    // define the combine job type
    enum {ONLY_RAW_FILE_MSG, ONLY_REAL_FILE_MSG, BOTH_RAW_REAL_MSG};

    // define the loading COUT DEBUG info level
    enum {ALI_CM_LOADING_DEBUG_LEVEL=8};
}

#endif

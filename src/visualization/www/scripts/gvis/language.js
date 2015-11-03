var language = "english"; // "default", "english", "c".
//var language = "default"; // "default", "english", "chinese".

var language_lib_chinese = {
  add_word : function(key, value) {
    language_lib_chinese.text[key] = value;
  },
  add_type_word : function(key, value, type, attr) {
    attr = attr == undefined ? key : attr;
    if (type in language_lib_chinese.type) {
      if (attr in language_lib_chinese.type[type]) {
        language_lib_chinese.type[type][attr][key] = value;
      }
      else {
        language_lib_chinese.type[type][attr] = {};
        language_lib_chinese.type[type][attr][key] = value;
      }
    }
    else {
      language_lib_chinese.type[type] = {};
      if (attr in language_lib_chinese.type[type]) {
        language_lib_chinese.type[type][attr][key] = value;
      }
      else {
        language_lib_chinese.type[type][attr] = {};
        language_lib_chinese.type[type][attr][key] = value;
      }
      
    }
  },
  text : {},
  type : {}
};

var language_lib_english = {
  add_word : function(key, value) {
    language_lib_english.text[key] = value;
  },
  add_type_word : function(key, value, type, attr) {
    attr = attr == undefined ? key : attr;
    if (type in language_lib_english.type) {
      if (attr in language_lib_english.type[type]) {
        language_lib_english.type[type][attr][key] = value;
      }
      else {
        language_lib_english.type[type][attr] = {};
        language_lib_english.type[type][attr][key] = value;
      }
    }
    else {
      language_lib_english.type[type] = {};
      if (attr in language_lib_english.type[type]) {
        language_lib_english.type[type][attr][key] = value;
      }
      else {
        language_lib_english.type[type][attr] = {};
        language_lib_english.type[type][attr][key] = value;
      }
      
    }
  },
  text : {},
  type : {}
};

var language_key_chinese = {};

// language_lib_chinese.add_word("Transaction Lookup", "交易反查");
// language_lib_chinese.add_word("Start Time", "起始时间");
// language_lib_chinese.add_word("End Time", "终止时间");
language_lib_chinese.add_word("Depth", "查询深度");
language_lib_chinese.add_word("Max", "最大节点个数");
language_lib_chinese.add_word("Type", "节点类型");
language_lib_chinese.add_word("ID", "节点ID");
language_lib_chinese.add_word("input type", "输入类型");
language_lib_chinese.add_word("input id", "输入ID");
language_lib_chinese.add_word("input depth", "请输入查询深度");
language_lib_chinese.add_word("input max", "请输入最大节点数");
language_lib_chinese.add_word("Submit Query", "查询");
language_lib_chinese.add_word("Force Layout", "力学排列");
language_lib_chinese.add_word("Restart Force", "重置力学排列");
language_lib_chinese.add_word("Set Center View", "图置中");
language_lib_chinese.add_word("Set Tree Root", "设置树根节点");
language_lib_chinese.add_word("Tree Layout", "树型排列");
language_lib_chinese.add_word("Hierarchical Layout", "层叠排列");
language_lib_chinese.add_word("Set Hierarchical Root", "设置层叠起始节点");
language_lib_chinese.add_word("Circle Layout", "环形排列");
language_lib_chinese.add_word("Set Center Node", "设置环心节点");
language_lib_chinese.add_word("No Selected Node", "无选中节点");
language_lib_chinese.add_word("Summary Info of the Selected Nodes", "选中节点的摘要信息");
language_lib_chinese.add_word("All Types", "全部类型");
language_lib_chinese.add_word("Number of Selected Nodes", "选中节点个数");
language_lib_chinese.add_word("Number of Selected Edges", "选中边个数");
language_lib_chinese.add_word("Average In Degree", "平均入度");
language_lib_chinese.add_word("Average Out Degree", "平均出度");
language_lib_chinese.add_word("Number of Nodes", "节点个数");
language_lib_chinese.add_word("Number of Edges", "边个数");
language_lib_chinese.add_word("Input multiple types and ids", "请输入节点类型和节点ID");
language_lib_chinese.add_word("Summary Info of the Graph", "图摘要信息");
//["id", "orderTime", "merchantNumber", "cardHolder", "idCard",
// "userPhone", "bankCardNo", "flowId", "clientIp", "isBlacklisted"];
language_lib_chinese.add_word("id", "节点编号");
// language_lib_chinese.add_word("orderTime", "交易日期");
// language_lib_chinese.add_word("merchantNumber", "商品编号");
// language_lib_chinese.add_word("cardHolder", "持卡人姓名");
// language_lib_chinese.add_word("idCard", "身份证号");
// language_lib_chinese.add_word("userPhone", "用户电话号码");
// language_lib_chinese.add_word("bankCardNo", "用户银行卡号");
// language_lib_chinese.add_word("flowId", "交易流水号");
// language_lib_chinese.add_word("clientIp", "客户IP");
// language_lib_chinese.add_word("isBlacklisted", "黑交易");

//["Risk Level", "user", "client", "phone", "merchant", "bankcard", "transaction", ""]
// language_lib_chinese.add_word("Risk Level", "风险等级");
// language_lib_chinese.add_word("user", "用户");
// language_lib_chinese.add_word("client", "客户");
// language_lib_chinese.add_word("phone", "电话");
// language_lib_chinese.add_word("merchant", "商品");
// language_lib_chinese.add_word("bankcard", "银行卡");
// language_lib_chinese.add_word("transaction", "交易");

language_lib_chinese.add_word("Node Name", "节点名称");
language_lib_chinese.add_word("Link Name", "边名称");
language_lib_chinese.add_word("type", "类型");
//language_lib_chinese.add_word("outdegree", "相邻节点个数");
language_lib_chinese.add_word("orderEpochTime", "UNIX时间");
language_lib_chinese.add_word("name", "名称");


language_lib_chinese.add_word("No option selected ...", "没有信息项被选中");
language_lib_chinese.add_word("More than 4 options selected!", "多于4项信息被选中");
language_lib_chinese.add_word("More than 3 options selected!", "多于3项信息被选中");
language_lib_chinese.add_word("More than 2 options selected!", "多于2项信息被选中");
language_lib_chinese.add_word("Node id", "节点id");

language_lib_chinese.add_word("root node", "根节点");

language_lib_chinese.add_word("Query Type", "查询类型");
language_lib_chinese.add_word("Query ID", "查询ID");

language_lib_chinese.add_word("Unhighlight", "取消高亮");
language_lib_chinese.add_word("Highlight", "高亮显示");


language_lib_chinese.add_word("", "");
language_lib_chinese.add_word("", "");
language_lib_chinese.add_word("No Selected Node", "无选中节点");
language_lib_chinese.add_word("No Selected Node", "无选中节点");

language_lib_chinese.add_word("Node Label", "节点信息");
language_lib_chinese.add_word("Link Label", "连接信息");
language_lib_chinese.add_word("Options", "选项");
language_lib_chinese.add_word("Delete", "删除节点");
language_lib_chinese.add_word("Coloring", "修改颜色");

language_lib_chinese.add_word("Visualization", "可视化");
language_lib_chinese.add_word("JSON Message", "JSON信息");
language_lib_chinese.add_word("Graph Info", "图信息");
language_lib_chinese.add_word("Layout", "选择排列算法");


// Shandong Project Chinese Version.
language_lib_chinese.add_word("Phone Number", "电话号码");
language_lib_chinese.add_word("Source Phone Number", "呼出号码");
language_lib_chinese.add_word("Target Phone Number", "呼入号码");

language_lib_chinese.add_word("input phone number", "输入电话号码");
//language_lib_chinese.add_word("location", "地点");
language_lib_chinese.add_word("location", "附近使用人数");
language_lib_chinese.add_word("contact", "共同联系人");
language_lib_chinese.add_word("CF", "相同兴趣客户数");
language_lib_chinese.add_word("friend", "联系人");
language_lib_chinese.add_word("complaint", "投诉");
language_lib_chinese.add_word("reason", "推荐原因");
language_lib_chinese.add_word("enterprise", "集团");
language_lib_chinese.add_word("phone_no", "电话号码");
language_lib_chinese.add_word("gender", "性别");
language_lib_chinese.add_word("age", "年龄");
language_lib_chinese.add_word("datetime", "时间");
language_lib_chinese.add_word("user_id", "用户名");
language_lib_chinese.add_word("score", "相关用户数");
language_lib_chinese.add_type_word("score", "使用人数", "app", "score");
language_lib_chinese.add_word("count", "关联用户数");

//language_lib_chinese.add_type_word("english", "中文", "type", "attribute");

language_lib_chinese.add_word("app", "应用程序");

language_lib_chinese.add_word("male", "男");
language_lib_chinese.add_word("female", "女");

// User tag 
language_lib_chinese.add_word("op_time", "数据日期");
language_lib_chinese.add_word("product_no", "用户号码");
language_lib_chinese.add_word("user_id", "用户标识");
language_lib_chinese.add_word("city_id", "地市编码");
language_lib_chinese.add_word("open_date", "入网日期"); 
language_lib_chinese.add_word("enterprise_mark", "集团价值级别");
//1 A1 ；2 A2 ； 3 B1 ； 4 B2 ; 5 C ; 7 D1 ; 9 D2
language_lib_chinese.add_type_word("1", "A1", "phone_no", "enterprise_mark");
language_lib_chinese.add_type_word("2", "A2", "phone_no", "enterprise_mark");
language_lib_chinese.add_type_word("3", "B1", "phone_no", "enterprise_mark");
language_lib_chinese.add_type_word("4", "B2", "phone_no", "enterprise_mark");
language_lib_chinese.add_type_word("5", "C", "phone_no", "enterprise_mark");
language_lib_chinese.add_type_word("7", "D1", "phone_no", "enterprise_mark");
language_lib_chinese.add_type_word("9", "D2", "phone_no", "enterprise_mark");

language_lib_chinese.add_word("enkey_mark", "是否集团关键人");
language_lib_chinese.add_word("total_score", "可用积分总额");
language_lib_chinese.add_word("plan_mark", "是否套餐型客户");
language_lib_chinese.add_word("gprs_flow", "上月GPRS流量(mb)");
language_lib_chinese.add_word("arpu", "上月APRU");
language_lib_chinese.add_word("cal_arpu", "通话ARPU");
language_lib_chinese.add_word("local_cal_arpu", "市话ARPU");
language_lib_chinese.add_word("long_cal_arpu", "长途ARPU");
language_lib_chinese.add_word("roam_cal_arpu", "漫游ARPU");
language_lib_chinese.add_word("term_type", "用户终端类型");
language_lib_chinese.add_word("flow", "上月流量(mb)");
language_lib_chinese.add_word("gsm_flow", "上月2G流量");
language_lib_chinese.add_word("td_flow", "上月3G流量");
language_lib_chinese.add_word("lte_flow", "上月4G流量");
language_lib_chinese.add_word("age_id", "年龄");
/*
0 年龄不详
2 14岁以下
3 15-17岁 
4 18-20岁 
5 21-25岁 
6 26-30岁 
7 31-35岁 
8 36-40岁 
9 41-45岁 
10 46-50岁 
11 51-55岁 
12 56-60岁 
13 60岁以上
*/
language_lib_chinese.add_type_word("0", "年龄不详", "phone_no", "age_id");
language_lib_chinese.add_type_word("1", "年龄不详", "phone_no", "age_id");
language_lib_chinese.add_type_word("2", "14岁以下", "phone_no", "age_id");
language_lib_chinese.add_type_word("3", "15-17岁", "phone_no", "age_id");
language_lib_chinese.add_type_word("4", "18-20岁", "phone_no", "age_id");
language_lib_chinese.add_type_word("5", "21-25岁", "phone_no", "age_id");
language_lib_chinese.add_type_word("6", "26-30岁", "phone_no", "age_id");
language_lib_chinese.add_type_word("7", "31-35岁", "phone_no", "age_id");
language_lib_chinese.add_type_word("8", "36-40岁", "phone_no", "age_id");
language_lib_chinese.add_type_word("9", "41-45岁", "phone_no", "age_id");
language_lib_chinese.add_type_word("10", "46-50岁", "phone_no", "age_id");
language_lib_chinese.add_type_word("11", "51-55岁", "phone_no", "age_id");
language_lib_chinese.add_type_word("12", "56-60岁", "phone_no", "age_id");
language_lib_chinese.add_type_word("13", "60岁以上", "phone_no", "age_id");


language_lib_chinese.add_word("sex_id", "性别");
language_lib_chinese.add_word("user_online", "在网时长");
language_lib_chinese.add_word("area_mark", "用户区域");
//0不详 1城市用户,2县城用户,3农村用户
language_lib_chinese.add_type_word("0", "不详", "phone_no", "area_mark");
language_lib_chinese.add_type_word("1", "城市用户", "phone_no", "area_mark");
language_lib_chinese.add_type_word("2", "县城用户", "phone_no", "area_mark");
language_lib_chinese.add_type_word("3", "农村用户", "phone_no", "area_mark");

language_lib_chinese.add_word("fact_fee", "用户实际消费(元)");
language_lib_chinese.add_word("data_fee", "数据业务费（元）");
language_lib_chinese.add_word("enteract_mark", "是否集团统付");
language_lib_chinese.add_word("important_flag", "是否重要用户");
language_lib_chinese.add_word("content", "投诉内容");
language_lib_chinese.add_word("response_type", "投诉处理方案");
language_lib_chinese.add_word("date_time", "时间");
language_lib_chinese.add_word("is_potential", "潜在");
language_lib_chinese.add_word("importance", "是否重要客户"); //"1 关键人; 2 普通成员; 3 联系人"

language_lib_chinese.add_word("weight", "权重");
language_lib_chinese.add_word("user", "用户");
language_lib_chinese.add_word("device", "终端");
language_lib_chinese.add_word("group_count", "集团人数");
language_lib_chinese.add_word("Recommand", "推荐");
language_lib_chinese.add_word("Reason", "原因");

language_lib_chinese.add_type_word('Many people in the same location are using this APP.', '您周围的用户正在使用的app.', 'reason', 'reason')
language_lib_chinese.add_type_word('Some your friends are using this APP.', '您的朋友正在使用的app.', 'reason', 'reason')
language_lib_chinese.add_type_word('Collaborative filtering recommands this APP to you.', '与您兴趣相同的用户正在使用的app.', 'reason', 'reason')

/*************** Complaint ****************/
language_lib_chinese.add_word("warning", "预警");
language_lib_chinese.add_word("is_warning", "是否预警");
language_lib_chinese.add_word("solution", "投诉解决建议");
language_lib_chinese.add_word("phone_no_complaint", "投诉");

language_lib_chinese.add_type_word("count", "朋友越级人数", "friend", "count");
language_lib_chinese.add_type_word("count", "相关成员数", "enterprise", "count");
language_lib_chinese.add_type_word("count", "常住地越级人数", "location", "count");

language_lib_chinese.add_word("complaint_count", "次数");
language_lib_chinese.add_word("Complaint_Type_A", "越级");
language_lib_chinese.add_word("Complaint_Type_B", "一般");
language_lib_chinese.add_word("Complaint_Type_C", "咨询");
language_lib_chinese.add_word("Complaint_Type_D", "其他");

language_lib_chinese.add_word("srtype", "投诉类别");
language_lib_chinese.add_word("level", "投诉级别");

language_lib_chinese.add_type_word("outdegree", "案例总数", "srtype", "outdegree");
language_lib_chinese.add_type_word("srtype_name", "分类细则", "srtype", "srtype_name");
language_lib_chinese.add_type_word("category", "投诉分类", "srtype", "category");
/*
outdegree 175
srtype_name srtype_6
category  category_0
*/

language_lib_chinese.add_type_word("102", "预警原因", "102", "type");
language_lib_chinese.add_type_word("103", "预警原因", "103", "type");
language_lib_chinese.add_type_word("104", "一般", "104", "type");
language_lib_chinese.add_type_word("105", "投诉预警", "105", "type");
language_lib_chinese.add_type_word("106", "案例", "106", "type");

language_lib_chinese.add_type_word("108", "越级", "108", "type");

language_lib_chinese.add_type_word("109", "越级", "109", "type");

language_lib_chinese.add_word("complaint_srtype", "案例");
language_lib_chinese.add_word("srtype_solution", "处理建议");
language_lib_chinese.add_word("bypass_complaint", "越级投诉");

language_lib_chinese.add_type_word("bypass_complaint_count", "越级次数", "bypass_complaint", "bypass_complaint_count");
language_lib_chinese.add_type_word("Yes", "高危投诉客户", "warning", "is_warning");
language_lib_chinese.add_type_word("No", "普通投诉客户", "warning", "is_warning");



language_lib_chinese.add_word("phone_no_phone_no", "朋友圈");
language_lib_chinese.add_word("phone_no_enterprise", "集团");
language_lib_chinese.add_word("user_phone_no", "用户");
language_lib_chinese.add_word("phone_no_device", "终端");
language_lib_chinese.add_word("user_loc", "地点");
language_lib_chinese.add_word("device_web", "浏览");
language_lib_chinese.add_word("device_app", "上网");
language_lib_chinese.add_word("device_base_station", "常驻");
language_lib_chinese.add_word("base_station", "基站");
language_lib_chinese.add_word("web", "网页");
language_lib_chinese.add_type_word("type_name", "服务类型", "web");
language_lib_chinese.add_type_word("name", "网页名称", "web");
language_lib_chinese.add_word(" ", " ");


//language_lib_chinese.add_type_word("english", "中文", "type", "attribute");

language_lib_chinese.add_type_word("call_times", "通话次数", "phone_no_phone_no");

language_lib_chinese.add_type_word("outdegree", "成员个数", "enterprise", "outdegree");
language_lib_chinese.add_type_word("1", "关键人", "phone_no", "importance");
language_lib_chinese.add_type_word("2", "普通用户", "phone_no", "importance");
language_lib_chinese.add_type_word("3", "联系人", "phone_no", "importance");

language_lib_chinese.add_type_word("0", "否", "phone_no", "enkey_mark");
language_lib_chinese.add_type_word("1", "是", "phone_no", "enkey_mark");
language_lib_chinese.add_type_word("0", "否", "phone_no", "plan_mark");
language_lib_chinese.add_type_word("1", "是", "phone_no", "plan_mark");
language_lib_chinese.add_type_word("1", "济南", "phone_no", "city_id");
language_lib_chinese.add_type_word("1", "男", "phone_no", "sex_id");
language_lib_chinese.add_type_word("2", "女", "phone_no", "sex_id");
language_lib_chinese.add_type_word("0", "不详", "phone_no", "sex_id");

language_lib_chinese.add_type_word("1", "是", "phone_no", "enteract_mark");
language_lib_chinese.add_type_word("0", "否", "phone_no", "enteract_mark");

//0:未知,1:td智能机,2:4G智能机,3:其他智能机,4:非智能机
language_lib_chinese.add_type_word("0", "未知", "phone_no", "term_type");
language_lib_chinese.add_type_word("1", "td智能机", "phone_no", "term_type");
language_lib_chinese.add_type_word("2", "4G智能机", "phone_no", "term_type");
language_lib_chinese.add_type_word("3", "其他智能机", "phone_no", "term_type");
language_lib_chinese.add_type_word("4", "非智能机", "phone_no", "term_type");

//Phone Fraud 
language_lib_chinese.add_type_word("has_friend", "朋友圈", "phone_no", "has_friend");
language_lib_chinese.add_type_word("0", "不在朋友圈", "phone_no", "has_friend");
language_lib_chinese.add_type_word("1", "存在朋友圈", "phone_no", "has_friend");

language_lib_chinese.add_type_word("no_shortest_path", "距离超过4层关系", "no_shortest_path", "type");
language_lib_chinese.add_type_word("phone_call", "通话", "phone_call", "type");
language_lib_chinese.add_type_word("phone_no_on_path", "朋友", "phone_no_on_path", "type");
language_lib_chinese.add_type_word("outgoing_calls", "主叫次数", "phone_no", "outgoing_calls");
language_lib_chinese.add_type_word("fraction_of_callees_has_friends", "朋友主叫/主叫次数", "phone_no", "fraction_of_callees_has_friends");
language_lib_chinese.add_word("phone_no_on_path", "朋友");

language_lib_chinese.add_type_word("Customer 360", "用户全景", "tab", "tab name");
language_lib_chinese.add_type_word("Realtime", "实时用户全景", "tab", "tab name");
language_lib_chinese.add_type_word("Phone Fraud", "电话欺诈展示", "tab", "tab name");
language_lib_chinese.add_type_word("Recommandation", "智能推荐展示", "tab", "tab name");
language_lib_chinese.add_type_word("Complaint", "投诉查询，展示，和预警", "tab", "tab name");
language_lib_chinese.add_type_word("Potential", "用户潜在集团图谱", "tab", "tab name");
language_lib_chinese.add_type_word("Enterprise", "集团用户图谱", "tab", "tab name");
language_lib_chinese.add_type_word("Graph", "图结构展示", "tab", "tab name");

//language_lib_chinese.add_type_word("outdegree", "", "complaint");
//language_lib_chinese.add_type_word("outdegree", "", "srtype");

language_lib_chinese.add_type_word("english value", "中文", "type", "attribute name");
language_lib_chinese.add_type_word("english value", "中文", "type", "attribute name");
language_lib_chinese.add_type_word("english value", "中文", "type", "attribute name");
language_lib_chinese.add_type_word("english value", "中文", "type", "attribute name");
language_lib_chinese.add_type_word("english value", "中文", "type", "attribute name");
language_lib_chinese.add_type_word("english value", "中文", "type", "attribute name");


// Shandong Project English Version.
//language_lib_english.add_word("Phone Number", "电话号码");
language_lib_english.add_word("Source Phone Number", "Call-out Number");
language_lib_english.add_word("Target Phone Number", "Call-in Number");

//language_lib_english.add_word("input phone number", "输入电话号码");
//language_lib_english.add_word("location", "地点");
language_lib_english.add_word("location", "nearby people");
language_lib_english.add_word("contact", "sharing contact");
language_lib_english.add_word("CF", "same users");
language_lib_english.add_word("friend", "friends");
// language_lib_english.add_word("complaint", "投诉");
// language_lib_english.add_word("reason", "推荐原因");
// language_lib_english.add_word("enterprise", "集团");
language_lib_english.add_word("phone_no", "phone number");
// language_lib_english.add_word("gender", "性别");
// language_lib_english.add_word("age", "年龄");
// language_lib_english.add_word("datetime", "时间");
language_lib_english.add_word("user_id", "name");
language_lib_english.add_word("score", "related users");
language_lib_english.add_type_word("score", "users", "app", "score");
language_lib_english.add_word("count", "related users");

//language_lib_english.add_type_word("english", "中文", "type", "attribute");

// language_lib_english.add_word("app", "应用程序");

// language_lib_english.add_word("male", "男");
// language_lib_english.add_word("female", "女");

// User tag 
language_lib_english.add_word("op_time", "datetime");
language_lib_english.add_word("product_no", "user number");
language_lib_english.add_word("user_id", "user id");
language_lib_english.add_word("city_id", "city id");
language_lib_english.add_word("open_date", "open date");
language_lib_english.add_word("enterprise_mark", "enterprise mark");
//1 A1 ；2 A2 ； 3 B1 ； 4 B2 ; 5 C ; 7 D1 ; 9 D2
language_lib_english.add_type_word("1", "A1", "phone_no", "enterprise_mark");
language_lib_english.add_type_word("2", "A2", "phone_no", "enterprise_mark");
language_lib_english.add_type_word("3", "B1", "phone_no", "enterprise_mark");
language_lib_english.add_type_word("4", "B2", "phone_no", "enterprise_mark");
language_lib_english.add_type_word("5", "C", "phone_no", "enterprise_mark");
language_lib_english.add_type_word("7", "D1", "phone_no", "enterprise_mark");
language_lib_english.add_type_word("9", "D2", "phone_no", "enterprise_mark");

language_lib_english.add_word("enkey_mark", "is key person");
language_lib_english.add_word("total_score", "score");
language_lib_english.add_word("plan_mark", "is plan user");
language_lib_english.add_word("gprs_flow", "GPRS data(mb)");
language_lib_english.add_word("arpu", "APRU");
language_lib_english.add_word("cal_arpu", "calling ARPU");
language_lib_english.add_word("local_cal_arpu", "local ARPU");
language_lib_english.add_word("long_cal_arpu", "long ARPU");
language_lib_english.add_word("roam_cal_arpu", "roam ARPU");
language_lib_english.add_word("term_type", "device type");
language_lib_english.add_word("flow", "data(mb)");
language_lib_english.add_word("gsm_flow", "2G data");
language_lib_english.add_word("td_flow", "3G data");
language_lib_english.add_word("lte_flow", "4G data");
language_lib_english.add_word("age_id", "age");
/*
0 年龄不详
2 14岁以下
3 15-17岁 
4 18-20岁 
5 21-25岁 
6 26-30岁 
7 31-35岁 
8 36-40岁 
9 41-45岁 
10 46-50岁 
11 51-55岁 
12 56-60岁 
13 60岁以上
*/
language_lib_english.add_type_word("0", "unknown", "phone_no", "age_id");
language_lib_english.add_type_word("1", "unknown", "phone_no", "age_id");
language_lib_english.add_type_word("2", "14 or less", "phone_no", "age_id");
language_lib_english.add_type_word("3", "15-17yrs", "phone_no", "age_id");
language_lib_english.add_type_word("4", "18-20yrs", "phone_no", "age_id");
language_lib_english.add_type_word("5", "21-25yrs", "phone_no", "age_id");
language_lib_english.add_type_word("6", "26-30yrs", "phone_no", "age_id");
language_lib_english.add_type_word("7", "31-35yrs", "phone_no", "age_id");
language_lib_english.add_type_word("8", "36-40yrs", "phone_no", "age_id");
language_lib_english.add_type_word("9", "41-45yrs", "phone_no", "age_id");
language_lib_english.add_type_word("10", "46-50yrs", "phone_no", "age_id");
language_lib_english.add_type_word("11", "51-55yrs", "phone_no", "age_id");
language_lib_english.add_type_word("12", "56-60yrs", "phone_no", "age_id");
language_lib_english.add_type_word("13", "60yrs or more", "phone_no", "age_id");


language_lib_english.add_word("sex_id", "Gender");
language_lib_english.add_word("user_online", "online time");
language_lib_english.add_word("area_mark", "region id");
//0不详 1城市用户,2县城用户,3农村用户
language_lib_english.add_type_word("0", "unknown", "phone_no", "area_mark");
language_lib_english.add_type_word("1", "city user", "phone_no", "area_mark");
language_lib_english.add_type_word("2", "county user", "phone_no", "area_mark");
language_lib_english.add_type_word("3", "village user", "phone_no", "area_mark");

language_lib_english.add_word("fact_fee", "fact fee");
language_lib_english.add_word("data_fee", "data fee");
language_lib_english.add_word("enteract_mark", "is paied by enterprise");
language_lib_english.add_word("important_flag", "important");
language_lib_english.add_word("content", "complaint content");
language_lib_english.add_word("response_type", "response type");
language_lib_english.add_word("date_time", "date time");
language_lib_english.add_word("is_potential", "potential");
language_lib_english.add_word("importance", "importance"); //"1 关键人; 2 普通成员; 3 联系人"

// // language_lib_english.add_word("weight", "权重");
// language_lib_english.add_word("user", "用户");
// language_lib_english.add_word("device", "终端");
language_lib_english.add_word("group_count", "enterprise count");
// language_lib_english.add_word("Recommand", "推荐");
// language_lib_english.add_word("Reason", "原因");

/*************** Complaint ****************/
// language_lib_english.add_word("warning", "预警");
// language_lib_english.add_word("is_warning", "是否预警");
// language_lib_english.add_word("solution", "投诉解决建议");
language_lib_english.add_word("phone_no_complaint", "complaint");

language_lib_english.add_type_word("count", "friend bypass complaints", "friend", "count");
language_lib_english.add_type_word("count", "members", "enterprise", "count");
language_lib_english.add_type_word("count", "nearby bypass complaints", "location", "count");

language_lib_english.add_word("complaint_count", "times");
language_lib_english.add_word("Complaint_Type_A", "bypass");
language_lib_english.add_word("Complaint_Type_B", "general");
language_lib_english.add_word("Complaint_Type_C", "consult");
language_lib_english.add_word("Complaint_Type_D", "other");

language_lib_english.add_word("srtype", "complaint type");
language_lib_english.add_word("level", "complaint level");

language_lib_english.add_type_word("outdegree", "case numbers", "srtype", "outdegree");
language_lib_english.add_type_word("srtype_name", "category", "srtype", "srtype_name");
language_lib_english.add_type_word("category", "complaint type", "srtype", "category");
/*
outdegree 175
srtype_name srtype_6
category  category_0
*/

language_lib_english.add_type_word("102", "warning reason", "102", "type");
language_lib_english.add_type_word("103", "warning reason", "103", "type");
language_lib_english.add_type_word("104", "general", "104", "type");
language_lib_english.add_type_word("105", "complaint warning", "105", "type");
language_lib_english.add_type_word("106", "case", "106", "type");

language_lib_english.add_type_word("108", "bypass", "108", "type");

language_lib_english.add_type_word("109", "bypass", "109", "type");

language_lib_english.add_word("complaint_srtype", "case");
language_lib_english.add_word("srtype_solution", "response suggestion");
language_lib_english.add_word("bypass_complaint", "bypass complaint");

language_lib_english.add_type_word("bypass_complaint_count", "bypass times", "bypass_complaint", "bypass_complaint_count");
language_lib_english.add_type_word("Yes", "complaint warning", "warning", "is_warning");
language_lib_english.add_type_word("No", "normal", "warning", "is_warning");



language_lib_english.add_word("phone_no_phone_no", "friend");
language_lib_english.add_word("phone_no_enterprise", "enterprise");
language_lib_english.add_word("user_phone_no", "user");
language_lib_english.add_word("phone_no_device", "device");
language_lib_english.add_word("user_loc", "location");
language_lib_english.add_word("device_web", "browsing");
language_lib_english.add_word("device_app", "using");
language_lib_english.add_word("device_base_station", "stay");
language_lib_english.add_word("base_station", "base station");
// language_lib_english.add_word("web", "网页");
language_lib_english.add_type_word("type_name", "service type", "web");
language_lib_english.add_type_word("name", "web name", "web");
language_lib_english.add_word(" ", " ");


//language_lib_english.add_type_word("english", "中文", "type", "attribute");

language_lib_english.add_type_word("call_times", "calls", "phone_no_phone_no");

language_lib_english.add_type_word("outdegree", "members", "enterprise", "outdegree");
language_lib_english.add_type_word("1", "VIP", "phone_no", "importance");
language_lib_english.add_type_word("2", "Normal", "phone_no", "importance");
language_lib_english.add_type_word("3", "Key person", "phone_no", "importance");

language_lib_english.add_type_word("0", "No", "phone_no", "enkey_mark");
language_lib_english.add_type_word("1", "Yes", "phone_no", "enkey_mark");
language_lib_english.add_type_word("0", "No", "phone_no", "plan_mark");
language_lib_english.add_type_word("1", "Yes", "phone_no", "plan_mark");
language_lib_english.add_type_word("1", "JiNan", "phone_no", "city_id");
language_lib_english.add_type_word("1", "Male", "phone_no", "sex_id");
language_lib_english.add_type_word("2", "Female", "phone_no", "sex_id");
language_lib_english.add_type_word("0", "unknown", "phone_no", "sex_id");

language_lib_english.add_type_word("1", "Yes", "phone_no", "enteract_mark");
language_lib_english.add_type_word("0", "No", "phone_no", "enteract_mark");

//0:未知,1:td智能机,2:4G智能机,3:其他智能机,4:非智能机
language_lib_english.add_type_word("0", "unknown", "phone_no", "term_type");
language_lib_english.add_type_word("1", "td smart phone", "phone_no", "term_type");
language_lib_english.add_type_word("2", "4G smart phone", "phone_no", "term_type");
language_lib_english.add_type_word("3", "other smart phone", "phone_no", "term_type");
language_lib_english.add_type_word("4", "Not smart phone", "phone_no", "term_type");

//Phone Fraud 
language_lib_english.add_type_word("has_friend", "friend", "phone_no", "has_friend");
language_lib_english.add_type_word("0", "Not in friend list", "phone_no", "has_friend");
language_lib_english.add_type_word("1", "Exist in firend list", "phone_no", "has_friend");

language_lib_english.add_type_word("no_shortest_path", "more than 4 steps", "no_shortest_path", "type");
language_lib_english.add_type_word("phone_call", "call", "phone_call", "type");
language_lib_english.add_type_word("phone_no_on_path", "firend", "phone_no_on_path", "type");
language_lib_english.add_type_word("outgoing_calls", "call-out times", "phone_no", "outgoing_calls");
language_lib_english.add_type_word("fraction_of_callees_has_friends", "call friends/call-out times", "phone_no", "fraction_of_callees_has_friends");
language_lib_english.add_word("phone_no_on_path", "friend");



function toLanguage(str, type, attr) {
  attr = attr == undefined ? str : attr;
  
  try{
    if (language == "chinese") {
      if (type in language_lib_chinese.type && attr in language_lib_chinese.type[type]) {
        return language_lib_chinese.type[type][attr][str] == undefined ? toLanguage(str) : language_lib_chinese.type[type][attr][str];
      }
      else {
        return language_lib_chinese.text[str] == undefined ? "" + str : language_lib_chinese.text[str];
      }
    }
    else if (language == "english"){
      if (type in language_lib_english.type && attr in language_lib_english.type[type]) {
        return language_lib_english.type[type][attr][str] == undefined ? toLanguage(str) : language_lib_english.type[type][attr][str];
      }
      else {
        return language_lib_english.text[str] == undefined ? "" + str : language_lib_english.text[str];
      }
    }
    else if (language == "default"){
      return str;
    }
  }
  catch(err) {
    return "@" + str + "@";
  }
}

/*
function fromLanguage(str) {
  try{
    if (language == "chinese") {
      return language_key_chinese.text[str] == undefined ? str : language_key_chinese.text[str];
    }
    else if (language == "english" || language == "default"){
      return str;
    }
  }
  catch(err) {
    return str;
  }
}


// This piece of code should after all language_lib.add_word;
Object.keys(language_lib_chinese).forEach(function(key) {
  language_key_chinese[language_lib_chinese[key]] = key;
})
*/


//////// Other language related. ///////////////////

$('#nodelabel_label_id').html(toLanguage("Node Label"));
$('#linklabel_label_id').html(toLanguage("Link Label"));
$('#options_label_id').html(toLanguage("Options"));
$('#highlighting_label_id').html(toLanguage("Highlight"));
$('#delete_label_id').html(toLanguage("Delete"));
$('#coloring_label_id').html(toLanguage("Coloring"));
$('#visualization_label_id').html(toLanguage("Visualization"));
$('#jsonmessage_label_id').html(toLanguage("JSON Message"));
$('#graphinformation_label_id').html(toLanguage("Graph Info"));
$('#layout_label_id').html(toLanguage("Layout"));
$('#forcelayout_label_id').html(toLanguage("Force Layout"));
$('#treelayout_label_id').html(toLanguage("Tree Layout"));
$('#hierarchicallayout_label_id').html(toLanguage("Hierarchical Layout"));
$('#circlelayout_label_id').html(toLanguage("Circle Layout"));












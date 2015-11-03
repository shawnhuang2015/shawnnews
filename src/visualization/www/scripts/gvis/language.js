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


language_lib_chinese.add_type_word("english value", "中文", "type", "attribute name");
language_lib_chinese.add_type_word("english value", "中文", "type", "attribute name");
language_lib_chinese.add_type_word("english value", "中文", "type", "attribute name");
language_lib_chinese.add_type_word("english value", "中文", "type", "attribute name");
language_lib_chinese.add_type_word("english value", "中文", "type", "attribute name");
language_lib_chinese.add_type_word("english value", "中文", "type", "attribute name");

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












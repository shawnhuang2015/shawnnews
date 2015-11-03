var language = "chinese"; // "default", "english", "c".
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

//language_lib_chinese.add_type_word("english value", "中文", "type", "attribute name");

// language_lib_chinese.add_word("Transaction Lookup", "交易反查");
// language_lib_chinese.add_word("Start Time", "起始时间");
// language_lib_chinese.add_word("End Time", "终止时间");

language_lib_chinese.add_type_word("Graph Structure Lookup", "图结构展示", "ui_component", "label");
language_lib_chinese.add_type_word("Depth", "查询深度", "ui_component", "label");
language_lib_chinese.add_type_word("Max", "最大节点个数", "ui_component", "label");
language_lib_chinese.add_type_word("Type", "节点类型", "ui_component", "label");
language_lib_chinese.add_type_word("ID", "节点ID", "ui_component", "label");
language_lib_chinese.add_type_word("input type", "输入类型", "ui_component", "label");
language_lib_chinese.add_type_word("input id", "输入ID", "ui_component", "label");
language_lib_chinese.add_type_word("input depth", "请输入查询深度", "ui_component", "label");
language_lib_chinese.add_type_word("input max", "请输入最大节点数", "ui_component", "label");
language_lib_chinese.add_type_word("Submit Query", "查询", "ui_component", "label");
language_lib_chinese.add_type_word("Force Layout", "力学排列", "ui_component", "label");
language_lib_chinese.add_type_word("Restart Force", "重置力学排列", "ui_component", "label");
language_lib_chinese.add_type_word("Set Center View", "图置中", "ui_component", "label");
language_lib_chinese.add_type_word("Set Tree Root", "设置树根节点", "ui_component", "label");
language_lib_chinese.add_type_word("Tree Layout", "树型排列", "ui_component", "label");
language_lib_chinese.add_type_word("Hierarchical Layout", "层叠排列", "ui_component", "label");
language_lib_chinese.add_type_word("Set Hierarchical Root", "设置层叠起始节点", "ui_component", "label");
language_lib_chinese.add_type_word("Circle Layout", "环形排列", "ui_component", "label");
language_lib_chinese.add_type_word("Set Center Node", "设置环心节点", "ui_component", "label");
language_lib_chinese.add_type_word("No Selected Node", "无选中节点", "ui_component", "label");
language_lib_chinese.add_type_word("Summary Info of the Selected Nodes", "选中节点的摘要信息", "ui_component", "label");
language_lib_chinese.add_type_word("All Types", "全部类型", "ui_component", "label");
language_lib_chinese.add_type_word("Number of Selected Nodes", "选中节点个数", "ui_component", "label");
language_lib_chinese.add_type_word("Number of Selected Edges", "选中边个数", "ui_component", "label");
language_lib_chinese.add_type_word("Average In Degree", "平均入度", "ui_component", "label");
language_lib_chinese.add_type_word("Average Out Degree", "平均出度", "ui_component", "label");
language_lib_chinese.add_type_word("Number of Nodes", "节点个数", "ui_component", "label");
language_lib_chinese.add_type_word("Number of Edges", "边个数", "ui_component", "label");
language_lib_chinese.add_type_word("Input multiple types and ids", "请输入节点类型和节点ID", "ui_component", "label");
language_lib_chinese.add_type_word("Summary Info of the Graph", "图摘要信息", "ui_component", "label");

language_lib_chinese.add_type_word("id", "节点编号", "ui_component", "label");

language_lib_chinese.add_type_word("Node Name", "节点名称", "ui_component", "label");
language_lib_chinese.add_type_word("Link Name", "边名称", "ui_component", "label");
language_lib_chinese.add_type_word("type", "类型", "ui_component", "label");
language_lib_chinese.add_type_word("outdegree", "相邻节点个数", "ui_component", "label");
language_lib_chinese.add_type_word("orderEpochTime", "UNIX时间", "ui_component", "label");
language_lib_chinese.add_type_word("name", "名称", "ui_component", "label");

language_lib_chinese.add_type_word("No option selected ...", "没有信息项被选中", "ui_component", "label");
language_lib_chinese.add_type_word("More than 4 options selected!", "多于4项信息被选中", "ui_component", "label");
language_lib_chinese.add_type_word("More than 3 options selected!", "多于3项信息被选中", "ui_component", "label");
language_lib_chinese.add_type_word("More than 2 options selected!", "多于2项信息被选中", "ui_component", "label");
language_lib_chinese.add_type_word("Node id", "节点id", "ui_component", "label");

language_lib_chinese.add_type_word("root node", "根节点", "ui_component", "label");

language_lib_chinese.add_type_word("Query Type", "查询类型", "ui_component", "label");
language_lib_chinese.add_type_word("Query ID", "查询ID", "ui_component", "label");

language_lib_chinese.add_type_word("Unhighlight", "取消高亮", "ui_component", "label");
language_lib_chinese.add_type_word("Highlight", "高亮显示", "ui_component", "label");


language_lib_chinese.add_type_word("", "", "ui_component", "label");
language_lib_chinese.add_type_word("", "", "ui_component", "label");
language_lib_chinese.add_type_word("No Selected Node", "无选中节点", "ui_component", "label");
language_lib_chinese.add_type_word("No Selected Node", "无选中节点", "ui_component", "label");

language_lib_chinese.add_type_word("Node Label", "节点信息", "ui_component", "label");
language_lib_chinese.add_type_word("Link Label", "连接信息", "ui_component", "label");
language_lib_chinese.add_type_word("Options", "选项", "ui_component", "label");
language_lib_chinese.add_type_word("Delete", "删除节点", "ui_component", "label");
language_lib_chinese.add_type_word("Coloring", "修改颜色", "ui_component", "label");

language_lib_chinese.add_type_word("Visualization", "可视化", "ui_component", "label");
language_lib_chinese.add_type_word("JSON Message", "JSON信息", "ui_component", "label");
language_lib_chinese.add_type_word("Graph Info", "图信息", "ui_component", "label");
language_lib_chinese.add_type_word("Layout", "选择排列算法", "ui_component", "label");


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

$('#nodelabel_label_id').html(toLanguage("Node Label", "ui_component", "label"));
$('#linklabel_label_id').html(toLanguage("Link Label", "ui_component", "label"));
$('#options_label_id').html(toLanguage("Options", "ui_component", "label"));
$('#highlighting_label_id').html(toLanguage("Highlight", "ui_component", "label"));
$('#delete_label_id').html(toLanguage("Delete", "ui_component", "label"));
$('#coloring_label_id').html(toLanguage("Coloring", "ui_component", "label"));
$('#visualization_label_id').html(toLanguage("Visualization", "ui_component", "label"));
$('#jsonmessage_label_id').html(toLanguage("JSON Message", "ui_component", "label"));
$('#graphinformation_label_id').html(toLanguage("Graph Info", "ui_component", "label"));
$('#layout_label_id').html(toLanguage("Layout", "ui_component", "label"));
$('#forcelayout_label_id').html(toLanguage("Force Layout", "ui_component", "label"));
$('#treelayout_label_id').html(toLanguage("Tree Layout", "ui_component", "label"));
$('#hierarchicallayout_label_id').html(toLanguage("Hierarchical Layout", "ui_component", "label"));
$('#circlelayout_label_id').html(toLanguage("Circle Layout", "ui_component", "label"));












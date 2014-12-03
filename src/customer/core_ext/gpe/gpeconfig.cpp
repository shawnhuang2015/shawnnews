/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 * gpeconfig.cpp: /gdbms/src/customers/gpe_base/gpeconfig.cpp
 *
 *  Created on: Dec 10, 2013
 *      Author: lichen
 ******************************************************************************/

#include "gpeconfig.hpp"
#include <gutil/gstring.hpp>

namespace gperun {
  bool GPEConfig::enabledelta_ = true;
  bool GPEConfig::accuratedegree_ = false;
  std::string GPEConfig::udfmode_ = "";
  std::string GPEConfig::hostname_ = "";
  std::string GPEConfig::ipaddress_ = "";
  std::string GPEConfig::port_ = "";
  std::string GPEConfig::client_prefix_ = "";
  std::string GPEConfig::partitionpath_ = "";
  size_t GPEConfig::maxjobs_ = 0;
  size_t GPEConfig::num_post_threads_ = 0;
  float GPEConfig::get_request_timeoutsec_ = 0;
  float GPEConfig::prefetch_request_timeoutsec_ = 0;
  size_t GPEConfig::prefetch_request_limit_ = 0;
  std::string GPEConfig::log_path_ = "";
  std::string GPEConfig::get_request_queue_ = "";
  std::string GPEConfig::prefetch_request_queue_ = "";
  std::string GPEConfig::response_queue_ = "";
  std::string GPEConfig::delta_queue_ = "";
  int GPEConfig::rest_num_ = 0;
  int GPEConfig::queue_sleep_time_ = 0;
  std::string GPEConfig::zk_connection_ = "";
  std::string GPEConfig::kafka_connection_ = "";
  std::string GPEConfig::incoming_port_ = "";
  std::string GPEConfig::outgoing_port_to_ids_ = "";
  std::vector<std::string> GPEConfig::rest_incoming_host_strs_;
  std::vector<float> GPEConfig::udfweights_;
  GPEConfig::Maps_t GPEConfig::customizedsetttings_;
  topology4::RebuildSetting GPEConfig::rebuildsetting_;

  bool GPEConfig::CheckTimeOut(std::string requestid) {
    uint64_t current_ms = gutil::GTimer::GetTotalMilliSecondsSinceEpoch();
    uint64_t resttime_ms = gutil::extract_rest_request_time(requestid);
    GPROFILER(requestid) << "GPE|Run|resttime|"  << resttime_ms << "|gpetime|" << current_ms << "\n";
    if (resttime_ms > 0 && current_ms > (resttime_ms + (uint64_t) (get_request_timeoutsec_ * 1000)))
      return false;
    return true;
  }

  bool GPEConfig::SetConfig(std::string key, std::string value) {
    if (key == "queuesleeptime") {
      std::cout << "Update GPE config queue_sleep_time_ from "
                << queue_sleep_time_;
      queue_sleep_time_ = boost::lexical_cast<int>(value);
      std::cout << " to " << queue_sleep_time_ << "\n";
      return true;
    }
    if (key == "getrequesttimeoutsec") {
      std::cout << "Update GPE config get_request_timeoutsec_ from "
                << get_request_timeoutsec_;
      get_request_timeoutsec_ = boost::lexical_cast<float>(value);
      std::cout << " to " << get_request_timeoutsec_ << "\n";
      return true;
    }
    if (key == "prefetchrequesttimeoutsec") {
      std::cout << "Update GPE config prefetch_request_timeoutsec_ from "
                << prefetch_request_timeoutsec_;
      prefetch_request_timeoutsec_ = boost::lexical_cast<float>(value);
      std::cout << " to " << prefetch_request_timeoutsec_ << "\n";
      return true;
    }
    if (key == "prefetchrequestlimit") {
      std::cout << "Update GPE config prefetch_request_limit_ from "
                << prefetch_request_limit_;
      prefetch_request_limit_ = boost::lexical_cast<size_t>(value);
      std::cout << " to " << prefetch_request_limit_ << "\n";
      return true;
    }
    LOG(ERROR)<< "Invalid SetConfig command " << key << "," << value;
    return false;
  }

  YAML::Node GPEConfig::LoadConfig(std::string engineConfigFile) {
    udfweights_.resize(1, 1.0f);
    YAML::Node root = YAML::LoadFile(engineConfigFile.c_str());
    enabledelta_ = root["INSTANCE"]["enabledelta"].as<int>(1) != 0;
    accuratedegree_ = root["INSTANCE"]["accuratedegree"].as<int>(0) != 0;
    udfmode_ = root["INSTANCE"]["udfmode"].as<std::string>("");
    hostname_ = root["INSTANCE"]["name"].as<std::string>("");
    ipaddress_ = root["INSTANCE"]["ip"].as<std::string>("");
    port_ = root["INSTANCE"]["port"].as<std::string>("");
    client_prefix_ = root["INSTANCE"]["queue_client_name_prefix"]
                     .as<std::string>("");
    partitionpath_ =
        root["INSTANCE"]["graph_partition_snapshot"]["snapshot_path"]
        .as<std::string>("");
    maxjobs_ = root["INSTANCE"]["num_max_running_instances"].as<size_t>(4);
    num_post_threads_ = root["INSTANCE"]["num_post_threads"].as<size_t>(3);
    GASSERT(maxjobs_ > 0, maxjobs_);
    GASSERT(num_post_threads_ > 0, num_post_threads_);
    get_request_timeoutsec_ = root["INSTANCE"]["get_request_timeoutsec"]
                              .as<float>(10);
    prefetch_request_timeoutsec_ =
        root["INSTANCE"]["prefetch_request_timeoutsec"].as<float>(5);
    prefetch_request_limit_ = root["INSTANCE"]["prefetch_request_limit"]
                              .as<size_t>(12);

    /* set up the log path */
    std::string log_root_path = glib::io::to_absolute_path(
                                  root["INSTANCE"]["log_dir"].as<std::string>("logs"));
    if (*(log_root_path.rbegin()) != '/')
      log_root_path = log_root_path + "/";
    log_path_ = log_root_path + "gperun";
    std::string get_request_queue_name = "get_request_queue";
    if(udfmode_.size() > 0)
      get_request_queue_name = udfmode_ + "_" + get_request_queue_name;
    get_request_queue_ =
        root["QUEUE"][get_request_queue_name]["name"].as<std::string>("");
    prefetch_request_queue_ = root["QUEUE"]["prefetch_request_queue"]["name"]
                              .as<std::string>("");
    response_queue_ = root["QUEUE"]["response_queue"]["name"].as<std::string>("");
    rest_num_ = root["QUEUE"]["response_queue"]["rest_num"].as<int>(1);
    delta_queue_ = root["QUEUE"]["delta_queue"]["name"].as<std::string>("");
    queue_sleep_time_ = root["QUEUE"]["get_request_queue"]["timeout"].as<int>(50);
    zk_connection_ = gutil::yamlConnection2String(root["ZOOKEEPER"],
        "2181");
    kafka_connection_ = gutil::yamlConnection2String(root["KAFKA"],
        "9092");


    /* ------------------------------------------------------- */
    incoming_port_ = root["INSTANCE"]["incoming_port"].as<std::string>("");
    outgoing_port_to_ids_ = root["INSTANCE"]["outgoing_port_to_ids"].as<std::string>("");
    YAML::Node rest_hosts = root["REST"]["machines"];

    for (YAML::const_iterator it = rest_hosts.begin();
         it != rest_hosts.end(); ++it) {
      rest_incoming_host_strs_.push_back((*it)["ip"].as<std::string>("") + ":" + (*it)["incoming_port"].as<std::string>(""));
    }
    rebuildsetting_.sleep_no_job_ = root["INSTANCE"]["rebuild_nojob_sleepsec"].as<unsigned int>(60);
    rebuildsetting_.sleep_between_jobs_ = root["INSTANCE"]["rebuild_betweenjob_sleepsec"].as<unsigned int>(60);
    rebuildsetting_.sleep_switch_topology_ptr_ = root["INSTANCE"]["rebuild_switch_sleepsec"].as<unsigned int>(60);
    std::cout << "enabledelta: " << enabledelta_ << "\n"
              << "accuratedegree: " << accuratedegree_ << "\n"
              << "udfmode: " << udfmode_ << "\n"
              << "hostname: " << hostname_ << "\n" << "ipaddress: "
              << ipaddress_ << "\n" << "port: " << port_ << "\n"
              << "log directory: " << log_path_ << "\n"
              << "client_prefix: " << client_prefix_ << "\n"
              << "partitionpath: " << partitionpath_ << "\n" << "maxjobs: "
              << maxjobs_ << "\n" << "num_post_threads: "
              << num_post_threads_<< "\n" << "get_request_timeoutsec: "
              << get_request_timeoutsec_ << "\n"
              << "prefetch_request_timeoutsec: "
              << prefetch_request_timeoutsec_ << "\n"
              << "prefetch_request_limit: " << prefetch_request_limit_
              << "\n" << "get_request_queue: " << get_request_queue_
              << "\n" << "prefetch_request_queue: "
              << prefetch_request_queue_ << "\n" << "response_queue: "
              << response_queue_ << "\n" << "delta_queue: " << delta_queue_
              << "\n" << "rest_num: " << rest_num_ << "\n"
              << "\n" << "queue_sleep_time: " << queue_sleep_time_ << "\n"
              << "zk_connection: " << zk_connection_ << "\n"
              << "kafka_connection: " << kafka_connection_ << "\n"
              << " -------------------------------------------\n"
              << " gpe ip/port \n"
              << " -------------------------------------------\n"
              << " gpe incoming port: " << incoming_port_ << "\n"
              << " gpe outgoing port to ids : " << outgoing_port_to_ids_ << "\n";
    for(size_t i = 0; i<rest_incoming_host_strs_.size(); ++i) {
      std::cout << " rest " << (i+1) << " " << rest_incoming_host_strs_[i] << "\n";
    }
    std::cout << "\n";
    for(Maps_t::iterator it = customizedsetttings_.begin(); it != customizedsetttings_.end(); ++it)
      std::cout << it->first << ": " << it->second << "\n";
    std::cout << "\n";
    std::cout << rebuildsetting_;
    return root;
  }

}  // namespace gperun


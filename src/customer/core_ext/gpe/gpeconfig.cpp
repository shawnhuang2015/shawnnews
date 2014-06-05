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
#include "../../core_impl/gpe_impl/impl.hpp"

namespace gperun {
  std::string GPEConfig::hostname_ = "";
  std::string GPEConfig::ipaddress_ = "";
  std::string GPEConfig::port_ = "";
  std::string GPEConfig::client_prefix_ = "";
  std::string GPEConfig::partitionpath_ = "";
  size_t GPEConfig::maxjobs_ = 0;
  float GPEConfig::get_request_timeoutsec_ = 0;
  float GPEConfig::prefetch_request_timeoutsec_ = 0;
  size_t GPEConfig::prefetch_request_limit_ = 0;
  std::string GPEConfig::log_path_ = "";
  std::string GPEConfig::get_request_queue_ = "";
  std::string GPEConfig::prefetch_request_queue_ = "";
  std::string GPEConfig::response_queue_ = "";
  std::string GPEConfig::delta_queue_ = "";
  int GPEConfig::queue_sleep_time_ = 0;
  std::string GPEConfig::zk_connection_ = "";
  std::string GPEConfig::kafka_connection_ = "";
  std::string GPEConfig::incoming_port_ = "";
  std::string GPEConfig::outgoing_port_to_ids_ = "";
  std::vector<std::string> GPEConfig::rest_incoming_host_strs_;
  std::vector<float> GPEConfig::udfweights_;
  GPEConfig::Maps_t GPEConfig::customizedsetttings_;

  bool GPEConfig::CheckTimeOut(std::string requestid) {
    std::vector<std::string> strs;
    boost::split(strs, requestid, boost::is_any_of(":"));
    if (strs.size() != 4)
      return true;
    uint64_t ms = gutil::GTimer::GetTotalMilliSecondsSinceEpoch();
    uint64_t request_ms = boost::lexical_cast<uint64_t>(strs[2]);
    std::cout << "gpe time: " << ms << "\n";
    std::cout << "rest time: " << request_ms << "\n";
    if (strs[1] == "g") {
      if (ms
          > (request_ms + (uint64_t) (get_request_timeoutsec_ * 1000))) {
        std::cout << ms << "," << request_ms << ","
                  << get_request_timeoutsec_ << "\n";
        return false;
      }
    } else if (strs[1] == "p") {
      if (ms
          > (request_ms + (uint64_t) (prefetch_request_timeoutsec_ * 1000))) {
        std::cout << ms << "," << request_ms << ","
                  << prefetch_request_timeoutsec_ << "\n";
        return false;
      }
    }
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

  void GPEConfig::LoadConfig(std::string engineConfigFile) {
    udfweights_.resize(1, 1.0f);
    YAML::Node root = YAML::LoadFile(engineConfigFile.c_str());
    hostname_ = root["HOST"]["name"].as<std::string>("");
    ipaddress_ = root["HOST"]["ip"].as<std::string>("");
    port_ = root["HOST"]["port"].as<std::string>("");
    client_prefix_ = root["HOST"]["queue_client_name_prefix"]
                     .as<std::string>("");
    partitionpath_ =
        root["HOST"]["graph_partition_snapshot"]["snapshot_path"]
        .as<std::string>("");
    maxjobs_ = root["HOST"]["num_max_running_instances"].as<size_t>(4);
    get_request_timeoutsec_ = root["HOST"]["get_request_timeoutsec"]
                              .as<float>(10);
    prefetch_request_timeoutsec_ =
        root["HOST"]["prefetch_request_timeoutsec"].as<float>(5);
    prefetch_request_limit_ = root["HOST"]["prefetch_request_limit"]
                              .as<size_t>(12);

    /* set up the log path */
    std::string log_root_path = glib::io::to_absolute_path(
                                  root["HOST"]["log_dir"].as<std::string>("logs"));
    if (*(log_root_path.rbegin()) != '/')
      log_root_path = log_root_path + "/";
    log_path_ = log_root_path + "gperun";
    get_request_queue_ =
        root["QUEUE"]["get_request_queue"].as<std::string>("");
    prefetch_request_queue_ = root["QUEUE"]["prefetch_request_queue"]
                              .as<std::string>("");
    response_queue_ = root["QUEUE"]["response_queue"].as<std::string>("");
    delta_queue_ = root["QUEUE"]["delta_queue"].as<std::string>("");
    queue_sleep_time_ = root["QUEUE"]["sleep_time"].as<int>(50);
    zk_connection_ = gutil::yamlConnection2String(root["ZOOKEEPER"],
        "2181");
    kafka_connection_ = gutil::yamlConnection2String(root["KAFKA"],
        "9092");

 
    /* ------------------------------------------------------- */
    incoming_port_ = root["HOST"]["incoming_port"].as<std::string>("");
    outgoing_port_to_ids_ = root["HOST"]["outgoing_port_to_ids"].as<std::string>("");
    YAML::Node rest_hosts = root["REST"]["machines"];

    for (YAML::const_iterator it = rest_hosts.begin();
          it != rest_hosts.end(); ++it) {
      rest_incoming_host_strs_.push_back((*it)["ip"].as<std::string>("") + ":" + (*it)["incoming_port"].as<std::string>(""));

    }

    UDIMPL::GPE_UD_Impl::LoadCustimzedSetting(root, customizedsetttings_);

    std::cout << "hostname: " << hostname_ << "\n" << "ipaddress: "
              << ipaddress_ << "\n" << "port: " << port_ << "\n"
              << "log directory: " << log_path_ << "\n"
              << "client_prefix: " << client_prefix_ << "\n"
              << "partitionpath: " << partitionpath_ << "\n" << "maxjobs: "
              << maxjobs_ << "\n" << "get_request_timeoutsec: "
              << get_request_timeoutsec_ << "\n"
              << "prefetch_request_timeoutsec: "
              << prefetch_request_timeoutsec_ << "\n"
              << "prefetch_request_limit: " << prefetch_request_limit_
              << "\n" << "get_request_queue: " << get_request_queue_
              << "\n" << "prefetch_request_queue: "
              << prefetch_request_queue_ << "\n" << "response_queue: "
              << response_queue_ << "\n" << "delta_queue: " << delta_queue_
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
  }
}  // namespace gperun


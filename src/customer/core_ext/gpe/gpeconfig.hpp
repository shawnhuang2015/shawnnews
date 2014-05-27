/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 * gpeconfig.hpp: /gdbms/src/customers/gpe_base/gpeconfig.hpp
 *
 *  Created on: Dec 10, 2013
 *      Author: lichen
 ******************************************************************************/

#ifndef GPE_GPECONFIG_HPP_
#define GPE_GPECONFIG_HPP_

#include <gutil/gtimer.hpp>
#include <gutil/gyamlhelper.hpp>
#include <gutil/gsqlcontainer.hpp>
#include <lib/base_io.hpp>
#include <yaml-cpp/yaml.h>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include "../../core_impl/gpe_impl/impl.hpp"

namespace gperun {

/**
 * GPE server configuration.
 * POC modification: Likely since POC may need additional configurations
 */
struct GPEConfig {
    typedef GSQLMap_t<std::string, std::string> Maps_t;
    static std::string hostname_;
    static std::string ipaddress_;
    static std::string port_;
    static std::string client_prefix_;
    static std::string partitionpath_;
    static size_t maxjobs_;
    static std::string log_path_;
    static std::string get_request_queue_;
    static std::string prefetch_request_queue_;
    static std::string response_queue_;
    static std::string delta_queue_;
    static int queue_sleep_time_;
    static float get_request_timeoutsec_;
    static float prefetch_request_timeoutsec_;
    static size_t prefetch_request_limit_;
    static std::string zk_connection_;
    static std::string kafka_connection_;
    static std::vector<float> udfweights_;
    static Maps_t customizedsetttings_;

    static bool CheckTimeOut(std::string requestid) {
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

    static bool SetConfig(std::string key, std::string value) {
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

    static void LoadConfig(std::string engineConfigFile) {
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
                  << "kafka_connection: " << kafka_connection_ << "\n";
        for(Maps_t::iterator it = customizedsetttings_.begin(); it != customizedsetttings_.end(); ++it)
            std::cout << it->first << ": " << it->second << "\n";
        std::cout << "\n";
    }
};
}  // namespace gperun

#endif /* GPE_GPECONFIG_HPP_ */

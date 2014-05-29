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

    static bool CheckTimeOut(std::string requestid);

    static bool SetConfig(std::string key, std::string value);

    static void LoadConfig(std::string engineConfigFile);
};
}  // namespace gperun

#endif /* GPE_GPECONFIG_HPP_ */

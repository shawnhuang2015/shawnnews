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
#include <topology4/metafiles.hpp>

namespace gperun {

  /**
   * GPE server configuration.
   * POC modification: Likely since POC may need additional configurations
   */
  struct GPEConfig {
    typedef GSQLMap_t<std::string, std::string> Maps_t;
    /// enable delta or not. By default, it is true.
    static bool enabledelta_;
    /// udf mode of current process. Right now support "offline" and default "normal"
    static std::string udfmode_;
    static std::string hostname_;
    static std::string ipaddress_;
    static std::string port_;
    static std::string client_prefix_;
    static std::string partitionpath_;
    static size_t maxjobs_;
    static size_t num_post_threads_;
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
    static std::string incoming_port_;
    static std::string outgoing_port_to_ids_;
    static std::vector<std::string> rest_incoming_host_strs_;
    static Maps_t customizedsetttings_;
    static topology4::RebuildSetting rebuildsetting_;

    static bool CheckTimeOut(std::string requestid);

    static bool SetConfig(std::string key, std::string value);

    static YAML::Node LoadConfig(std::string engineConfigFile);
  };
}  // namespace gperun

#endif /* GPE_GPECONFIG_HPP_ */

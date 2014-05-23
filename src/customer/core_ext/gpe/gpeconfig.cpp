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
std::vector<float> GPEConfig::udfweights_;
}  // namespace gperun


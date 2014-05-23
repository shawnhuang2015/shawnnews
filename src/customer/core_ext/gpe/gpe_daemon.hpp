/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 *
 *  Created on: Oct 14, 2013
 *      Author: lichen
 ******************************************************************************/

#ifndef SRC_CUSTOMERS_GPE_DAEMON_H_
#define SRC_CUSTOMERS_GPE_DAEMON_H_

#include <BaseDaemon.h>
#include <gutil/dummyqueue.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include "gpeconfig.hpp"

namespace gperun {

/**
 * GPE server Daemon.
 * POC modification: Not Likely.
 */
class GPEDaemon : public gdist::BaseDaemon {
 public:
  GPEDaemon(string hostportStr, std::string rootPath)
      : BaseDaemon(hostportStr, rootPath) {
    type_ = "GPE";
    quit_ = false;
    rebuild_ = false;
    dummyqueue_ = NULL;
    listenerthread_ = NULL;
#ifdef ComponentTest
    dummyqueue_ = new gutil::DummyReadQueue("", "daemon",
        "", 0, 1, 0);
    listenerthread_ = new boost::thread(boost::bind(&GPEDaemon::run, this));
#endif
  }

  virtual ~GPEDaemon() {
    if (dummyqueue_ != NULL)
      delete dummyqueue_;
    if (listenerthread_ != NULL) {
      listenerthread_->join();
      delete listenerthread_;
    }
  }

  void run() {
    while (!quit_) {
      gdist::DQMessage* msg = dummyqueue_->read();
      handleCommand(msg->key);
      delete msg;
    }
  }

  /// received quit command or not
  bool quit_;
  /// received rebuild command or not
  bool rebuild_;

  /// start GPE daemon. After "connect" function.
  void StartGPEDaemon() {
    LOG(INFO)<< "startDaemon";
#ifndef ComponentTest
    BaseDaemon::startDaemon();
    std::vector<std::string> cfgs = getChildren(
        rootPath_ + "/global_config", false);
    for (size_t i = 0; i < cfgs.size(); ++i) {
      if (boost::starts_with(cfgs[i], "setgpeconfig_")) {
        SetGPEConfig(cfgs[i]);
      }
    }
#endif
  }

  void connect(std::string str, int timeout) {
#ifndef ComponentTest
    BaseDaemon::connect(str, timeout);
#endif
  }

  /// handle commands directed at this daemon (e.g. "guit", "rebuild")
  virtual void handleCommand(std::string command) {
    LOG(INFO)<< command << " command received";
#ifndef ComponentTest
    string cmd_path = rootPath_ + "/commands/" + daemonID_ + "/" + command;
    // remove /<root>/commands/<this_daemon>/<command>
    this->recursiveDelete(cmd_path.c_str());
#endif
    if (command.compare("quit") == 0) {
      quit_ = true;
      return;
    } else if (command.compare("rebuild") == 0) {
      rebuild_ = true;
      return;
    } else if (boost::starts_with(command, "setgpeconfig_")) {
      if (SetGPEConfig(command)) {
        return;
      }
    }
    LOG(ERROR)<< "Invalid GPEDaemon command " << command;
  }

  /// call back function when rebuild is done
  void DoneRebuild() {
#ifndef ComponentTest
    recursiveDelete(rootPath_ + "/global_status/gpe_rebuilding");
    time_t t = time(0);
    LOG(INFO)<< daemonID_ << " finished rebuild at timestamp " << t;
    stringstream ss;
    ss << t;
    string rebuildPath = rootPath_ + "/global_status/gpe_rebuild/"
    + hostportStr_;
    upsert(rebuildPath, ss.str());
#endif
    rebuild_ = false;
    LOG(INFO)<< "rebuild done";
  }

 private:
  /// the dummy queue for component testing
  gutil::DummyReadQueue* dummyqueue_;
  /// listen thread
  boost::thread* listenerthread_;

  bool SetGPEConfig(std::string command) {
    LOG(INFO)<< "set gpe config " << command;
    std::vector<std::string> strs;
    boost::split(strs, command, boost::is_any_of("_"));
    if (strs.size() == 3) {
      return GPEConfig::SetConfig(strs[1], strs[2]);
    }
    return false;
  }
};
}     // namespace gperun
#endif  // SRC_CUSTOMERS_GPE_DAEMON_H_

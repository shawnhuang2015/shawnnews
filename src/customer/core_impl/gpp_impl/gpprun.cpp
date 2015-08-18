/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 *
 *  Created on: Oct 14, 2013
 *      Author: lichen
 ******************************************************************************/

#include <gutil/statushubagent.hpp>
#include <gpp/serviceapi.hpp>

int main(int argc, char** argv) {
#ifdef BUILDVERSION
    std::string versStr = BUILDVERSION;
    std::replace(versStr.begin(), versStr.end(), ',', '\n');
    std::cerr << "GPP Server: Version\n" << versStr << std::endl;
#endif

  gutil::StatusHubAgentServer::Start();

  // you can use standard post filter or NULL or use your customized post filter here.
  PostFilterInterface* postfilterimpl = gperun::GPPServiceAPI::GetStandardPostFilter();
  gperun::GPPServiceAPI::Run(argc, argv, postfilterimpl);
  delete postfilterimpl;
}


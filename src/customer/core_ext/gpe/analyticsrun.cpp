//============================================================================
// Name        : gperun.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
// TODO fix this whole header
//============================================================================

#include "registry.hpp"
#include "../../core_impl/gpe_impl/impl.hpp"
#include <boost/program_options.hpp>
#include <fstream>
#include <gutil/glogging.hpp>

void printUsage(boost::program_options::options_description& desc, gperun::UDFRegistry& registry, bool specifiedCommand, std::string command) {
  std::cout << desc << std::endl;

  // Display specific commands available
  std::vector<std::string> availableCommands = registry.getCommands();
  std::cout << "Commands available (passed with -c) are:" << std::endl;
  for (unsigned int i = 0; i < availableCommands.size(); ++i) {
    std::cout << "  " << availableCommands[i] << std::endl;
  }
  std::cout << std::endl;

  // Display help for that command
  if (specifiedCommand) {
    if (registry.has(command)) {
      std::cout << registry[command]->optionsDescription << std::endl;
    }
  }
}

int main(int argc, char** argv) {

  //startup glog
  //GLogging_Init(argv[0], "/tmp/log/partition");
  GSQLLogger logger(argv[0], "/tmp/log/gpe");

  // Namespace alias for convenience
  namespace po = boost::program_options;

  // Set up the variables which will be parsed
  std::string command = "";
  std::string engineConfigFile = "";
  std::string topologyConfigFile = "";
  std::string outputFile = "";
  unsigned int port = 0;

  // Adds all the command line options to be parsed before calling the UDF runner
  po::options_description desc("Graph analytics command line options allowed");
  desc.add_options()
  ("help,h","prints the help message")
  ("server", "starts the engine in service mode")
  ("client", "starts the engine in client mode (for testing")
  ("port,p", po::value<unsigned int>(&port)->default_value(10001), "specifies the port for service or client mode")
  ("command,c", po::value<std::string>(&command), "the name of the UDF to run")
  ("enginefile,e", po::value<std::string>(&engineConfigFile), "the engine configuration file")
  ("topologyfile,f", po::value<std::string>(&topologyConfigFile), "the topology configuration file")
  ("outputfile,o", po::value<std::string>(&outputFile)->default_value("/tmp/graphsql/out.txt"), "the file to output to");

  // Parses the argumens
  po::variables_map arguments;
  po::store(po::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), arguments);
  po::notify(arguments);

  // Determines whether or not to display help
  bool displayHelp = (argc == 1) || (arguments.find("help") != arguments.end());
  bool specifiedCommand = (arguments.find("command") != arguments.end());

  // Sets the registry to be pure topologies only
  gperun::UDFRegistry& registry = gperun::Registries<char>::pureTopologyRegistry;
  UDIMPL::GPE_UD_Impl::registerAll();

  if (displayHelp) {
    // Display the help
    printUsage(desc, registry, specifiedCommand, command);
  } else {
    // Run the specified command or print an error message.
    if (registry.has(command)) {
      registry[command]->parseArguments(argc, argv);
      registry[command]->createUDF();
      //OLD USAGE:  give config file, path to partition directory, and algorithm
      registry[command]->runUDF(engineConfigFile,  topologyConfigFile, outputFile);
    } else {
      std::cout << "Command \"" << command << "\" is unrecognized. Please see the usage::" << std::endl;
      std::cout << desc << std::endl;
      printUsage(desc, registry, false, "");
    }
  }
  return 0;
}


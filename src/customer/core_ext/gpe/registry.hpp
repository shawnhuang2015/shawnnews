/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 * pagerank.hpp: /gperun/src/udf/pagerank.hpp
 *
 *  Created on: Jan 31, 2013
 *      Author: lichen, nicholas
 ******************************************************************************/

#ifndef REGISTRY_HPP_
#define REGISTRY_HPP_

#include <gpelib4/enginebase/baseudf.hpp>

#include <boost/unordered_map.hpp>
#include <boost/program_options.hpp>

using gpelib4::BaseUDF;

namespace gperun {

/**
 * This class describes the interface required for the
 * factories for creating UDFs. It will be placed in the
 * registry.
 */
class UDFRunner {
public:
  UDFRunner(std::string docstring = "Parameters for this UDF") : optionsDescription(docstring), udf(0) {
    }

    virtual ~UDFRunner() {
        if (udf != 0) {
            delete udf;
            udf = 0;
        }
    }

  void parseArguments(int argc, char** argv){
    namespace po = boost::program_options;
    po::store(po::command_line_parser(argc, argv).options(optionsDescription).allow_unregistered().run(), arguments);
    po::notify(arguments);
  }

    virtual void createUDF() = 0;
    virtual void runUDF(std::string engineConfigFile, std::string topologyConfigFile, std::string outputFile) = 0;
    BaseUDF* getUDF() { return udf; }

    boost::program_options::options_description optionsDescription;

protected:
    BaseUDF* udf;
    boost::program_options::variables_map arguments;

    template <typename T>
    void requires( const std::string& longArgument
                 , const std::string& shortArgument
                 , const std::string& description
                 , T* variable
                 , T defaultValue) {
        namespace po = boost::program_options;
        optionsDescription.add_options()
            ( (longArgument + "," + shortArgument).c_str()
            , po::value<T>(variable)->default_value(defaultValue)
            , description.c_str()
            );
    }

    template <typename T>
    void requires_list( const std::string& longArgument
                      , const std::string& shortArgument
                      , const std::string& description
                      , T* variable) {
        namespace po = boost::program_options;
        optionsDescription.add_options()
            ( (longArgument + "," + shortArgument).c_str()
            , po::value<T>(variable)->multitoken()
            , description.c_str()
            );
    }
};


/**
 * A UDF_Registry holds a map of UDFs and their required
 * arguments for running through a variety of interfaces.
 */
class UDFRegistry {
public:
    UDFRegistry() {

    }

    ~UDFRegistry(){
         for (boost::unordered_map<std::string,UDFRunner*>::const_iterator iter = UDFMap.begin();
                      iter != UDFMap.end(); ++iter) {
           delete iter->second;
         }
         UDFMap.clear();
    }

    void add(std::string udfname, UDFRunner* runner) {
        UDFMap[udfname] = runner;
    }

    UDFRunner* get(std::string udfname) {
        return UDFMap[udfname];
    }

    UDFRunner* operator[](std::string udfname) {
        return get(udfname);
    }

    bool has(std::string udfname) const {
        return UDFMap.find(udfname) != UDFMap.end();
    }

    std::vector<std::string> getCommands() const {
        std::vector<std::string> results;

        for (boost::unordered_map<std::string,UDFRunner*>::const_iterator iter = UDFMap.begin();
                iter != UDFMap.end(); ++iter) {
            results.push_back((*iter).first);
        }
        return results;
    }

private:
    boost::unordered_map<std::string, UDFRunner*> UDFMap;
};

template<typename tag>
class Registries {
public:
    static UDFRegistry pureTopologyRegistry;
};

template<typename tag>
UDFRegistry Registries<tag>::pureTopologyRegistry;

}

#endif

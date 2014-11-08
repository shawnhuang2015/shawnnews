/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.  
 * All rights reserved.  
 * Project: GraphSQL-GSE_2.0 --> POC 
 * loader_mapper.hpp: /src/customer/core_impl/gse_impl/loader_mapper.hpp 
 *
 *  Created on: Aug 21, 2014
 *  Author: Mingxi Wu 
 ******************************************************************************/

#ifndef SRC_CUSTOMER_COREIMPL_GSEIMPL_CANONNICAL_LOADERMAPPER_HPP_
#define SRC_CUSTOMER_COREIMPL_GSEIMPL_CANONNICAL_LOADERMAPPER_HPP_

#include <gse2/loader/gse_single_server_loader.hpp>
#include <gse2/loader/gse_single_server_loader.hpp>

namespace UDIMPL {

  /**
   * Override below when you need to customize the graph loading.
   * This is the mapper phase: user input line -> vertex/edge record
   */
  class GSE_CANONNICAL_LOADER: public gse2::GseSingleServerLoader {
    public:
      GSE_CANONNICAL_LOADER(gse2::WorkerConfig &wrkConfig, char separator)
        : gse2::GseSingleServerLoader(wrkConfig, separator) {
        }

      ~GSE_CANONNICAL_LOADER() {
      }

      /**
       * This is a local function to generate vertex attributes
       * @return Nothing
       */
      void GenVertexAttributeRecord() { }

      /**
       * Entry point to load the vertex source
       * @param[in] DataSource: contains data source from command line
       * @return Nothing
       */
      void LoadVertexData(std::vector<std::string> &DataSource) {

        vertexWriter_.start_counter();

        char* token_ptr;
        size_t  token_len;

        //initi a list of meta data 
        std::string fileName;
        char separator =',';
        bool primaryExisting = false;
        bool secondaryExisting = false;
        bool useSecondaryId = false;
        VERTEXID_T vid;
        uint32_t vertexTypeId;
        uint32_t maxTokenCount;

        //current vertex type code generation for person
        {
          separator = ',';
          useSecondaryId = false;
          vertexTypeId =0;
          //persons(id, name)
          maxTokenCount = 2; //starting from 0
          //for current vertex type, what are the parameters
          fileName = "/vagrant/data/TestData/persons.csv";

          char* positionMap[maxTokenCount];//which is max_index +1
          uint32_t tokenLen[maxTokenCount];
          //open first vertex  file
          fileReader_ = new gutil::FileLineReader(fileName);
            //skip header line
          fileReader_->MoveNextLine();

          //convert row to vertex  record
          while (fileReader_->MoveNextLine()) {

            primaryExisting = false;     
            secondaryExisting = false;     

            bool invalidLine = false;
            //find current line position map
            for(uint32_t i=0; i< maxTokenCount; i++) {
              //current line has less tokens, skip this line
              if (!fileReader_->NextString(token_ptr, token_len, separator)){
                invalidLine = true;
                break;
              }
              positionMap[i]=token_ptr;
              tokenLen[i]=token_len;
            }

            //skip the current line
            if (invalidLine) {
              continue;
            }

            //VERTEXID_T upsertNow(STRING_UID_T uid, uint32_t uidlen,  VTYPEID_T tid, bool &existing, bool primkey=true) 
            //create vertex based on primary_id only, primary_id len>0
            if (!useSecondaryId && tokenLen[0]>0) {
              vid = upsertNow(positionMap[0], tokenLen[0], vertexTypeId, primaryExisting);
              //ignore this line if duplicate primary_id
              if(primaryExisting){
                continue;
              }
              //assuming both primary_id and secondary_id present
            } else if (useSecondaryId && tokenLen[0]>0 && tokenLen[1]> 0){
              vid = upsertAlias(positionMap[0],tokenLen[0], positionMap[1], 
                  tokenLen[1], vertexTypeId, primaryExisting, secondaryExisting);
              //either one exist, skip this record
              if(primaryExisting || secondaryExisting){
                continue; 
              }
            } else {
              continue; 
            }


            //generate attributes
            //id: uint32
            positionMap[0][tokenLen[0]] = '\0';
            vertexWriter_.write((uint32_t)atoi(positionMap[0]));
            //name: string
            vertexWriter_.write(positionMap[1],tokenLen[1]);
            vertexWriter_.flush(vid);

          }//end while loop
          fileReader_->CloseFile();
          delete fileReader_;
        }//end person vertex

        //load movie vertex
        {
          separator = ',';
          useSecondaryId = false;
          vertexTypeId = 1;
          //persons(id, name)
          maxTokenCount = 4; //starting from 0
          //for current vertex type, what are the parameters
          fileName = "/vagrant/data/TestData/movies.csv";

          char* positionMap[maxTokenCount];//which is max_index +1
          uint32_t tokenLen[maxTokenCount];
          //open first vertex  file
          fileReader_ = new gutil::FileLineReader(fileName);
           //skip header line
          fileReader_->MoveNextLine();

          //convert row to vertex  record
          while (fileReader_->MoveNextLine()) {

            primaryExisting = false;     
            secondaryExisting = false;     

            bool invalidLine = false;
            //find current line position map
            for(uint32_t i=0; i< maxTokenCount; i++) {
              //current line has less tokens, skip this line
              if (!fileReader_->NextString(token_ptr, token_len, separator)){
                invalidLine = true;
                break;
              }
              positionMap[i]=token_ptr;
              tokenLen[i]=token_len;
            }

            //skip the current line
            if (invalidLine) {
              continue;
            }

            //VERTEXID_T upsertNow(STRING_UID_T uid, uint32_t uidlen,  VTYPEID_T tid, bool &existing, bool primkey=true) 
            //create vertex based on primary_id only, primary_id len>0
            if (!useSecondaryId && tokenLen[0]>0) {
              vid = upsertNow(positionMap[0], tokenLen[0], vertexTypeId, primaryExisting);
              //ignore this line if duplicate primary_id
              if(primaryExisting){
                continue;
              }
              //assuming both primary_id and secondary_id present
            } else if (useSecondaryId && tokenLen[0]>0 && tokenLen[1]> 0){
              vid = upsertAlias(positionMap[0],tokenLen[0], positionMap[1], 
                  tokenLen[1], vertexTypeId, primaryExisting, secondaryExisting);
              //either one exist, skip this record
              if(primaryExisting || secondaryExisting){
                continue; 
              }
            } else {
              continue; 
            }

            //generate attributes
            //id: uint32
            positionMap[0][tokenLen[0]] = '\0';
            vertexWriter_.write((uint32_t)atoi(positionMap[0]));
            //title: string
            vertexWriter_.write(positionMap[1],tokenLen[1]);
            //country: enumerator
            vertexWriter_.write((uint32_t)enumMappers_.encode(1, std::string(positionMap[2], tokenLen[2])));
            //year: uint32
            positionMap[3][tokenLen[3]] = '\0';
            vertexWriter_.write((uint32_t)atoi(positionMap[3]));
            vertexWriter_.flush(vid);
          }//end while loop
          fileReader_->CloseFile();
          delete fileReader_;
        }//end movie vertex

        vertexWriter_.stop_counter();

        /* 

        //string, bool, uint32_t , real, fixedbinary(n), string compress

        //string
        vertexWriter_.write(positionMap[2],tokenLen[2]);
        //bool 0/1
        positionMap[3][tokenLen[3]] = '\0';

        if (atoi(positionMap[3] ==0)){
        vertexWriter_.write(false);
        }
        else {
        vertexWriter_.write(true);
        }

        //real
        positionMap[4][tokenLen[4]] = '\0';
        vertexWriter_.write(atof(positionMap[4]));
        //int
        positionMap[5][tokenLen[5]] = '\0';
        vertexWriter_.write(atoi(positionMap[4]));

        //fixedbinary(n)
        vertexWriter_.write(tokenLen[6],positionMap[6]);

        //string compress
        vertexWriter_.write(enumMappers_.encode(1, std::string(positionMap[7], tokenLen[7])));

        vertexWriter_.flush(vid); */

      }//end loadVertex func

      /**
       * Standard function to generate edge attributes
       * @return Nothing
       */
      void GenEdgeAttributeRecord() { }


      /**
       * Entry point to load the edge source
       * @param[in] DataSource: contains data source from command line
       * @return Nothing
       */
      void LoadEdgeData(std::vector<std::string> &DataSource) {

        char* token_ptr;
        size_t  token_len;

        std::string fileName;
        uint32_t maxTokenCount; 
        char separator =',';
        uint32_t edgeTypeId =0;
        uint64_t srcId;
        uint64_t targetId;
        bool existingSrcVId=false;
        bool existingTargetVId=false;
        uint32_t srcVTypeId;
        uint32_t targetVTypeId;
        bool isDirected = false;

        //1. load edge DataSource[2] = roles.csv
        // roles (personID: INT, movieID: INT, name: STRING)
        edgeWriter_.start_counter();

        {
          fileName = "/vagrant/data/TestData/roles.csv";
          maxTokenCount = 3;
          separator = ',';
          edgeTypeId = 0;
          isDirected = true;
          srcVTypeId = 0;
          targetVTypeId = 1;

          char* positionMap[maxTokenCount];//which is max_index +1
          uint32_t tokenLen[maxTokenCount];

          //open first edge file
          fileReader_ = new gutil::FileLineReader(fileName);
          //skip header line
          fileReader_->MoveNextLine();



          //convert row to edge record
          while (fileReader_->MoveNextLine()) {

            existingSrcVId=false;
            existingTargetVId=false;

            bool invalidLine = false;
            //find current line position map
            for(uint32_t i=0; i< maxTokenCount; i++) {
              //current line has less tokens, skip this line
              if (!fileReader_->NextString(token_ptr, token_len, separator)){
                invalidLine = true;
                break;
              }
              positionMap[i]=token_ptr;
              tokenLen[i]=token_len;
            }

            if (invalidLine){
              continue;
            }

            //VERTEXID_T upsertNow(STRING_UID_T uid, uint32_t uidlen,  VTYPEID_T tid, bool &existing, bool primkey=true) 
            VERTEXID_T from_vid = upsertNow(positionMap[0], tokenLen[0], srcVTypeId, existingSrcVId);

            //create new vertex with default value attributes
            if (!existingSrcVId) {
              //default value  for src Vertex Attributes
              //id
              vertexWriter_.write((uint32_t)0);
              //name
              vertexWriter_.write("");
              vertexWriter_.flush(from_vid);
            }

            //VERTEXID_T upsertNow(STRING_UID_T uid, uint32_t uidlen,  VTYPEID_T tid, bool &existing, bool primkey=true) 
            VERTEXID_T to_vid = upsertNow(positionMap[1], tokenLen[1], targetVTypeId, existingTargetVId);

            //create new vertex with default value attributes
            if (!existingTargetVId) {
              //id
              vertexWriter_.write((uint32_t)0);
              //title
              vertexWriter_.write("");
              //country
              vertexWriter_.write((uint32_t)0);
              //year 
              vertexWriter_.write((uint32_t)0);
              //flush
              vertexWriter_.flush(to_vid);
            }

            //write edge attribute: role
            edgeWriter_.write(positionMap[2],tokenLen[2]);

            //flush edge with directed flag set to true
            edgeWriter_.flush(edgeTypeId, from_vid, to_vid, isDirected);

          }//end while

          fileReader_->CloseFile();
          delete fileReader_;
        }//end current edge

        edgeWriter_.stop_counter();
      }//end LoadEdgeData
  };//end class definition

}// namespace UDIMPL
#endif    /* SRC_CUSTOMER_COREIMPL_GSEIMPL_LOADERMAPPER_HPP_ */

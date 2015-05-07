#ifndef ALIEGFILTER_CPP
#define ALIEGFILTER_CPP

#include "ali_cm_graph_info.hpp"
#include "ali_load_event_file.hpp"
//#include "ali_load_event_file_old.hpp"
#include "../parse.hpp"


bool AliEgFilter(RequestInfo *request_info,
                              ProcessingResult *processing_result,
                              Handler *handler)
{
  //read tag and build reader
  char sep, eol;
  std::string eol_str;
  size_t tag_start_pos = 0;
  gutil::FileLineReader *filelinereader;
  std::string tag;
  if(request_info->data_length > 0 && request_info->data[0] == '#'){
    //parse separator and eol using the internal format
    sep = request_info->data[1];
    std::string eol_str = std::string(&request_info->data[2], 1);
    tag_start_pos = 4;

    size_t tag_end_pos = tag_start_pos + 1;
    while(tag_end_pos < request_info->data_length && request_info->data[tag_end_pos] != '\n')
      tag_end_pos ++;
    if(tag_end_pos == request_info->data_length){
      processing_result->error_message = "No CSV data found.";
      return false;
    }

    //parse the tag and get Canonnical_Loader_Config
    tag = std::string(&request_info->data[tag_start_pos], tag_end_pos - tag_start_pos);
    //prepare loader parameters
    //gse2::Canonnical_Loader_Config *loader_config = RESTPP_COMPONENTS::loadingjobmanager->GetConfig(tag);
    filelinereader = new gutil::FileLineReader(&request_info->data[tag_end_pos + 1], request_info->data_length - tag_end_pos - 1, eol_str);
  }
  else{
    //parse separator and eol using the standard format:
    /*
    separator="\1"  DATA_EOL= "\n" \n # this line is mandatory
    updateUser\n # this is the action tag
    20885021391231230x011230x011111\n # one data line, end with EOL
    20995021391231230x011450x011222\n # one data line, end with EOL
    \n\n # double EOL as end of block
    */
    char *header_eol = "\n";
    std::string temp_data(request_info->data, request_info->data_length);
    size_t separator_start_quote_pos = EatOrdered(temp_data, 0, "separator=", header_eol);
    if(separator_start_quote_pos == string::npos){
      processing_result->error_message = "separator not defined.";
      return false;
    }
    size_t separator_end_quote_pos = FindEndQuote(temp_data, separator_start_quote_pos + 1);
    if(separator_end_quote_pos == string::npos || 
       ParseSeparatorEol(temp_data.substr(separator_start_quote_pos,
			 separator_end_quote_pos - separator_start_quote_pos + 1), sep, false) == false){
      processing_result->error_message = "separator parse failed.";
      return false;
    }

    size_t eol_start_quote_pos = EatOrdered(temp_data, separator_end_quote_pos + 1, "DATA_EOL=", header_eol);
    if(eol_start_quote_pos == string::npos){
      processing_result->error_message = "DATA_EOL not defined.";
      return false;
    }
    size_t eol_end_quote_pos = FindEndQuote(temp_data, eol_start_quote_pos + 1);
    if(eol_end_quote_pos == string::npos ||
       ParseSeparatorEol(temp_data.substr(eol_start_quote_pos, eol_end_quote_pos - eol_start_quote_pos + 1), eol, true) == false){
      processing_result->error_message = "DATA_EOL parse failed.";
      return false;
    }
    //find tag
    size_t tag_start_pos = EatUnordered(temp_data, eol_end_quote_pos + 1, " \t\r\n");
    if(tag_start_pos == string::npos){
      processing_result->error_message = "tag parse failed.";
      return false;
    }
    size_t tag_end_pos = temp_data.find("\n", tag_start_pos);
    if(tag_end_pos == string::npos){
      processing_result->error_message = "tag parse failed.";
      return false;
    }
    tag = temp_data.substr(tag_start_pos, tag_end_pos - tag_start_pos);
    filelinereader = new gutil::FileLineReader(&temp_data[tag_end_pos + 1],
                                            temp_data.length() - tag_end_pos - 2, std::string(1, eol));
  }



  handler->csv_post_reader.Reset(filelinereader);
  handler->csv_post_writer.Reset(&processing_result->id_set);

  if(tag == "event"){
    ALI_CM_GRAPH_LOADING::AliEventFileLoader alievent_loader(&handler->csv_post_reader,
                                                           &handler->csv_post_writer,
                                                           &handler->delta_buffer_writer);
    alievent_loader.startLoad(processing_result->error_message);
  }
  else{
      processing_result->error_message = "Invalid tag.";
  }
  if(processing_result->buffer_capacity < handler->csv_post_writer.buffer_.length())
    processing_result->ExtendBuffer(handler->csv_post_writer.buffer_.length());
  memcpy(processing_result->buffer, handler->csv_post_writer.buffer_.buffer_, handler->csv_post_writer.buffer_.length());
  processing_result->buffer_length = handler->csv_post_writer.buffer_.bufferpos_
                                      - handler->csv_post_writer.buffer_.buffer_;
  processing_result->ids_action = IDS_UPSERT;

  delete filelinereader;
  return true;
}

#endif

/**
 * FileName: ali_load_realtime_account_file.hpp
 * Author:   Eric Chu
 * Date:     04/24/2015
 * Description: customerized loader for account_realtime.csv
 * History: 
 *  + create file. 04/24/2015
 */

#ifndef ALI_LOAD_REALTIME_ACCOUNT_FILE_HPP
#define ALI_LOAD_REALTIME_ACCOUNT_FILE_HPP

#include <gse2/config/canonnical_loader_config.hpp>
#include <gse2/base/gse_base_types.hpp>

#include <ctime>
#include <algorithm>
#include <boost/lexical_cast.hpp>

#include "ali_cm_graph_info.hpp"
#include "ali_cm_loader_utility.hpp"

using namespace gse2;

namespace ALI_CM_GRAPH_LOADING {
    class AliRealtimeAccountFileLoader {
    public:
        AliRealtimeAccountFileLoader(CSVPostReader *csv_post_reader, 
                                CSVPostWriter *csv_post_writer, 
                                gse2::GSE_DELTA_BUFFER *delta_buffer_writer) 
            : csv_post_reader_(csv_post_reader), 
              csv_post_writer_(csv_post_writer),
              delta_buffer_writer_(delta_buffer_writer) {
            GASSERT(NULL != csv_post_reader_, "csv_post_reader_ null pointer");
            GASSERT(NULL != csv_post_writer_, "csv_post_writer_ null pointer");
            GASSERT(NULL != delta_buffer_writer_, "delta_buffer_writer_ null pointer");
        }

        // @func: startLoading()
        // @desc: star the loading process, main entry
        bool startLoad(std::string &error_lines) {
            bool ret = true;
            error_lines = std::string("");
            int line_no = 0;
            while (csv_post_reader_->MoveNextLine()) {
                bool current_ret = loadOneLineData();
                if (ret) {
                    ret = current_ret;
                }
                if (!current_ret) {
                    error_lines += boost::lexical_cast<std::string>(line_no) + " ";
                }
                line_no ++;
            }
            //total number of lines in this request.
            error_lines += boost::lexical_cast<std::string>(line_no) + " ";
            return ret;
        }

        // @func: loadOneLineData()
        // @desc: only load the partil B accoutn atributes
        // @retr: if account id is not valid then skip the line
        // @note: format:
        //  false | is_certify | is_bind_M | is_exkt | is_corre_acc
        //  is_freezed | user_name | balance | freeze_amt | is_forbid
        //  create_time
        bool loadOneLineData() {
            char *p_uid = NULL;
            size_t uid_len = 0;
            if (!csv_post_reader_->NextString(p_uid, uid_len, 
                        ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            // check uid validation
            if (!isValidStrValue(p_uid, uid_len)) {
                return false;
            }

            delta_buffer_writer_->Reset();
            char *p_cont = NULL;
            size_t cont_len = 0;
            bool is_certify = false;
            if (!csv_post_reader_->NextString(p_cont, cont_len, 
                        ALI_COLUMN_SEPERATOR)) {
                return false;
            } else {
                std::string tmp(p_cont, cont_len);
                if (trim(tmp) == std::string("1")) {
                    is_certify = true;
                }
            }

            bool is_bind_m = false;
            if (!csv_post_reader_->NextString(p_cont, cont_len, 
                        ALI_COLUMN_SEPERATOR)) {
                return false;
            } else {
                std::string tmp(p_cont, cont_len);
                if (trim(tmp) == std::string("1")) {
                    is_bind_m = true;
                }
            }

            bool is_exkt = false;
            if (!csv_post_reader_->NextString(p_cont, cont_len, 
                        ALI_COLUMN_SEPERATOR)) {
                return false;
            } else {
                std::string tmp(p_cont, cont_len);
                if (trim(tmp) == std::string("1")) {
                    is_exkt = true;
                }
            }

            bool is_corre_acc = false;
            if (!csv_post_reader_->NextString(p_cont, cont_len, 
                        ALI_COLUMN_SEPERATOR)) {
                return false;
            } else {
                std::string tmp(p_cont, cont_len);
                if (trim(tmp) == std::string("1")) {
                    is_corre_acc = true;
                }
            }

            bool is_freezed = false;
            if (!csv_post_reader_->NextString(p_cont, cont_len, 
                        ALI_COLUMN_SEPERATOR)) {
                return false;
            } else {
                std::string tmp(p_cont, cont_len);
                if (trim(tmp) == std::string("1")) {
                    is_freezed = true;
                }
            }

            char *p_user_name = NULL;
            size_t user_name_len = 0;
            if (!csv_post_reader_->NextString(p_user_name, user_name_len,
                        ALI_COLUMN_SEPERATOR)) {
                return false;
            } 

            uint64_t balance;
            if (!csv_post_reader_->NextUnsignedLong(balance, 
                        ALI_COLUMN_SEPERATOR)) {
                return false;
            }


            uint64_t frozen_amt;
            if (!csv_post_reader_->NextUnsignedLong(frozen_amt, 
                        ALI_COLUMN_SEPERATOR)) {
                return false;
            }
            
            bool is_forbid = false;
            if (!csv_post_reader_->NextString(p_cont, cont_len, 
                        ALI_COLUMN_SEPERATOR)) {
                return false;
            } else {
                std::string tmp(p_cont, cont_len);
                if (trim(tmp) == std::string("1")) {
                    is_forbid = true;
                }
            }

            uint64_t create_time;
            if (!csv_post_reader_->NextUnsignedLong(create_time, 
                        ALI_COLUMN_SEPERATOR)) {
                return false;
            } 

            // star to write vertex info
            uint64_t default_uint = 0;
            float    default_float = 0.0f;
            delta_buffer_writer_->Write(uint32_t(0), true);
            delta_buffer_writer_->Write(create_time, false);
            delta_buffer_writer_->Write(default_float, true);
            delta_buffer_writer_->Write(default_uint, true);
            delta_buffer_writer_->Write(is_certify, false);
            delta_buffer_writer_->Write(is_bind_m, false);
            delta_buffer_writer_->Write(is_exkt, false);
            delta_buffer_writer_->Write(false, true);
            delta_buffer_writer_->Write(is_corre_acc, false);
            delta_buffer_writer_->Write(is_freezed, false);
            delta_buffer_writer_->Write(p_user_name, user_name_len, false);
            delta_buffer_writer_->Write(balance, false);
            delta_buffer_writer_->Write(default_uint, true);
            delta_buffer_writer_->Write(default_uint, true);
            delta_buffer_writer_->Write(frozen_amt, false);
            delta_buffer_writer_->Write(is_forbid, false);
            // write the default task_falt [normal]
            delta_buffer_writer_->Write((uint32_t)1, true);

            csv_post_writer_->FlushVertex(V_T_ACCOUNT,
                                delta_buffer_writer_->GetBufferHead(),
                                delta_buffer_writer_->GetBufferLength(),
                                p_uid,
                                uid_len,
                                false,
                                0,
                                0);
            return true;
        }

    private:
        CSVPostReader *csv_post_reader_;
        CSVPostWriter *csv_post_writer_;
        gse2::GSE_DELTA_BUFFER *delta_buffer_writer_;
    };
}

#endif


// ali_load_event_file.hpp
// Eric Chu
// 04/22/2015
// load the event csv


#ifndef ALI_LOAD_EVENT_FILE_HPP
#define ALI_LOAD_EVENT_FILE_HPP

#include <gse2/config/canonnical_loader_config.hpp>
#include <gse2/base/gse_base_types.hpp>

#include <ctime>
#include <algorithm>
#include <boost/lexical_cast.hpp>

#include "ali_cm_graph_info.hpp"
#include "ali_cm_loader_utility.hpp"

namespace ALI_CM_GRAPH_LOADING {
    class AliEventFileLoader {
    public:
        AliEventFileLoader( CSVPostReader *csv_post_reader_, 
                            CSVPostWriter *csv_post_writer_, 
                            gse2::GSE_DELTA_BUFFER *delta_buffer_writer_) 
            :csv_post_reader_(csv_post_reader_), 
              csv_post_writer_(csv_post_writer_), 
              delta_buffer_writer_(delta_buffer_writer_) {
            GASSERT(NULL != csv_post_reader_, "csv_post_reader_ null pointer");
            GASSERT(NULL != csv_post_writer_, "csv_post_writer_ null pointer");
            GASSERT(NULL != delta_buffer_writer_, "delta_buffer_writer_ null pointer");
        }

        // @param : error_lines: return error line no if have any 
        //          in format x,y,z
        // @return  false: loading has error
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
        // @return: false if load current error
        bool loadOneLineData() {
            if (!getEventVertexInfo()) {
                // no need to load the follwing media vertex
                return false;
            }

            bool ret = true;
            ret = writeAccountVertexEdge();
            ret = writeMediaVertexEdge("mac");
            ret = writeMediaVertexEdge("umid");
            ret = writeMediaVertexEdge("imei");
            ret = writeMediaVertexEdge("imsi");
            ret = writeMediaVertexEdge("tid");
            ret = writeMediaVertexEdge("pcid");
            ret = writeIpMediaVertexEdge();
            ret = writeMediaVertexEdge("vkeyid");
            ret = writeMediaVertexEdge("utdid");
            ret = writeMediaVertexEdge("wifimac");
            ret = writeMediaVertexEdge("wifi_node_name");
            ret = writeMediaVertexEdge("lbs");
            ret = writeMediaVertexEdge("cell_id");
            ret = writeMediaVertexEdge("ua");
            ret = writeMediaVertexEdge("gmac");

            ret = writeBankCardVertexEdge(true);
            ret = writeBankCardVertexEdge(false);
            // Event Vertex must to be written at last
            ret = writeEventVertexEdge();

            return ret;
        }

        // writeEventVertexEdge()
        // @describe: a. write one event vertex bin
        //            b. getthe event_uid_ and the event_vtype_id_
        // @return: false if found time stamp error
        bool getEventVertexInfo() {
            char *event_pid = NULL;
            size_t  pid_len = 0;
            csv_post_reader_->NextString(event_pid, pid_len, ALI_COLUMN_SEPERATOR);
            if (!isValidStrValue(event_pid, pid_len)) {
                return false;
            }

            event_uid_ = std::string(event_pid, pid_len);

            char *content = NULL;
            size_t  cont_len = 0;
            // get the event label
            csv_post_reader_->NextString(content, cont_len, ALI_COLUMN_SEPERATOR);
            event_label_id_ = csv_post_writer_->enum_mapper_->encode(ENUM_EVENT_LABEL, std::string(content, cont_len));

            // get the event name
            csv_post_reader_->NextString(content, cont_len, ALI_COLUMN_SEPERATOR);
            event_name_id_ = csv_post_writer_->enum_mapper_->encode(ENUM_EVENT_NAME, std::string(content, cont_len));

            // get the event occur time and day no
            csv_post_reader_->NextUnsignedLong(event_time_, ALI_COLUMN_SEPERATOR);

            int day_no = 0;
            if (!getEventDayNoByTime(event_time_, day_no)){
               return false; 
            }

            event_vtype_id_ = getEventVertexIdByDay(day_no);
            
            return true;
        }

        // here write the event vertex value,
        // as the is_saved_card attr is at the end of the line
        bool writeEventVertexEdge() {
            delta_buffer_writer_->Reset();

            char *content = NULL;
            size_t  cont_len = 0;
            // get the is_saved_card
            csv_post_reader_->NextString(content, cont_len, ALI_COLUMN_SEPERATOR);
            uint32_t is_saved_card_id = csv_post_writer_->enum_mapper_->encode(
                    ENUM_IS_SAVED_CARD, std::string(content, cont_len));

            // write the event name, label, time, is_saved
            delta_buffer_writer_->Write(event_name_id_, false);
            delta_buffer_writer_->Write(event_label_id_, false);
            delta_buffer_writer_->Write(event_time_, false);
            delta_buffer_writer_->Write(is_saved_card_id, false);
            
            csv_post_writer_->FlushVertex(event_vtype_id_,
                           delta_buffer_writer_->GetBufferHead(),
                           delta_buffer_writer_->GetBufferLength(),
                           const_cast<char *>(event_uid_.c_str()),
                           event_uid_.length(),
                           false,
                           0,
                           0);
            return true;
        }

        // writeAccountVertexEdge()
        // @describe: writeh account vertex with default value
        // @return: false if have any keyword error
        bool writeAccountVertexEdge() {
            delta_buffer_writer_->Reset();
            char *account_pid = NULL;
            size_t  pid_len = 0;
            // read $4 user_id
            csv_post_reader_->NextString(account_pid, pid_len, ALI_COLUMN_SEPERATOR);
            if (!isValidStrValue(account_pid, pid_len)) {
                return false;
            }

            uint64_t test_default = 0;
            // put a defaul value for account vertex
            // trust_province, create_time, risk_score, logout_time, is_certify, is_bind_mobile, 
            // has_ekt, is_merchant, has_correlation_account, is_frozen, user_name, balance, 
            // income_1m_amt, income_1m_kt_amt, frozen_amt, is_forbidden_pay
            delta_buffer_writer_->Write(uint32_t(0), true);
            delta_buffer_writer_->Write(test_default, true);
            delta_buffer_writer_->Write(0.0f, true);
            delta_buffer_writer_->Write(test_default, true);
            delta_buffer_writer_->Write(false, true);
            delta_buffer_writer_->Write(false, true);
            delta_buffer_writer_->Write(false, true);
            delta_buffer_writer_->Write(false, true);
            delta_buffer_writer_->Write(false, true);
            delta_buffer_writer_->Write(false, true);
            std::string tmp_str = std::string("");
            delta_buffer_writer_->Write(tmp_str, true);
            delta_buffer_writer_->Write(test_default, true);
            delta_buffer_writer_->Write(test_default, true);
            delta_buffer_writer_->Write(test_default, true);
            delta_buffer_writer_->Write(test_default, true);
            delta_buffer_writer_->Write(false, true);
            delta_buffer_writer_->Write(
                    (uint64_t)csv_post_writer_->enum_mapper_->encode(ENUM_TASK_FLAG,
                        std::string(task_flag_default_val)), true);

            csv_post_writer_->FlushVertex(V_T_ACCOUNT,
                           delta_buffer_writer_->GetBufferHead(),
                           delta_buffer_writer_->GetBufferLength(),
                           account_pid,
                           pid_len,
                           false,
                           0,
                           0);

            // flusht the event account edge
            csv_post_writer_->FlushEdge(E_T_USE,
                           event_vtype_id_,
                           V_T_ACCOUNT,
                           const_cast<char *>(event_uid_.c_str()),
                           event_uid_.length(),
                           NULL,
                           0,
                           account_pid,
                           pid_len,
                           NULL,
                           0,
                           NULL,
                           0,
                           false);
            return true;
        }

        // WriterMediaVertexEdge()
        // @param the specified media type, except ip
        // @return false: if has any error
        bool writeMediaVertexEdge(std::string media_type) {
            delta_buffer_writer_->Reset();

            char *media_pid = NULL;
            size_t  pid_len = 0;
            csv_post_reader_->NextString(media_pid, pid_len, ALI_COLUMN_SEPERATOR);
            if (!isValidStrValue(media_pid, pid_len)) {
                return false;
            }

            std::string media_id = media_type + "#" + std::string(media_pid, pid_len);

            // bin format: media_type, is_black, (ip_province, ip_city) only for ip
            uint32_t media_type_enumerator = csv_post_writer_->enum_mapper_->encode( \
                    ENUM_MEDIA_TYPE, media_type);
            delta_buffer_writer_->Write(media_type_enumerator, false);
            delta_buffer_writer_->Write(false, true);
            // the dummpy_enumerator will not be used by any UDFs.
            uint32_t dummy_enumerator = 0;
            delta_buffer_writer_->Write(dummy_enumerator, true);
            delta_buffer_writer_->Write(dummy_enumerator, true);

            csv_post_writer_->FlushVertex(V_T_MEDIA,
                           delta_buffer_writer_->GetBufferHead(),
                           delta_buffer_writer_->GetBufferLength(),
                           const_cast<char *>(media_id.c_str()),
                           media_id.length(),
                           false,
                           0,
                           0);

            // flush the event meida edge
            csv_post_writer_->FlushEdge(E_T_USE,
                           event_vtype_id_,
                           V_T_MEDIA,
                           const_cast<char *>(event_uid_.c_str()),
                           event_uid_.length(),
                           NULL,
                           0,
                           const_cast<char *>(media_id.c_str()),
                           media_id.length(),
                           NULL,
                           0,
                           NULL,
                           0,
                           false);
            return true;
        }

        // WriterIpMediaVertexEdge()
        // @return false: if has any error
        bool writeIpMediaVertexEdge() {
            delta_buffer_writer_->Reset();

            char *media_pid = NULL;
            size_t  pid_len = 0;
            csv_post_reader_->NextString(media_pid, pid_len, ALI_COLUMN_SEPERATOR);

            char *p_ip_province = NULL;
            size_t ip_province_len = 0;
            // get ip_province, ip_city
            csv_post_reader_->NextString(p_ip_province, ip_province_len, ALI_COLUMN_SEPERATOR);

            char *p_ip_city = NULL;
            size_t ip_city_len = 0;
            csv_post_reader_->NextString(p_ip_city, ip_city_len, ALI_COLUMN_SEPERATOR);

            if (!isValidStrValue(media_pid, pid_len)) {
                return false;
            }

            std::string media_id = "ip#" + std::string(media_pid, pid_len);

            // bin format: media_type, is_black, ip_province, ip_city
            std::string ip_media = std::string("ip");
            uint32_t ip_media_enumerator = csv_post_writer_->enum_mapper_->encode( \
                    ENUM_MEDIA_TYPE, ip_media);
            delta_buffer_writer_->Write(ip_media_enumerator, false);
            delta_buffer_writer_->Write(false, true);

            uint32_t ip_province_id = csv_post_writer_->enum_mapper_->encode( \
                    ENUM_PROVINCE, std::string(p_ip_province, ip_province_len));
            uint32_t ip_city_id = csv_post_writer_->enum_mapper_->encode( \
                    ENUM_IP_CITY, std::string(p_ip_city, ip_city_len));
            delta_buffer_writer_->Write(ip_province_id, false);
            delta_buffer_writer_->Write(ip_city_id, false);

            csv_post_writer_->FlushVertex(V_T_MEDIA,
                           delta_buffer_writer_->GetBufferHead(),
                           delta_buffer_writer_->GetBufferLength(),
                           const_cast<char *>(media_id.c_str()),
                           media_id.length(),
                           false,
                           0,
                           0);

            // flush the event ip meida edge
            csv_post_writer_->FlushEdge(E_T_USE,
                           event_vtype_id_,
                           V_T_MEDIA,
                           const_cast<char *>(event_uid_.c_str()),
                           event_uid_.length(),
                           NULL,
                           0,
                           const_cast<char *>(media_id.c_str()),
                           media_id.length(),
                           NULL,
                           0,
                           NULL,
                           0,
                           false);
            return true;
        }

        // @func: writeBankCardVertexEdge
        // @param: income_card: yes: the income_card
        // @return: false if has any error
        bool writeBankCardVertexEdge(bool income_card) {
            delta_buffer_writer_->Reset();
            char *pid = NULL;
            size_t pid_len = 0;
            csv_post_reader_->NextString(pid, pid_len, ALI_COLUMN_SEPERATOR);

            char *p_user_name = NULL;
            size_t user_name_len = 0;
            if (income_card) {
                csv_post_reader_->NextString(p_user_name, user_name_len, ALI_COLUMN_SEPERATOR);
            }

            if (!isValidStrValue(pid, pid_len)) {
                return false;
            }

            // is_balck, account_name
            delta_buffer_writer_->Write(false, false);
            std::string u_name_str = std::string(p_user_name, user_name_len);
            if (income_card) {
                delta_buffer_writer_->Write(u_name_str, false);
            } else {
                delta_buffer_writer_->Write(u_name_str, true);
            }

            csv_post_writer_->FlushVertex(V_T_BANK_CARD,
                           delta_buffer_writer_->GetBufferHead(),
                           delta_buffer_writer_->GetBufferLength(),
                           pid,
                           pid_len,
                           false,
                           0,
                           0);

            // flush the event bank_card edge
            if (income_card) {
                // income_card
                csv_post_writer_->FlushEdge(E_T_INCOME_CARD,
                           event_vtype_id_,
                           V_T_BANK_CARD,
                           const_cast<char *>(event_uid_.c_str()),
                           event_uid_.length(),
                           NULL,
                           0,
                           pid,
                           pid_len,
                           NULL,
                           0,
                           NULL,
                           0,
                           false);
            } else {
                // outcome_card 
                csv_post_writer_->FlushEdge(E_T_OUTCOME_CARD,
                           event_vtype_id_,
                           V_T_BANK_CARD,
                           const_cast<char *>(event_uid_.c_str()),
                           event_uid_.length(),
                           NULL,
                           0,
                           pid,
                           pid_len,
                           NULL,
                           0,
                           NULL,
                           0,
                           false);
            }
            return true;
        }

    private:
        CSVPostReader *csv_post_reader_;
        CSVPostWriter *csv_post_writer_;
        gse2::GSE_DELTA_BUFFER *delta_buffer_writer_;

        uint32_t    event_vtype_id_;
        std::string event_uid_;
        uint32_t event_label_id_;
        uint32_t event_name_id_;
        uint64_t event_time_;
    };
}

#endif

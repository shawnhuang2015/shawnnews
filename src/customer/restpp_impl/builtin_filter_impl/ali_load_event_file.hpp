
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
    struct EventLineRecord {
        char *p_event_id;
        size_t event_id_len;
        uint32_t event_label;
        uint32_t event_name;
        uint64_t event_time;
        char *p_user_id;
        size_t user_id_len;
        char *p_mac;
        size_t mac_len;
        char *p_umid;
        size_t umid_len;
        char *p_imei;
        size_t imei_len;
        char *p_imsi;
        size_t imsi_len;
        char *p_tid;
        size_t tid_len;
        char *p_pcid;
        size_t pcid_len;
        char *p_ip;
        size_t ip_len;
        char *p_ip_province;
        size_t ip_province_len;
        char *p_ip_city;
        size_t ip_city_len;
        char *p_vkeyid;
        size_t vkeyid_len;
        char *p_utdid;
        size_t utdid_len;
        char *p_wifimac;
        size_t wifimac_len;
        char *p_wifi_node_name;
        size_t wifi_node_name_len;
        char *p_lbs;
        size_t lbs_len;
        char *p_cell_id;
        size_t cell_id_len;
        char *p_ua;
        size_t ua_len;
        char *p_gmac;
        size_t gmac_len;
        char *p_income_card_no;
        size_t income_card_no_len;
        char *p_in_card_uname;
        size_t in_card_uname_len;
        char *p_bank_card_no;
        size_t bank_card_no_len;
        uint32_t is_saved_card;
    public:
        void reset() {
            memset(this, 0, sizeof(EventLineRecord));
        }
    };

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
            bool ret = true;

            if (!oneLineScan()) {
                // error skip the current line
                return false;
            }

            // make sure the event id is a valid one
            if (!isValidStrValue(line_record_.p_event_id, 
                        line_record_.event_id_len)) {
                return false;
            }

            // otherwise the data is intact

            ret = writeEventVertexEdge();
            if (!ret) {
                // write event error no need to continue
                return false;
            }
            writeAccountVertexEdge();
            writeMediaVertexEdge(line_record_.p_mac, 
                    line_record_.mac_len,"mac");
            writeMediaVertexEdge(line_record_.p_umid,
                    line_record_.umid_len, "umid");
            writeMediaVertexEdge(line_record_.p_imei, 
                    line_record_.imei_len, "imei");
            writeMediaVertexEdge(line_record_.p_imsi, 
                    line_record_.imsi_len, "imsi");
            writeMediaVertexEdge(line_record_.p_tid, 
                    line_record_.tid_len, "tid");
            writeMediaVertexEdge(line_record_.p_pcid, 
                    line_record_.pcid_len, "pcid");
            writeIpMediaVertexEdge();
            writeMediaVertexEdge(line_record_.p_vkeyid, 
                    line_record_.vkeyid_len, "vkeyid");
            writeMediaVertexEdge(line_record_.p_utdid, 
                    line_record_.utdid_len, "utdid");
            writeMediaVertexEdge(line_record_.p_wifimac,
                    line_record_.wifimac_len, "wifimac");
            writeMediaVertexEdge(line_record_.p_wifi_node_name, 
                    line_record_.wifi_node_name_len, "wifi_node_name");
            writeMediaVertexEdge(line_record_.p_lbs, 
                    line_record_.lbs_len, "lbs");
            writeMediaVertexEdge(line_record_.p_cell_id, 
                    line_record_.cell_id_len, "cell_id");
            writeMediaVertexEdge(line_record_.p_ua, 
                    line_record_.ua_len, "ua");
            writeMediaVertexEdge(line_record_.p_gmac, 
                    line_record_.gmac_len, "gmac");
            writeBankCardVertexEdge(true, 
                    line_record_.p_income_card_no, 
                    line_record_.income_card_no_len);
            writeBankCardVertexEdge(false, 
                    line_record_.p_bank_card_no,
                    line_record_.bank_card_no_len);

            return ret;
        }

        // writeEventVertexEdge()
        // here write the event vertex value,
        // as the is_saved_card attr is at the end of the line
        bool writeEventVertexEdge() {
            delta_buffer_writer_->Reset();

            int day_no = 0;
            if (!getEventDayNoByTime(line_record_.event_time, day_no)){
               return false; 
            }

            event_vtype_id_ = getEventVertexIdByDay(day_no);
            event_uid_ = std::string(line_record_.p_event_id, 
                    line_record_.event_id_len);

            // write the event name, label, time, is_saved
            delta_buffer_writer_->Write(line_record_.event_name, false);
            delta_buffer_writer_->Write(line_record_.event_label, false);
            delta_buffer_writer_->Write(line_record_.event_time, false);
            delta_buffer_writer_->Write(line_record_.is_saved_card, false);

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
            if (!isValidStrValue(line_record_.p_user_id, 
                        line_record_.user_id_len)) {
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
                           line_record_.p_user_id,
                           line_record_.user_id_len,
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
                           line_record_.p_user_id,
                           line_record_.user_id_len,
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
        bool writeMediaVertexEdge(char *p_media,
                                  size_t media_len,
                                  std::string media_type) {
            delta_buffer_writer_->Reset();

            if (!isValidStrValue(p_media, media_len)) {
                return false;
            }

            std::string media_id = media_type + "#" + std::string(p_media, media_len);

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

            if (!isValidStrValue(line_record_.p_ip, line_record_.ip_len)) {
                return false;
            }

            std::string media_id = "ip#" + 
                std::string(line_record_.p_ip, line_record_.ip_len);

            // bin format: media_type, is_black, ip_province, ip_city
            std::string ip_media = std::string("ip");
            uint32_t ip_media_enumerator = csv_post_writer_->enum_mapper_->encode( \
                    ENUM_MEDIA_TYPE, ip_media);
            delta_buffer_writer_->Write(ip_media_enumerator, false);
            delta_buffer_writer_->Write(false, true);

            uint32_t ip_province_id = csv_post_writer_->enum_mapper_->encode( \
                    ENUM_PROVINCE, std::string(line_record_.p_ip_province, 
                        line_record_.ip_province_len));
            uint32_t ip_city_id = csv_post_writer_->enum_mapper_->encode( \
                    ENUM_IP_CITY, std::string(line_record_.p_ip_city, 
                        line_record_.ip_city_len));
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
        bool writeBankCardVertexEdge(bool income_card, 
                                     char *p_card, 
                                     size_t len) {
            delta_buffer_writer_->Reset();
            if (!isValidStrValue(p_card, len)) {
                return false;
            }

            // is_balck, account_name
            delta_buffer_writer_->Write(false, true);
            std::string u_name_str;
            if (income_card) {
                u_name_str = std::string(
                        line_record_.p_in_card_uname, 
                        line_record_.in_card_uname_len);
                delta_buffer_writer_->Write(u_name_str, false);
            } else {
                u_name_str = std::string("");
                delta_buffer_writer_->Write(u_name_str, true);
            }

            csv_post_writer_->FlushVertex(V_T_BANK_CARD,
                           delta_buffer_writer_->GetBufferHead(),
                           delta_buffer_writer_->GetBufferLength(),
                           p_card,
                           len,
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
                           p_card,
                           len,
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
                           p_card,
                           len,
                           NULL,
                           0,
                           NULL,
                           0,
                           false);
            }
            return true;
        }

        // @fucn: oneLineScan
        // @desc: scan the current record line and mark at line_record_
        // @retu: false, current line has some error; default skipp current line
        bool oneLineScan() {
            char *p_cont = NULL;
            size_t cont_len = 0;
            line_record_.reset();

            if (!csv_post_reader_->NextString(line_record_.p_event_id, \
                        line_record_.event_id_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(p_cont, \
                        cont_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }
            line_record_.event_label = csv_post_writer_->enum_mapper_->encode(ENUM_EVENT_LABEL, std::string(p_cont, cont_len));

            if (!csv_post_reader_->NextString(p_cont, \
                        cont_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }
            line_record_.event_name = csv_post_writer_->enum_mapper_->encode(ENUM_EVENT_NAME, std::string(p_cont, cont_len));


            if (!csv_post_reader_->NextUnsignedLong(line_record_.event_time, \
                    ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_user_id, \
                        line_record_.user_id_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_mac, \
                        line_record_.mac_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_umid, \
                        line_record_.umid_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_imei, \
                        line_record_.imei_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_imsi, \
                        line_record_.imsi_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_tid, \
                        line_record_.tid_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_pcid, \
                        line_record_.pcid_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_ip, \
                        line_record_.ip_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_ip_province, \
                        line_record_.ip_province_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_ip_city, \
                        line_record_.ip_city_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_vkeyid, \
                        line_record_.vkeyid_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_utdid, \
                        line_record_.utdid_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_wifimac, \
                        line_record_.wifimac_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_wifi_node_name, \
                        line_record_.wifi_node_name_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_lbs, \
                        line_record_.lbs_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_cell_id, \
                        line_record_.cell_id_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_ua, \
                        line_record_.ua_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_gmac, \
                        line_record_.gmac_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_income_card_no, \
                        line_record_.income_card_no_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_in_card_uname, \
                        line_record_.in_card_uname_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_bank_card_no, \
                        line_record_.bank_card_no_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(p_cont, \
                        cont_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }
            line_record_.is_saved_card = csv_post_writer_->enum_mapper_->encode(ENUM_IS_SAVED_CARD, std::string(p_cont, cont_len));

            return true;
        }

    private:
        CSVPostReader *csv_post_reader_;
        CSVPostWriter *csv_post_writer_;
        gse2::GSE_DELTA_BUFFER *delta_buffer_writer_;

        EventLineRecord line_record_;
        uint32_t event_vtype_id_;
        std::string event_uid_;
    };
}

#endif

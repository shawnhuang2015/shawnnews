/**
 * FileName: ali_load_raw_account_file.hpp
 * Author:   Eric Chu
 * Date:     04/24/2015
 * Description: customerized loader for alipay_account.csv
 * History: 
 *  + create file. 04/24/2015
 */

#ifndef ALI_LOAD_RAW_ACCOUNT_FILE_HPP
#define ALI_LOAD_RAW_ACCOUNT_FILE_HPP

#include <gse2/config/canonnical_loader_config.hpp>
#include <gse2/base/gse_base_types.hpp>

#include <ctime>
#include <algorithm>
#include <boost/lexical_cast.hpp>

#include "ali_cm_graph_info.hpp"
#include "ali_cm_loader_utility.hpp"

using namespace gse2;

namespace ALI_CM_GRAPH_LOADING {
    struct RawAccountLineRecord {
        char *p_uid;
        size_t uid_len;
        char *p_ssn;
        size_t ssn_len;
        bool is_merchant;
        char *p_trust_province;
        size_t trust_province_len;
        char *p_bind_mobile;
        size_t bind_mobile_len;
        char *p_bind_card;
        size_t bind_card_len;
        char *p_trust_card;
        size_t trust_card_len;
        char *p_trust_mac;
        size_t trust_mac_len;
        char *p_trust_umid;
        size_t trust_umid_len;
        char *p_trust_imei;
        size_t trust_imei_len;
        char *p_trust_tid;
        size_t trust_tid_len;
        char *p_trust_ip3;
        size_t trust_ip3_len;
        char *p_trust_ua;
        size_t trust_ua_len;
        char *p_trust_lbs;
        size_t trust_lbs_len;
        char *p_trust_gmac;
        size_t trust_gmac_len;
        char *p_trust_vkeyid;
        size_t trust_vkeyid_len;
        char *p_trans_card;
        size_t trans_card_len;
        uint64_t income_1m_amt;
        uint64_t income_1m_kt_amt;
    public:
        void reset() {
            memset(this, 0, sizeof(RawAccountLineRecord));
        }
    };

    class AliRawAccountFileLoader {
    public:
        AliRawAccountFileLoader(CSVPostReader *csv_post_reader, 
                                CSVPostWriter *csv_post_writer,
                                gse2::GSE_DELTA_BUFFER *delta_buffer_writer)
            : csv_post_reader_(csv_post_reader),
              csv_post_writer_(csv_post_writer), 
              delta_buffer_writer_(delta_buffer_writer) {
            GASSERT(NULL != csv_post_reader_, "csv_post_reader null pointer");
            GASSERT(NULL != csv_post_writer_, "csv_post_writer null pointer");
            GASSERT(NULL != delta_buffer_writer_, "delta_buffer_writer null pointer");
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

        enum {TRUST_CARD, BIND_CARD, HIST_CARD};
    private:
        // @func: loadOneLineData()
        // @desc: load one line data of alipay_account.csv
        // @return: false found error while processing
        // @note: if account_id is not valid then skip the current line
        bool loadOneLineData() {
            if (!oneLineScan()) {
                // skip the current line;
                return false;
            }

            // check the validation of Account ID first
            if (!isValidStrValue(line_record_.p_uid, 
                        line_record_.uid_len)) {
                return false;
            }

            writeAccountVertex();
            writeSsnVertexEdge();
            writeMobileVertexEdges();
            writeBankCardVertexEdges(line_record_.p_bind_card, 
                    line_record_.bind_card_len, BIND_CARD);
            writeBankCardVertexEdges(line_record_.p_trust_card, 
                    line_record_.trust_card_len, TRUST_CARD);

            // write all the media
            writeMediaVertexEdges(line_record_.p_trust_mac, 
                    line_record_.trust_mac_len, "mac");
            writeMediaVertexEdges(line_record_.p_trust_umid, 
                    line_record_.trust_umid_len, "umid");
            writeMediaVertexEdges(line_record_.p_trust_imei, 
                    line_record_.trust_imei_len, "imei");
            writeMediaVertexEdges(line_record_.p_trust_tid, 
                    line_record_.trust_tid_len, "tid");
            writeMediaVertexEdges(line_record_.p_trust_ip3, 
                    line_record_.trust_ip3_len, "ip");
            writeMediaVertexEdges(line_record_.p_trust_ua, 
                    line_record_.trust_ua_len, "ua");
            writeMediaVertexEdges(line_record_.p_trust_lbs, 
                    line_record_.trust_lbs_len, "lbs");
            writeMediaVertexEdges(line_record_.p_trust_gmac, 
                    line_record_.trust_gmac_len, "gmac");
            writeMediaVertexEdges(line_record_.p_trust_vkeyid, 
                    line_record_.trust_vkeyid_len, "vkeyid");

            writeBankCardVertexEdges(line_record_.p_trans_card, 
                    line_record_.trans_card_len, HIST_CARD);

            return true;
        }

        // @func: writeAccountVertex()
        // @desc: write one partial account vertex 
        // @return: false processing error
        // @note: special handling
        // output: trust_province|create_time|0.0|0|is_certify|is_bind_m|is_exkt|
        //            is_merchant|is_corre|is_freezed|user_name|balance|
        //            1m_amt|1m_kt_amt|freeze_amt|is_forbid|task_flag
        bool writeAccountVertex() {
            delta_buffer_writer_->Reset();
            char *p_cont = NULL;
            size_t cont_len = 0;
            uint64_t default_uint = 0;
            float    default_float = 0.0f;


            // process trust province
            std::vector<uint32_t> province_id_list;
            StrTokenizer st = StrTokenizer(line_record_.p_trust_province, 
                    line_record_.trust_province_len);
            while (st.NextString(p_cont, cont_len) && cont_len > 0) {
                province_id_list.push_back(
                            csv_post_writer_->enum_mapper_->encode(ENUM_PROVINCE, 
                            std::string(p_cont, cont_len)));
            }
            
            // star to write the unit_set
            delta_buffer_writer_->Write((uint32_t)province_id_list.size(), province_id_list.size() == 0);
            for (size_t i = 0; i < province_id_list.size(); ++i) {
                delta_buffer_writer_->WriteUintSet_Value(province_id_list[i]);
            }

            delta_buffer_writer_->Write(default_uint, true);
            delta_buffer_writer_->Write(default_float, true);
            delta_buffer_writer_->Write(default_uint, true);

            delta_buffer_writer_->Write(false, true);
            delta_buffer_writer_->Write(false, true);
            delta_buffer_writer_->Write(false, true);

            delta_buffer_writer_->Write(line_record_.is_merchant, false);
            delta_buffer_writer_->Write(false, true);
            delta_buffer_writer_->Write(false, true);
            std::string temp_string = std::string("");
            delta_buffer_writer_->Write(temp_string, true);
            delta_buffer_writer_->Write(default_uint, true);

            delta_buffer_writer_->Write(line_record_.income_1m_amt, false);
            delta_buffer_writer_->Write(line_record_.income_1m_kt_amt, false);
            delta_buffer_writer_->Write(default_uint, true);
            delta_buffer_writer_->Write(false, true);

            // write the default task_flag value
            delta_buffer_writer_->Write((uint32_t)csv_post_writer_->enum_mapper_->encode(ENUM_TASK_FLAG, 
                        std::string(task_flag_default_val)), false);

            csv_post_writer_->FlushVertex(V_T_ACCOUNT, 
                                delta_buffer_writer_->GetBufferHead(),
                                delta_buffer_writer_->GetBufferLength(),
                                line_record_.p_uid, 
                                line_record_.uid_len,
                                false,
                                0,
                                0);

            return true;
        }

        // @func: writeSsnVertexEdge()
        // @desc: write the SSN vertex and
        // @retr: false error
        bool writeSsnVertexEdge() {
            delta_buffer_writer_->Reset();
            if (!isValidStrValue(line_record_.p_ssn, 
                        line_record_.ssn_len)) {
                return false;
            }
            // write the defaul is_back value
            delta_buffer_writer_->Write(false, true);
            csv_post_writer_->FlushVertex(V_T_SSN,
                                delta_buffer_writer_->GetBufferHead(),
                                delta_buffer_writer_->GetBufferLength(),
                                line_record_.p_ssn,
                                line_record_.ssn_len,
                                false,
                                0,
                                0);

            // write register edge info
            csv_post_writer_->FlushEdge(E_T_REGISTER,
                              V_T_ACCOUNT,
                              V_T_SSN,
                              line_record_.p_uid,
                              line_record_.uid_len,
                              NULL,
                              0,
                              line_record_.p_ssn,
                              line_record_.ssn_len,
                              NULL,
                              0,
                              NULL,
                              0,
                              false);
            
            return true;
        }

        // @func: writeMobileVertexEdges()
        // @desc: writhe the mobile vertex and edge list
        // @retr: always ture, just ignore the dirty data
        bool writeMobileVertexEdges() {
            char *p_cont = NULL;
            size_t cont_len = 0;

            // process mobile list
            StrTokenizer st = StrTokenizer(line_record_.p_bind_mobile, 
                                    line_record_.bind_mobile_len);
            while (st.NextString(p_cont, cont_len) && cont_len > 0) {
                if (!isValidStrValue(p_cont, cont_len)) {
                    continue;
                }

                // prepare the vertex and edge
                delta_buffer_writer_->Reset();
                delta_buffer_writer_->Write(false, true);
                csv_post_writer_->FlushVertex(V_T_MOBILE,
                                    delta_buffer_writer_->GetBufferHead(),
                                    delta_buffer_writer_->GetBufferLength(),
                                    p_cont,
                                    cont_len,
                                    false,
                                    0,
                                    0);
                
                csv_post_writer_->FlushEdge(E_T_BIND,
                                  V_T_ACCOUNT,
                                  V_T_MOBILE,
                                  line_record_.p_uid,
                                  line_record_.uid_len,
                                  NULL,
                                  0,
                                  p_cont,
                                  cont_len,
                                  NULL,
                                  0,
                                  NULL,
                                  0,
                                  false);
            }

            return true;
        }

        // @func: writeBankCardVertexEdges()
        // @desc: write the bank card info
        // @para: p_card: card_info list
        //        len: info length
        //        car_type: card type info
        // @retr: alwyas return ture, ignore the dirty data
        bool writeBankCardVertexEdges(char *p_card, 
                size_t len, int card_type) {
            char *p_cont = NULL;
            size_t cont_len = 0;

            // process the bank card list
            StrTokenizer st = StrTokenizer(p_card, len);
            while (st.NextString(p_cont, cont_len) && cont_len > 0) {
                // check whether NULL first
                if (!isValidStrValue(p_cont, cont_len)) {
                    continue;
                }

                // reset the writer buffer
                delta_buffer_writer_->Reset();

                char *p_card = NULL;
                size_t card_info_len = 0;
                StrTokenizer bk_st = StrTokenizer(p_cont, cont_len);
                // read bank card no
                if (!(bk_st.NextString(p_card, 
                           card_info_len, ALI_ATTR_SEPERATOR) 
                        && card_info_len > 0)) {
                    continue;
                }

                delta_buffer_writer_->Write(false, true);

                // read name_info
                std::string emp_str("");
                if (card_type == HIST_CARD) {
                    delta_buffer_writer_->Write(emp_str, true);
                } else {
                    char *p_card_name = NULL;
                    size_t card_name_len = 0;
                    if (bk_st.NextString(p_card_name, 
                               card_name_len, ALI_ATTR_SEPERATOR) 
                            && card_name_len > 0) {
                        delta_buffer_writer_->Write(p_card_name, card_name_len, false);
                    } else {
                        // otherwise write defalut value
                        delta_buffer_writer_->Write(emp_str, true);
                    }
                }

                // flush vertex and edge
                csv_post_writer_->FlushVertex(V_T_BANK_CARD,
                                     delta_buffer_writer_->GetBufferHead(),
                                     delta_buffer_writer_->GetBufferLength(),
                                     p_card,
                                     card_info_len,
                                     false,
                                     0,
                                     0);

                if (card_type == TRUST_CARD) {
                    csv_post_writer_->FlushEdge(E_T_TRUST,
                                       V_T_ACCOUNT,
                                       V_T_BANK_CARD,
                                       line_record_.p_uid,
                                       line_record_.uid_len,
                                       NULL,
                                       0,
                                       p_card,
                                       card_info_len,
                                       NULL,
                                       0,
                                       NULL,
                                       0,
                                       false);
                } else if (card_type == BIND_CARD) {
                    csv_post_writer_->FlushEdge(E_T_BIND,
                                       V_T_ACCOUNT,
                                       V_T_BANK_CARD,
                                       line_record_.p_uid,
                                       line_record_.uid_len,
                                       NULL,
                                       0,
                                       p_card,
                                       card_info_len,
                                       NULL,
                                       0,
                                       NULL,
                                       0,
                                       false);
                } else {
                    csv_post_writer_->FlushEdge(E_T_HIST_TRANS_CARD,
                                      V_T_ACCOUNT,
                                      V_T_BANK_CARD,
                                      line_record_.p_uid,
                                      line_record_.uid_len,
                                      NULL,
                                      0,
                                      p_card,
                                      card_info_len,
                                      NULL,
                                      0,
                                      NULL,
                                      0,
                                      false);
                }
            }
            return true;
        }

        // @func: writeMacVertexEdges()
        // @desc: writehe media verex and edges 
        // @para: p_media: point to medie buffer
        //        media_len: media buffer length
        //        meida_type: mac, ...etc
        // @retr: alwyas true ignore the dirty data
        bool writeMediaVertexEdges(char *p_media, 
                                   size_t media_len, 
                                   std::string media_type) {
            char *p_cont = NULL;
            size_t cont_len = 0;

            StrTokenizer st = StrTokenizer(p_media, media_len);
            while (st.NextString(p_cont, cont_len) && cont_len > 0) {
                if (!isValidStrValue(p_cont, cont_len)) {
                    continue;
                }

                delta_buffer_writer_->Reset();

                //prepare write the media
                uint32_t media_type_id = 
                        csv_post_writer_->enum_mapper_->encode(ENUM_MEDIA_TYPE, media_type);
                delta_buffer_writer_->Write(media_type_id, false);
                delta_buffer_writer_->Write(false, true);
                delta_buffer_writer_->Write(uint32_t(0), true);
                delta_buffer_writer_->Write(uint32_t(0), true);

                std::string media_pid = 
                        media_type + "#" + std::string(p_cont, cont_len);
                csv_post_writer_->FlushVertex(V_T_MEDIA,
                                     delta_buffer_writer_->GetBufferHead(),
                                     delta_buffer_writer_->GetBufferLength(),
                                     const_cast<char*>(media_pid.c_str()),
                                     media_pid.length(),
                                     false,
                                     0,
                                     0);

                csv_post_writer_->FlushEdge(E_T_TRUST_MEDIA,
                                   V_T_ACCOUNT,
                                   V_T_MEDIA,
                                   line_record_.p_uid,
                                   line_record_.uid_len,
                                   NULL,
                                   0,
                                   const_cast<char*>(media_pid.c_str()),
                                   media_pid.length(),
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

            if (!csv_post_reader_->NextString(line_record_.p_uid, \
                        line_record_.uid_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_ssn, \
                        line_record_.ssn_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(p_cont, cont_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            } else {
                std::string tmp(p_cont, cont_len);
                if (trim(tmp) == std::string("1")) {
                    line_record_.is_merchant = true;
                } else if (trim(tmp) == std::string("0")) {
                    line_record_.is_merchant = false;
                } else {
                    return false;
                }
            }

            if (!csv_post_reader_->NextString(line_record_.p_trust_province, \
                    line_record_.trust_province_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_bind_mobile, \
                    line_record_.bind_mobile_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_bind_card, \
                    line_record_.bind_card_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_trust_card, \
                    line_record_.trust_card_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_trust_mac, \
                    line_record_.trust_mac_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_trust_umid, \
                    line_record_.trust_umid_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_trust_imei, \
                    line_record_.trust_imei_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_trust_tid, \
                    line_record_.trust_tid_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_trust_ip3, \
                    line_record_.trust_ip3_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_trust_ua, \
                    line_record_.trust_ua_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_trust_lbs, \
                    line_record_.trust_lbs_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_trust_gmac, \
                    line_record_.trust_gmac_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_trust_vkeyid, \
                    line_record_.trust_vkeyid_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextString(line_record_.p_trans_card, \
                    line_record_.trans_card_len, ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextUnsignedLong(line_record_.income_1m_amt, \
                    ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            if (!csv_post_reader_->NextUnsignedLong(line_record_.income_1m_kt_amt, \
                    ALI_COLUMN_SEPERATOR)) {
                return false;
            }

            return true;
        }

    private:
        CSVPostReader *csv_post_reader_;
        CSVPostWriter *csv_post_writer_;
        gse2::GSE_DELTA_BUFFER *delta_buffer_writer_;

        RawAccountLineRecord line_record_;
    };
}

#endif

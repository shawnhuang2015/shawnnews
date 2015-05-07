
//ali_cm_loader_utility.hpp

#ifndef ALI_CM_LOADER_UTILITY_HPP
#define ALI_CM_LOADER_UTILITY_HPP

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

#include "ali_cm_graph_info.hpp"

namespace ALI_CM_GRAPH_LOADING {
    class StrTokenizer {

    public:
        StrTokenizer(char *p_str, size_t len) : lineend_(p_str+len), linereadposition_(p_str) {
            //GASSERT(NULL != p_str, "StrTokenizer pointer is null");
        }   

        bool NextString(char*& strptr, size_t& strlength,
                              char separator = ALI_FIELD_SEPERATOR) {
            if (linereadposition_ >= lineend_)
                return false;
            strptr = linereadposition_;
            while (*linereadposition_ != separator && linereadposition_ < lineend_) {
                linereadposition_++;
            }   
            strlength = linereadposition_ - strptr;

            linereadposition_++;  // jump over separator

            return true;
        }   

    private:
        char* lineend_;
        char* linereadposition_;
    };

    // trim from start
    inline static std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), 
                    std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
    }

    // trim from end
    inline static std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), 
                    std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
    }

    // trim from both ends
    inline static std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
    }

    // function for reading the binary pos for combing the acount vertex value
    inline static void getUintSetLen(uint8_t *ptr, size_t &len) {
        uint8_t *old_ptr = ptr;
        VertexLocalId_t size = gutil::CompactReader::readCompressed(ptr);
        for (VertexLocalId_t i = 0; i < size; ++i) {
            ptr += gutil::CompactReader::GetCompressedSize(ptr);  // get key size
        }
        len = ptr - old_ptr;
    }

    inline static void getUintLen(uint8_t *ptr, size_t &len) {
        uint8_t *old_ptr = ptr;
        ptr += gutil::CompactReader::GetCompressedSize(ptr);
        len = ptr - old_ptr;
    }

    inline static void getStringLen(uint8_t *ptr, size_t &len) {
        uint8_t *old_ptr = ptr;
        VertexLocalId_t strlen = gutil::CompactReader::readCompressed(ptr);
        len = ptr - old_ptr + strlen;
    }

    // get the event type id
    static int getEventVertexIdByDay(int day_no) {
        return V_T_EVENT0 + day_no;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // HELPER FUNCTION
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    static bool getEventDayNoByTime(uint64_t event_time, int &day_no) {
        // change for a simple implementation
        // input is for seconds
        const int seconds_per_day = 60*60*24;
        int total_days = event_time/seconds_per_day;
        day_no = total_days%MAX_ROUND_DAYS;

        return true;
        /*
        // preapre to get the day no
        int year = 0;
        int month = 0;
        int day = 0;
        getDateByMs(event_time, year, month, day);

        int day_diff = getDiffDays(BASE_YEAR, BASE_MONTH, BASE_DAY, year, month, day);
        if (day_diff < 0) {
            return false;
        }

        day_no = day_diff%MAX_ROUND_DAYS;
        return true;
        */
    }

    static bool isValidStrValue(const char *pValue, uint32_t len) {
        if (len < 0) {
            return false;
        }

        // check null case
        std::string data = std::string(pValue, len);
        std::transform(data.begin(), data.end(), data.begin(), ::tolower);
        if (data == "null") {
            return false;
        }

        // check all empty cases
        for (uint32_t i = 0; i < len; i++) {
            if (pValue[i] != ' ') {
                return true;
            }
        }

        return false;
    }

    static void getDateByMs(long ms, int &year, int &month, int &day) {
        time_t tt = time_t(ms/1000);

        tm utc_tm = *gmtime(&tt);
        year = utc_tm.tm_year + 1900;
        month = utc_tm.tm_mon + 1;
        day = utc_tm.tm_mday;
    }

    // Make a tm structure representing this date
    static std::tm make_tm(int year, int month, int day)
    {
        std::tm tm = {0};
        tm.tm_year = year - 1900; // years count from 1900
        tm.tm_mon = month - 1;    // months count from January=0
        tm.tm_mday = day;         // days count from 1
        return tm;
    }


    // check whether one input date is a valid one 
    static bool valid_date(int y, int m, int d) {
        //gregorian dates started in 1582
        if (! (1582<= y )  )//comment these 2 lines out if it bothers you
            return false;
        if (! (1<= m && m<=12) )
            return false;
        if (! (1<= d && d<=31) )
            return false;
        if ( (d==31) && (m==2 || m==4 || m==6 || m==9 || m==11) )
            return false;
        if ( (d==30) && (m==2) )
            return false;
        if ( (m==2) && (d==29) && (y%4!=0) )
            return false;
        if ( (m==2) && (d==29) && (y%400==0) )
            return true;
        if ( (m==2) && (d==29) && (y%100==0) )
            return false;
        if ( (m==2) && (d==29) && (y%4==0)  )
            return true;

        return true;
    }

    // get diff days between ay-am-ad -- by-bm-bd
    // return -1: error
    static int getDiffDays(int ay, int am, int ad, int by, int bm, int bd) {
        if (!valid_date(ay, am, ad) || !valid_date(by, bm, bd)) {
            return -1;
        }

        std::tm tm1 = make_tm(by,bm,bd);    // April 2nd, 2012
        std::tm tm2 = make_tm(ay,am,ad);    // February 2nd, 2003

        // Arithmetic time values.
        // On a posix system, these are seconds since 1970-01-01 00:00:00 UTC
        std::time_t time1 = std::mktime(&tm1);
        std::time_t time2 = std::mktime(&tm2);

        // Divide by the number of seconds in a day
        const int seconds_per_day = 60*60*24;
        std::time_t difference = (time1 - time2) / seconds_per_day;

        return int(difference);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    inline static void string_split(const std::string &s, std::vector<std::string> &elems, char delim = ALI_FIELD_SEPERATOR) {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
    }
}

#endif

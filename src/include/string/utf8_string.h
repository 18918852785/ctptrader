#pragma once

#include <string>
#include <vector>
#include <stdio.h>
#include "string_define.h"

#define string_format         cosmos::utf8_string::format
#define string_replace        cosmos::utf8_string::replace1
#define string_trim           cosmos::utf8_string::trim
#define string_split          cosmos::utf8_string::split
#define string_utf2gbk        cosmos::utf8_string::utf2gbk
#define string_gbk2utf        cosmos::utf8_string::gbk2utf8
#define string_to_ltc_seconds cosmos::utf8_string::str2ltc_seconds
#define string_to_date        cosmos::utf8_string::str2date
#define time_to_string        cosmos::utf8_string::time2str
#define string_to_int         cosmos::utf8_string::str2int
#define my_int_to_string      cosmos::utf8_string::int2str
#define my_int64_to_string    cosmos::utf8_string::int642str

struct _SYSTEMTIME;
namespace cosmos
{

	class utf8_string : public std::string
	{
	public:
		std::string utf2gbk() const;
		
		static std::string utf2gbk(const std::string& utf8);
		static std::string gbk2utf8(const std::string& gbk);
		static std::string int2str(int i);
		static std::string int642str(__int64 i);
		static int         str2int(const std::string& s);
		static std::string format(const char* fmt, ...);
		static std::string replace1(const std::string& src, const char* pold, const char* pnew);
		static std::string trim(const std::string& src);
		
		static std::vector< std::string > split( const std::string& s, const std::string& delim);
		static std::vector< std::string > split( const std::string& s, const std::string& delim, bool remove_empty);
		static std::vector< std::string > split( const std::string& s, const std::vector<std::string>& delims);
		static std::vector< std::string > split( const std::string& s, const std::vector<std::string>& delims, bool remove_empty);
		
		static size_t      find_first_of(const std::string& s, const std::vector<std::string>& delims, size_t last, size_t& delim_len);
		static __int64     str2ltc_seconds(const std::string& str_date,int addjust, int offset);
		static int         str2date       (const std::string& str_date);
		static bool        str_is_number  (const std::string& str_date);
		static __int64     GetLTCSecond(const int &td_date, const int &nSecondOfDay, int nTimeDiff = -8);
		static __int64     GetUTCMinute(const int &td_date);
		static std::string time2str(struct ::_SYSTEMTIME* t);
	};

}
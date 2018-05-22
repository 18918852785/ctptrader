#pragma once
#ifdef _WIN32
	#include <windows.h>
	#include <atltime.h>
#else//linux
	#include "string_conv.h"
#endif//linux

#include "utf8_string.h"
#include <vector>
#include <time.h>

#include <stdarg.h>  // For va_start, etc.
#include <algorithm>

namespace cosmos
{

	std::string utf8_string::utf2gbk() const
	{
		return utf2gbk(*this);
	}

	std::string utf8_string::utf2gbk( const std::string& utf8 )
	{
#ifdef _WIN32
		std::string ret;
		if( utf8.size() == 0 )
			return ret;
		auto p = &utf8[0];
		auto wchar_len = utf8.size() * 2;
		std::vector<wchar_t> ws(wchar_len);
		auto ws_len = ::MultiByteToWideChar(CP_UTF8, 0, p, (int)utf8.size(), &ws[0], (int)ws.size());
		if( ws_len <= 0 )
			return ret;

		std::vector<char> gbk(utf8.size()*2);
		auto gbk_len = WideCharToMultiByte(CP_ACP, 0, &ws[0], ws_len, &gbk[0], (int)gbk.size(), 0, 0 );
		if(gbk_len <= 0 )
			return ret;
		ret = &gbk[0];
		return ret;
#else
		char* tmp = 0;
		std::string ret;
		if (string_converter::utf8_to_gbk( utf8.c_str(), &tmp) > 0)
		{
			ret = tmp;
			string_converter::free_buf(tmp);
		}
		return ret;
#endif
	}

	std::string utf8_string::gbk2utf8( const std::string& gbk_in )
	{
#ifdef _WIN32
		std::string ret;
		if( gbk_in.size() == 0 )
			return ret;
		auto p = &gbk_in[0];
		auto wchar_len = gbk_in.size() * 3;
		std::vector<wchar_t> ws(wchar_len);
		auto ws_len = ::MultiByteToWideChar(CP_ACP, 0, p, (int)gbk_in.size(), &ws[0], (int)ws.size());
		if( ws_len <= 0 )
			return ret;

		std::vector<char> utf8_ret(gbk_in.size()*2);
		auto gbk_len = WideCharToMultiByte(CP_UTF8, 0, &ws[0], ws_len, &utf8_ret[0], (int)utf8_ret.size(), 0, 0 );
		if(gbk_len <= 0 )
			return ret;
		ret = &utf8_ret[0];
		return ret;
#else
		char* tmp = 0;
		std::string ret;
		if (string_converter::gbk_to_utf8(gbk_in.c_str(), &tmp) > 0)
		{
			ret = tmp;
			string_converter::free_buf(tmp);
		}
		return ret;
#endif
	}

	std::string utf8_string::int2str( int i )
	{
		char buf[128];
		_itoa(i, buf, 10);
		return buf;
	}
#ifdef _WIN32
#else
	int _vscprintf(const char *format,va_list argptr);
	int _vscwprintf(const wchar_t *format,va_list argptr);
	//These functions can, therefore, beemulated with vsprintf() and vswprintf() :
	int _vscprintf(const char *format, va_list argptr)
	{
		return(vsnprintf(0, 0, format, argptr));
	}
	int _vscwprintf(const wchar_t *format, va_list argptr)
	{
		return(vswprintf(0, 0, format, argptr));
	}
#endif

	std::string utf8_string::format( const char* fmt, ... )
	{
		std::string str;
		va_list ap;
		va_start(ap, fmt);
		auto len = _vscprintf( fmt, ap ) // _vscprintf doesn't count
			+ 1; // terminating '\0'
		str.resize(len, 0);
		int n = vsnprintf((char *)str.data(), len, fmt, ap);
		va_end(ap);
		//删除字符串后面的0
		if( len > 1 )
			str.resize(len-1);
		return str;
	}

	int utf8_string::str2int( const std::string& s )
	{
		return atoi( s.c_str() );
	}

	std::string utf8_string::replace1( const std::string& source, const char* pold, const char* pnew )
	{
		std::string str(source);
		std::string old_value(pold), new_value(pnew);
		{   
			while(true)   {   
				std::string::size_type   pos(0);   
				if(   (pos=str.find(old_value))!=std::string::npos   )   
				{
					str.replace( pos,old_value.length(), new_value);   
				}
				else   break;   
			}   
			return   str;   
		}   
	}

	std::string utf8_string::trim( const std::string& src )
	{
		std::string s(src);
			if (s.empty())   
			{  
				return s;  
			}  

			s.erase(0,s.find_first_not_of(" "));  
			s.erase(s.find_last_not_of(" ") + 1);  
			return s;  
	}

	std::vector< std::string > utf8_string::split( const std::string& s, const std::string& delim )
	{
		std::vector<std::string> delims;
		delims.push_back(delim);
		return split(s,delims,false);
	}


	std::vector< std::string > utf8_string::split( const std::string& s, const std::string&  delim, bool remove_empty)
	{
		std::vector<std::string> delims;
		delims.push_back(delim);
		return split(s,delims,remove_empty);
	}
	std::vector< std::string > utf8_string::split( const std::string& s, const std::vector<std::string>& delims )
	{
		return split(s,delims,false);
	}

	std::vector< std::string > utf8_string::split( const std::string& s, const std::vector<std::string>& delims, bool remote_empty )
	{
		std::vector< std::string > ret;
		size_t last = 0;  
		size_t delim_len = 0;
		size_t index= find_first_of(s,delims,last, delim_len);
		while (index!=std::string::npos)  
		{  
			ret.push_back(s.substr(last,index-last));  
			last=index + delim_len;
			index = find_first_of(s, delims, last, delim_len);
		}  
		//if (index-last>0)  
		{  
			ret.push_back(s.substr(last));
		}  
		if( ! remote_empty )
			return ret;
		///
		//删除空元素
		std::vector<std::string> fields2;
		std::for_each( ret.begin(), ret.end(), [&fields2](const std::string& s)
		{
			auto s1 = string_trim( s );
			if( s1.size() == 0 ) return;
			fields2.push_back(s1);
		});
		return fields2;
	}


	__int64 utf8_string::str2ltc_seconds( const std::string& str_date ,int addjust, int offset)
	{
		//2016-10-19 10:15:22
		//2016-10-19T10:15:22Z
		SYSTEMTIME st={0};
		/*
	
		*/
		{
			std::vector<std::string> delims;
			delims.push_back("-");
			delims.push_back("T");
			delims.push_back(" ");
			delims.push_back(":");
			delims.push_back("Z");
			delims.push_back("z");
			auto fields = split(str_date, delims);
			if( fields.size() < 2 )
			{
				return 0;
			}


			st.wYear = str2int(fields[0]);
			st.wMonth = str2int(fields[1]);

			//////////////////////////////////////////////////////////////////////////
			//检查是否有效
			if( st.wYear < 1900 || st.wMonth < 1 || st.wMonth > 12)
				return 0;

			if( fields.size() >= 3)
				st.wDay = str2int(fields[2]);
			else
				st.wDay = 1;
			if( fields.size() >= 6 )
			{
				st.wHour = str2int(fields[3]);
				st.wMinute = str2int(fields[4]);
				st.wSecond = str2int(fields[5]);
			}
#ifdef _WIN32
			//标准时间
			CTime t(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
			CTimeSpan span8(0, addjust, 0, 0 );
			//北京时间
			t += span8;

			st.wYear = t.GetYear();
			st.wMonth = t.GetMonth();
			st.wDay = t.GetDay();
			st.wHour = t.GetHour();
			st.wMinute = t.GetMinute();
			st.wSecond = t.GetSecond();
#else

#endif
		}

		__int64 td_date = 1ll * st.wYear *10000 +
			          1ll * st.wMonth *100+
					  1ll * st.wDay;
		__int64 nSecondOfDay = 1ll * st.wHour*3600+
						   1ll * st.wMinute *60+
						   1ll * st.wSecond;

		return GetLTCSecond( (int)td_date, (int)nSecondOfDay, offset);
	}

	int utf8_string::str2date( const std::string& str_date )
	{
		//////////////////////////////////////////////////////////////////////////
		//6位或8位数字: 201510
		if( str_is_number(str_date ))
		{
			auto ret = string_to_int(str_date);
			if( ret <= 0 )
				return 0;//failed
			if( str_date.size() == 4 )
			{
				ret = ret * 10000 + 101;
				return ret;
			}
			if( str_date.size() == 6 )
			{
				ret = ret * 100 + 1;
				return ret;
			}
			if( str_date.size() == 8 )
				return ret;
			return 0;//failed
		}

		//2016-10-19 10:15:22
		//2016-10-19T10:15:22Z
		SYSTEMTIME st={0};
		/*
	
		*/
		{
			std::vector<std::string> delims;
			delims.push_back("-");
			delims.push_back("T");
			delims.push_back(" ");
			delims.push_back(":");
			delims.push_back("Z");
			delims.push_back("z");
			auto fields = split(str_date, delims);
			if( fields.size() < 2 )
			{
				return 0;
			}


			st.wYear  = str2int(fields[0]);
			st.wMonth = str2int(fields[1]);

			//////////////////////////////////////////////////////////////////////////
			//检查是否有效
			if( st.wYear < 1900 || st.wMonth < 1 || st.wMonth > 12)
				return 0;

			if( fields.size() >= 3)
				st.wDay = str2int(fields[2]);
			else
				st.wDay = 1;
			if( fields.size() >= 6 )
			{
				//st.wHour = str2int(fields[3]);
				//st.wMinute = str2int(fields[4]);
				//st.wSecond = str2int(fields[5]);
			}
			return st.wYear*10000 + st.wMonth *100 + st.wDay;
		}

	}

	__int64 utf8_string::GetUTCMinute(const int &td_date)
	{
		if(!td_date)
			return 0;
		struct tm btime;
		memset(&btime, 0, sizeof(struct tm));
		btime.tm_year = td_date/10000 -1900;
		btime.tm_mon =  td_date/100%100 -1;
		btime.tm_mday = td_date%100;
#ifdef _WIN32
		time_t bsec = _mkgmtime(&btime);
#else
		auto bsec = timegm(&btime);
#endif
		return bsec / 60;
	}

	__int64 utf8_string::GetLTCSecond(const int &td_date, const int &nSecondOfDay, int nTimeDiff)
	{
		__int64 ret = GetUTCMinute(td_date) * 60 + nSecondOfDay;
		ret = ret * 1000;
		ret &= 0x00ffffffffffffff;
		ret |= (__int64(nTimeDiff * 4) << 56);
		return ret;
	}

	size_t utf8_string::find_first_of( const std::string& s, const std::vector<std::string>& delims, size_t last, size_t& delim_len )
	{
		delim_len = 0;
		//找到最小的一个index，返回
		__int64 pos = -1;
		for(size_t i=0;i<delims.size();i++)
		{
			auto ret = s.find( delims[i], last);
			if( ret == -1 )
				continue;
			if (ret < (size_t)pos)
			{
				pos = (int)ret;
				delim_len = delims[i].size();
				//break;
			}
		}
		return (size_t)pos;
	}

	std::string utf8_string::int642str( __int64 i )
	{
		char buf[128]={0};
		_i64toa(i, buf, 10);
		return buf;
	}

	std::string utf8_string::time2str( struct ::_SYSTEMTIME* t )
	{
		return string_format("%04d-%02d-%02d %02d:%02d:%02d.%03d",
			t->wYear,
			t->wMonth,
			t->wDay,
			t->wHour,
			t->wMinute,
			t->wSecond,
			t->wMilliseconds);
	}

	bool utf8_string::str_is_number( const std::string& str_date )
	{

		if( str_date.size() == 0 )
			return false;
		for(size_t i=0;i<str_date.size();i++)
		{
			if( ! isdigit(str_date[i]) )
				return false;
		}
		return true;
	}

}
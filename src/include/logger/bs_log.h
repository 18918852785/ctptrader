#pragma once

#include <string>

#include "bs_log_file.h"
#include "../string/utf8_string.h"

#define LOG_INFO(fmt, ...)  do{if(cosmos::LOG_INFO_ENABLE()) \
	cosmos::bs_func_log::log_info(__FILE__, __FUNCTION__, __LINE__, \
	cosmos::utf8_string::format(fmt, ##__VA_ARGS__) );\
	}while(0)

#define LOG_ERROR(fmt, ...) do{if(cosmos::LOG_ERROR_ENABLE())\
	cosmos::bs_func_log::log_error(__FILE__, __FUNCTION__, __LINE__, \
	cosmos::utf8_string::format(fmt, ##__VA_ARGS__) );\
	}while(0)

#define LOG_DEBUG(fmt, ...) do{if(cosmos::LOG_DEBUG_ENABLE())\
	cosmos::bs_func_log::log_debug(__FILE__, __FUNCTION__, __LINE__, \
	cosmos::utf8_string::format(fmt, ##__VA_ARGS__) );\
	}while(0)

#define LOG_FUNCTION() cosmos::bs_func_log logger(__FILE__, __FUNCTION__, __LINE__ )

inline std::string log_format_file_line( const std::string& f, int l)
{
	return string_format("%60s:%4d", f.c_str(), l);
}

inline std::string log_format_function(const std::string&  f)
{
	return string_format("%60s", f.c_str());
}


namespace cosmos{

	bool LOG_INFO_ENABLE();
	bool LOG_ERROR_ENABLE();
	bool LOG_DEBUG_ENABLE();


	class bs_func_log
	{
	public:
		static void*                     m_spMgr;
		static bs_log_file::writter*   m_log_file_writter_debug;
		static bs_log_file::writter*   m_log_file_writter_info;
		static bs_log_file::writter*   m_log_file_writter_error;
		static news_log_file_ptr         m_log_file;

		std::string m_file, m_func;
		int m_line;
		bs_func_log(const char* file, const char* func, int line)
		{
			if( ! LOG_DEBUG_ENABLE() )
				return;
			m_line = line;
			m_file = log_format_file_line(file, m_line);

			m_func = log_format_function(func);
			char msg[2048] = {0};
			_snprintf_s(msg, sizeof(msg)-1, "%s: enter", m_func.c_str());

			output(m_spMgr, m_file.c_str(), msg, 3);
		}
		~bs_func_log()
		{
			if( ! LOG_DEBUG_ENABLE() )
				return;

			char msg[2048] = {0};
			_snprintf_s(msg, sizeof(msg)-1, "%s: leave", m_func.c_str());
			output(m_spMgr, m_file.c_str(), msg, 3);
		}

		static int  init_log();
		static void log_info (const char* file, const char* func, int line, const std::string& msg);
		static void log_error(const char* file, const char* func, int line, const std::string& msg);
		static void log_debug(const char* file, const char* func, int line, const std::string& msg);
		static void output(void* spMgr, const char* file, const char* msg, int InfoType);
	};


}


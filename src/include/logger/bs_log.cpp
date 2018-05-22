#include "bs_log.h"
#ifdef BSNEWS_X64
#include <IDataCtx.h>
#else
#endif

#ifdef _WIN32
#include <windows.h>
#endif


#include "print.h"
#include "../string/utf8_string.h"
#include "../config/bs_config.h"


namespace cosmos {
	void*                        bs_func_log::m_spMgr = 0;
	news_log_file_ptr            bs_func_log::m_log_file;
	bs_log_file::writter*      bs_func_log::m_log_file_writter_error = 0;
	bs_log_file::writter*      bs_func_log::m_log_file_writter_debug = 0;
	bs_log_file::writter*      bs_func_log::m_log_file_writter_info = 0;
	int                        bsquote_showinfo_enabled = 0;

	static bs_config::log_config      ods_config;
	static bool ods_config_inited = false;

	extern bs_config::log_config      console_config;
	extern bool console_config_inited;

	bool LOG_INFO_ENABLE()
	{
		if (bs_func_log::m_log_file_writter_info)
			return bs_func_log::m_log_file_writter_info->m_log_info_enabled;
		return false;
	}

	bool LOG_DEBUG_ENABLE()
	{
		if (bs_func_log::m_log_file_writter_debug)
			return bs_func_log::m_log_file_writter_debug->m_log_debug_enabled;
		return false;
	}

	bool LOG_ERROR_ENABLE()
	{
		if (bs_func_log::m_log_file_writter_error)
			return bs_func_log::m_log_file_writter_error->m_log_error_enabled;
		return false;
	}

	void bs_func_log::output(void* spMgr, const char* file_lineno, const char* msg1, int log_type)
	{
		std::string str(msg1);
		//替换掉 %
		str = string_replace(str, "%", "_");		//////////////////////////////////////////////////////////////////////////
		//prepare log file
		{
			std::string log_msg = file_lineno;
			log_msg += " ";
			log_msg += str;

#ifdef BSNEWS_X64
			if (log_type == InfoType::Debug)
			{
				m_log_file_writter_debug->log_debug(log_msg.c_str());
			}
			if (log_type == InfoType::Info)
			{
				m_log_file_writter_debug->log_info(log_msg.c_str());
				m_log_file_writter_info->log_info(log_msg.c_str());
			}
			if (log_type == InfoType::Error)
			{
				m_log_file_writter_debug->log_error(log_msg.c_str());
				m_log_file_writter_info->log_error(log_msg.c_str());
				m_log_file_writter_error->log_error(log_msg.c_str());
			}
#endif
		}
		//////////////////////////////////////////////////////////////////////////
#ifdef BSNEWS_X64
		if (bsquote_showinfo_enabled && spMgr)
		{
			std::string log_msg(str);
			log_msg += ",";
			log_msg += file_lineno;
			//virtual void DBLogF(InfoType level, int msgCode, bool utf_8_convert, const char *fmt, ...) = 0;
			((IDataCtx*)spMgr)->DBLog((InfoType)log_type,
				0,
				true, log_msg.c_str());
		}
#endif
		str += "\n";

		//////////////////////////////////////////////////////////////////////////
		//配置信息
#ifdef BSNEWS_X64
		if (!console_config_inited)
		{
			console_config_inited = true;
			console_config = bs_config::get()->m_log_config["console"];
		}
#endif

		if (!ods_config_inited)
		{
			ods_config_inited = true;
			ods_config = bs_config::get()->m_log_config["ods"];

			bsquote_showinfo_enabled = bs_config::get()->m_bsquote_showinfo_enabled;
		}

		//////////////////////////////////////////////////////////////////////////
		//console
#ifdef BSNEWS_X64
		my_printf_set_color_black();
		if (log_type == InfoType::Debug && console_config.m_debug_enabled)
			my_printf(str.c_str());
		else if (log_type == InfoType::Info && console_config.m_info_enabled)
			my_printf(str.c_str());
		else if (log_type == InfoType::Error && console_config.m_error_enabled)
		{
			my_printf_set_color_red();
			my_printf(str.c_str());
		}
		my_printf_set_color_black();
#endif		
		//////////////////////////////////////////////////////////////////////////
		//windows
#ifdef BSNEWS_X64
		if (log_type == InfoType::Debug && ods_config.m_debug_enabled)
			OutputDebugStringA(str.c_str());
		else if (log_type == InfoType::Info && ods_config.m_info_enabled)
			OutputDebugStringA(str.c_str());
		else if (log_type == InfoType::Error && ods_config.m_error_enabled)
			OutputDebugStringA(str.c_str());
#endif
	}

#define FORMAT_FILE_MESSAGE() \
	auto s_file = log_format_file_line(file, line);\
	auto s_msg = log_format_function(func) + ": " + msg;

	void bs_func_log::log_info(const char* file, const char* func, int line, const std::string& msg)
	{
		if (!LOG_INFO_ENABLE())
			return;
		FORMAT_FILE_MESSAGE();
		output(bs_func_log::m_spMgr, s_file.c_str(), s_msg.c_str(), 0/*Info*/);
	}

	void bs_func_log::log_error(const char* file, const char* func, int line, const std::string& msg)
	{
		if (!LOG_ERROR_ENABLE())
			return;
		FORMAT_FILE_MESSAGE();
		output(bs_func_log::m_spMgr, s_file.c_str(), s_msg.c_str(), 2/*Error*/);
	}

	void bs_func_log::log_debug(const char* file, const char* func, int line, const std::string& msg)
	{
		if (!LOG_DEBUG_ENABLE())
			return;
		FORMAT_FILE_MESSAGE();
		output(bs_func_log::m_spMgr, s_file.c_str(), s_msg.c_str(), 3/*Debug*/);
	}

	int bs_func_log::init_log()
	{
		if (!m_log_file)
			m_log_file.reset(new bs_log_file());
		if (!m_log_file_writter_info)
			m_log_file_writter_info = m_log_file->get(LOG_NAME_IFNO);
		if (!m_log_file_writter_debug)
			m_log_file_writter_debug = m_log_file->get(LOG_NAME_DEBUG);
		if (!m_log_file_writter_error)
			m_log_file_writter_error = m_log_file->get(LOG_NAME_ERROR);
		return 0;
	}

}


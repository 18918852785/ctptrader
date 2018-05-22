#include "bs_log_file.h"
#include "../string/utf8_string.h"
//#include "expo_proxy.h"
#include "../config/bs_config.h"
#include <assert.h>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#include <shlwapi.h>

namespace cosmos{
	volatile long  bs_log_file::m_counter = 0;

	void bs_log_file::write_field( const std::string& table_name, 
		const std::string& primary_key, 
		const std::string& field_name, 
		const std::string& field_value )
	{
		auto w = get(table_name);
		auto msg = string_format("%s=%s,",
			//primary_key.c_str(),
			field_name.c_str(),
			field_value.c_str()
			);
		w->write(msg.c_str(), (int) msg.size(), false);
		w->write_flush();
	}

	void bs_log_file::write_line_begin( const std::string& table_name, const std::string& primary_key,
		const std::string& message)
	{
		auto w = get(table_name);
		auto msg = primary_key+","+message+",";
		w->write( msg.c_str(), (int)msg.size(), true);
		//w->write_flush();
	}

	void bs_log_file::write_line_end( const std::string& table_name, const std::string& primary_key,
		const std::string& message)
	{
		auto w = get(table_name);
		auto msg = primary_key+","+message +"\n";
		w->write( msg.c_str(), (int)msg.size(), false);
		w->write_flush();
	}

	void bs_log_file::write_field( const std::string& table_name, const std::string& primary_key_name, const std::string& primary_key_name_value )
	{
		auto w = get(table_name);
		auto msg = primary_key_name +"=" + primary_key_name_value +",";
		w->write( msg.c_str(),(int) msg.size(), false);
	}

	std::string bs_log_file::writter::get_log_file_path( const std::string& table_name , std::string& log_name)
	{
		auto file_folder = bs_config::get_exe_folder();
		auto file_name = string_replace(table_name, "/", "_");
		log_name = file_name;

		file_name += "_" + get_log_date();

		auto path = file_folder + "log\\";
		CreateDirectoryA(path.c_str(), 0);
		path += file_name;
		path += LOG_FILE_EXT;

		return path;
	}


	bs_log_file::writter* bs_log_file::get( const std::string& table_name )
	{
		auto pos = m_writters.find( table_name );
		if( pos == m_writters.end() )
		{
			writter w;
			m_writters[table_name] = w;
			pos = m_writters.find( table_name );
			//
			pos->second.init(table_name);
		}
		return &pos->second;
	}

	void bs_log_file::dump( const char* log_name, const void* pbuf, size_t buf_len )
	{
		unsigned char* buf = (unsigned char*)pbuf;

		auto logger = get(log_name);
		
		std::ostringstream ss;
		std::ostringstream ss2;
		ss  << "len: " << buf_len << ": hex:";
		ss2 << "len: " << buf_len << ": hex:";
		for(size_t i=0; i<buf_len;i++)
		{
			char hex[128] = {0};
			sprintf_s(hex, sizeof(hex)-1, "%02x", buf[i]);
			ss << hex;
			if(buf[i])
				ss2 << buf[i];
		}
		ss  << "\nend\n";
		ss2 << "\nend\n";
		auto str  = ss.str();
		auto str2 = ss2.str();
		logger->write( str.c_str(),  (int)str.size(), true);
		logger->write( str2.c_str(), (int)str2.size(), false);
		logger->write_flush();
	}



	int bs_log_file::writter::init( const std::string& table_name )
	{
		if( m_file != 0 )
			return 0;

		auto path = get_log_file_path(table_name, m_name);

		auto lc = bs_config::get()->m_log_config[m_name];
		m_log_info_enabled = lc.m_info_enabled;
		m_log_error_enabled = lc.m_error_enabled;
		m_log_debug_enabled = lc.m_debug_enabled;

		//确保log目录存在
		{
			char folder[MAX_PATH];
			strcpy(folder, path.c_str());
			::PathRemoveFileSpecA(folder);
			CreateDirectoryA(folder, 0);
		}
		m_file_path = path;
		m_file = _fsopen( path.c_str(), "w+t", _SH_DENYWR);
		assert(m_file);
		return 0;
	}

	int bs_log_file::writter::write( const char* msg, int len ,bool time_header)
	{
		if( ! m_file )
			return -1;
		if(time_header)
		{
			auto header = get_log_header();
			fwrite( header.c_str(), header.size(), 1, m_file);
		}
		fwrite(msg, len, 1 , m_file );
		return 0;
	}

	std::string bs_log_file::writter::get_log_date()
	{
		SYSTEMTIME st = {0};
		GetLocalTime(&st);
		//Wind.Expo4_20170425_010320_000.log
		auto s_time = string_format("%04d%02d%02d_%06d",
			st.wYear, st.wMonth, st.wDay,
			GetCurrentProcessId());

		return s_time;
	}

	__int64 news_log_file_get_log_count()
	{
		::InterlockedIncrement(&bs_log_file::m_counter);
		return bs_log_file::m_counter;
	}
	std::string bs_log_file::writter::get_log_header()
	{

		SYSTEMTIME st = {0};
		GetLocalTime(&st);
		auto tid = GetCurrentThreadId();
		auto s_time = string_format("%08lld %02d-%02d %02d:%02d:%02d.%03d %05d,",
			news_log_file_get_log_count(),
			st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
			tid
			);

		return s_time;
	}

	void bs_log_file::writter::log_info( const char* msg )
	{
		if( ! m_log_info_enabled )
			return;
		auto s = string_format(" INFO %s\n", msg);
		write(s.c_str(), (int)s.size(), true);
		write_flush();
	}

	void bs_log_file::writter::log_error( const char* msg )
	{
		if( ! m_log_error_enabled )
			return;
		auto s = string_format(" ERROR %s\n", msg);
		write(s.c_str(), (int)s.size(), true);
		write_flush();
	}

	void bs_log_file::writter::log_debug( const char* msg)
	{
		if( !m_log_debug_enabled)
			return;
		auto s = string_format(" DEBUG %s\n", msg);
		write(s.c_str(), (int)s.size(), true);
		write_flush();
	}

	int bs_log_file::writter::write_flush()
	{
		if( ! m_file )
			return -1;
		fflush(m_file);
		return 0;
	}

}

#endif//win32

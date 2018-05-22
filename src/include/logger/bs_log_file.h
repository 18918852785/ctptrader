#pragma once

#include <stdio.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

#define LOG_NEWS_BROADCAST_NAME "bsnews_broadcast"
#define LOG_NAME_SYNC           "bsnews_sync"
#define LOG_NAME_ASYNC          "bsnews_async"

#define LOG_NAME_DEBUG          "bsnews_debug"
#define LOG_NAME_ERROR          "bsnews_error"
#define LOG_NAME_IFNO          "bsnews_info"

#define LOG_FILE_EXT ".log"//wind expo6 自动删除日志文件,所以修改日志文件扩展名

namespace cosmos{

	class bs_log_file
	{
	public:
		class writter
		{
		public:
			writter():m_file(0)
				,m_log_info_enabled(true)
				,m_log_error_enabled(true)
				,m_log_debug_enabled(false)
			{

			}
			~writter()
			{
				if(m_file)
					fclose(m_file);
				m_file = 0;
			}
			int init(const std::string& table_name);

			//////////////////////////////////////////////////////////////////////////
			//记录日志
			void log_info(const char*msg);
			void log_error(const char*msg);
			void log_debug(const char*msg);

			//////////////////////////////////////////////////////////////////////////
			int write(const char* msg, int len,bool time_header);
			int write_flush();
			
			//////////////////////////////////////////////////////////////////////////
			static std::string get_log_file_path(const std::string& table_name, std::string& log_name);
			static std::string get_log_header();
			static std::string get_log_date();
			FILE*         m_file;
			std::string   m_file_path;
			std::string   m_name;
			bool          m_log_info_enabled, m_log_error_enabled, m_log_debug_enabled;
		};

		void write_line_begin(
			const std::string& table_name, 
			const std::string& primary_key,
			const std::string& msg);

		void write_line_end(
			const std::string& table_name, 
			const std::string& primary_key,
			const std::string& msg);

		writter* get(const std::string& log_name);
		
		std::unordered_map<std::string, writter> m_writters;

		//打印二进制
		void dump(const char* log_name, const void* buf, size_t buf_len);


		//////////////////////////////////////////////////////////////////////////
		//计数器: 全局唯一号码
		static volatile long  m_counter;
		void write_field(const std::string& table_name, 
			const std::string& primary_key, 
			const std::string& field_name,
			const std::string& field_value
			);

	protected:
		void write_field(const std::string& table_name, 
			const std::string& primary_key_name,
			const std::string& primary_key_name_value);

	};

	typedef std::shared_ptr<bs_log_file> news_log_file_ptr;
}


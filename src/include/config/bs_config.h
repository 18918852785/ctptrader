#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>
#include <functional>

#define DUMP_NEWS_BROADCAST     cosmos::bs_config::get()->m_dump_news_message_broadcast
#define DUMP_NEWS_MESSAGE_SYNC  cosmos::bs_config::get()->m_dump_news_message_sync
#define DUMP_NEWS_MESSAGE_ASYNC cosmos::bs_config::get()->m_dump_news_message_async

namespace cosmos{

	//存盘的配置
	class news_persist_config
	{
	public:
		news_persist_config();
		news_persist_config(const std::string& ini_path);
		virtual ~news_persist_config() {}

		virtual int         init();
		virtual std::string get_config(const std::string& name);
		virtual int         set_config(const std::string& name, const std::string& value);

		std::string get_config_reboot_time();
		virtual int         load();
		virtual int         save();

		int         get_windcode_index();
		void        set_windcode_index(int i);
	public:
		static void        set_next_reboot_time();
		static std::string calculate_next_reboot_time();

		std::string                         m_path;
		std::unordered_map<std::string,std::string>   m_configs;
	protected:
	};

class bs_config
{
public:
	bs_config()
		:m_dump_news_message_async(false)
		,m_dump_news_message_sync(false)
		,m_dump_news_message_broadcast(false)
		,m_finance_news_content_write(false)
		,m_finance_news_content_update_primary_id(false)
		,m_pagesize(150)
		,m_max_row_count(1000)
		,m_max_column_count(340)
		,m_windcode_max_count(30)
		,m_windcode_batch_size(100)
		,m_news_broadcast_list_count(100)
		,m_news_broadcast_enable(0)
		,m_news_broadcast_indicator_max_count(100)
		,m_news_broadcast_value_max_count(100)
		,m_news_getlist_appclass(0)
		,m_sky_sync_xml_log(0)
		,m_command_result_max_size(1* 1024 * 1024 )
		,m_sync_column_interval_seconds( 4* 3600)//4小时
		,m_sync_windcode_interval_seconds( 4 * 3600)//4小时
		,m_sync_windcode_little_interval_seconds( 5 )//5秒
		,m_ltc_time_addjust(0)
		,m_ltc_time_offset(8)
		,m_sky_timeout_seconds(5)
		,m_reboot_enable(0)
		,m_hub_enabled(0)
		,m_hub_send(0)
		,m_hub_recv(0)
		,m_get_column_list(1)
		,m_news_picture_download(0)
		,m_bsquote_showinfo_enabled(0)
	{
		init();
	}
	~bs_config();

public:
	class log_config
	{
	public:
		log_config():m_info_enabled(false), m_error_enabled(false), m_debug_enabled(false)
		{

		}
		std::string m_name;
		bool        m_info_enabled, m_error_enabled, m_debug_enabled;
	};
public:

	//记录news的消息: 广播消息
	int m_dump_news_message_broadcast;

	//记录news的消息: 服务器异步推送的消息
	int m_dump_news_message_async;

	//记录news的消息: 同步发送和接收到的消息
	int m_dump_news_message_sync;

	//是否在 finance_news内写入 content/id 
	int m_finance_news_content_write;

	//使用 update_primary_id 插入主键
	int m_finance_news_content_update_primary_id;

	int m_pagesize;
	int m_max_row_count;
	int m_max_column_count;
	int m_windcode_max_count;
	int m_windcode_batch_size;
	int m_news_broadcast_list_count;
	int m_news_broadcast_enable;//接收新闻推送
	int m_news_broadcast_indicator_max_count;
	int m_news_broadcast_value_max_count;
	std::vector<std::string> m_news_broadcast_media_types;
	int m_news_getlist_appclass;//1411
	int m_sky_sync_xml_log;
	int m_command_result_max_size;
	int m_sync_column_interval_seconds;
	int m_sync_windcode_interval_seconds;
	int m_sync_windcode_little_interval_seconds;
	int m_ltc_time_addjust;
	int m_ltc_time_offset;
	int m_sky_timeout_seconds;
	int m_reboot_enable;
	//////////////////////////////////////////////////////////////////////////
	//hub
	int         m_hub_enabled;
	int         m_hub_send;
	int         m_hub_recv;
	int         m_get_column_list;//从服务器同步栏目列表
	std::string m_hub_parameter;
	std::string m_hub_expo5_name;

	//////////////////////////////////////////////////////////////////////////
	//zmq
	std::string m_zmq_endpoint;
	//////////////////////////////////////////////////////////////////////////
	std::vector<std::string> m_content_enabled;
	//同步：执行顺序
	std::vector<std::string> m_sync_enabled;

	std::string  m_news_broadcast_input_folder;

	std::unordered_map<std::string, log_config> m_log_config;
	std::vector<std::string>                    m_column_enabled;
	std::string                                 m_custom_column_table;
	std::string                                 m_custom_columns;

	int                     m_news_picture_download;  //下载新闻图片
	std::string             m_news_picture_download_url;//url替换
	std::string             m_news_picture_downloader;//图片下载程序

	//////////////////////////////////////////////////////////////////////////
	//log
	int                     m_bsquote_showinfo_enabled;
	//////////////////////////////////////////////////////////////////////////
	static std::shared_ptr<bs_config> get();
	static void                       term();

	bool                              hub_expo5_enabled(){ return m_hub_expo5_name.size()>0;}
public:
	//存盘的配置
	news_persist_config m_persist_config;

public:
	//folder & file utils
	static std::string       get_exe_folder();
	static bool              file_exists(const std::string& path);
	static int               delete_file(const std::string& file_path);
	static int               read_file(const std::string& file_path,std::vector<char> &ret);
	static int               read_file_lines(const std::string& file_path, std::vector<std::string> &ret);
	static int               write_file(const std::string& file_path,const std::string& content);
	static int               write_file(const std::string& file_path, const void* pbuf, int buf_len);
	static int               get_files(const std::string& file_folder, const std::string& filter, std::vector<std::string>& files, bool recursive = false);
	static int               get_files(const std::string& file_folder, const std::string& filter, std::function<void(const std::string&)> on_file, bool recursive = false);
	static int               get_folders(const std::string& file_folder, const std::string& filter, std::vector<std::string>& folders,bool recursive=false);
	static int               read_file_line(FILE* f, std::string& line);
	static int               message_box( const std::string& text, const std::string& caption);
public:
	//url 
	static std::string       trim_url( const std::string& text );
protected:
	int init();

	static bs_config* m_config;

	static std::string get_config_string(void* pnode, const std::string& xml_path);
	static int         get_config_int(void* pnode, const std::string& xml_path, int def);
};

typedef std::shared_ptr<bs_config> config_ptr;
}

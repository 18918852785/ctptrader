#ifdef _WIN32

#include "bs_config.h"
#include "../logger/bs_log.h"

#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>

#ifndef GUIFRAME_EXPORTS

#include "../string/utf8_string.h"

#include "../xml/bs_MSXMLDOM.h"
#include <Shlwapi.h>
#else
#include <Shlwapi.h>
#endif//#ifndef GUIFRAME_EXPORTS

#include <atltime.h>

#define MAX_RETRY_COUNT 3
#define CONFIG_REBOOT_TIME "reboot-time"
#define CONFIG_WINDCODE_INDEX "windcode-index"

namespace cosmos
{
	bs_config* bs_config::m_config = 0;

	bs_config::~bs_config()
	{

	}

	int bs_config::init()
	{
		//////////////////////////////////////////////////////////////////////////
		//config path: bsnews.xml
		auto path = get_exe_folder();
		path += "etc\\";
		{
			std::vector<std::string> files;
			if( 0 != get_files(path, "bsnews*.xml", files) )
				return -1;
			if( files.size() == 0 )
				return -1;
			path = files[0];
		}

#ifndef GUIFRAME_EXPORTS
		std::shared_ptr<cosmos::MSXML_DOMDocument> doc(new cosmos::MSXML_DOMDocument());
		if( ! doc->load(path) )
		{
			//LOG_INFO(" load bsnews.xml error: %s", path.c_str() );
			assert(false);
			return -1;
		}

		//////////////////////////////////////////////////////////////////////////
#define GET_LOG_CONFIG_INT(aa)  aa = get_config_int( doc.get(), "/bsnews/config/log/" # aa, aa )
		GET_LOG_CONFIG_INT(m_dump_news_message_broadcast );
		GET_LOG_CONFIG_INT(m_dump_news_message_async );
		GET_LOG_CONFIG_INT(m_dump_news_message_sync );

#define GET_CONFIG_INT(aa)  aa = get_config_int( doc.get(), "/bsnews/config/" # aa, aa )
		GET_CONFIG_INT(m_finance_news_content_write );
		GET_CONFIG_INT(m_finance_news_content_update_primary_id );
		GET_CONFIG_INT(m_pagesize );
		GET_CONFIG_INT(m_max_row_count );
		GET_CONFIG_INT(m_max_column_count);
		GET_CONFIG_INT(m_windcode_max_count);
		GET_CONFIG_INT(m_windcode_batch_size);
		GET_CONFIG_INT(m_news_broadcast_list_count);
		GET_CONFIG_INT(m_news_broadcast_enable);
		GET_CONFIG_INT(m_news_getlist_appclass);
		GET_CONFIG_INT(m_sky_sync_xml_log);
		GET_CONFIG_INT(m_command_result_max_size);
		GET_CONFIG_INT(m_sync_column_interval_seconds);
		GET_CONFIG_INT(m_sync_windcode_interval_seconds);
		GET_CONFIG_INT(m_sync_windcode_little_interval_seconds);
		GET_CONFIG_INT(m_ltc_time_addjust);
		GET_CONFIG_INT(m_ltc_time_offset);
		GET_CONFIG_INT(m_sky_timeout_seconds);
		//////////////////////////////////////////////////////////////////////////
		//hub
		GET_CONFIG_INT(m_hub_enabled);
		GET_CONFIG_INT(m_hub_send);
		GET_CONFIG_INT(m_hub_recv);
		GET_CONFIG_INT(m_get_column_list);
		m_hub_parameter = get_config_string( doc.get(), "/bsnews/config/m_hub_parameter");
		m_hub_expo5_name= get_config_string( doc.get(), "/bsnews/config/m_hub_expo5_name");

		//////////////////////////////////////////////////////////////////////////
		//zmq
		m_zmq_endpoint = get_config_string(doc.get(), "/bsnews/config/m_zmq_endpoint");

		//////////////////////////////////////////////////////////////////////////
		auto s_value = get_config_string( doc.get(), "/bsnews/config/m_content_enabled");
		m_content_enabled = string_split(s_value, ",");
		
		s_value = get_config_string( doc.get(), "/bsnews/config/m_news_broadcast_media_types");
		if( s_value.size() > 0 )
		{
			m_news_broadcast_media_types = string_split(s_value, ",");
		}else
		{
			m_news_broadcast_media_types .push_back("01");//新闻
			m_news_broadcast_media_types .push_back("03");//公告
		}
		
		//////////////////////////////////////////////////////////////////////////
		//m_colum_enabled
		s_value = get_config_string( doc.get(), "/bsnews/config/m_column_enabled");
		m_column_enabled = string_split(s_value, ",");

		//////////////////////////////////////////////////////////////////////////
		m_custom_column_table = get_config_string( doc.get(), "/bsnews/config/m_custom_column_table");
		m_custom_columns      = get_config_string( doc.get(), "/bsnews/config/m_custom_columns");

		//////////////////////////////////////////////////////////////////////////
		s_value = get_config_string( doc.get(), "/bsnews/config/m_sync_enabled");
		m_sync_enabled = string_split(s_value, ",");
		m_news_broadcast_input_folder = get_config_string( doc.get(), "/bsnews/config/m_news_broadcast_input_folder");

		//////////////////////////////////////////////////////////////////////////
		//下载
		m_news_picture_download = get_config_int(doc.get(), "/bsnews/config/m_news_picture_download", m_news_picture_download);
		m_news_picture_download_url = get_config_string(doc.get(), "/bsnews/config/m_news_picture_download_url");
		m_news_picture_downloader = get_config_string(doc.get(), "/bsnews/config/m_news_picture_downloader");

		//////////////////////////////////////////////////////////////////////////
		//log
		m_bsquote_showinfo_enabled = get_config_int(doc.get(), "/bsnews/config/m_bsquote_showinfo_enabled", m_bsquote_showinfo_enabled);
		//////////////////////////////////////////////////////////////////////////
		//log config
		{
			auto logs = doc->getNode("/bsnews/config/log");
			auto child = logs->getFirstChildNode();
			while( child.get() )
			{
				//////////////////////////////////////////////////////////////////////////
				//log
				auto log_name = child->getName();
				log_config lc;
				{
					lc.m_name = log_name;

					auto child2 = child->getFirstChildNode();
					while(child2.get())
					{
						auto name2 = child2->getName();
						auto value2 = string_to_int( string_trim( child2->getText() ));
						if( name2 == "info" )
							lc.m_info_enabled = value2 ? true: false;
						if( name2 == "error" )
							lc.m_error_enabled =value2? true: false;
						if( name2 == "debug")
							lc.m_debug_enabled = value2? true: false;
						//////////////////////////////////////////////////////////////////////////
						auto next2 = child2->getNextSiblingNode();
						child2 = next2;
					}
				}
				//保存起来
				m_log_config[log_name] = lc;
				//////////////////////////////////////////////////////////////////////////
				//next child
				auto next = child->getNextSiblingNode();
				child = next;
			}
		}

		m_persist_config.init();
#endif//#ifndef GUIFRAME_EXPORTS

		
		return 0;
	}

	std::string bs_config::get_exe_folder()
	{
		char szTemp[1024];
		if(GetModuleFileNameA(NULL, szTemp, sizeof(szTemp)) != 0)
		{
			char *pszPos = strrchr(szTemp, '\\');
			if(pszPos != NULL)
			{
				*(pszPos + 1) = 0;
			}
		}
		return szTemp;
	}

	std::string bs_config::get_config_string(void* pdoc, const std::string& xml_path)
	{
		std::string ret;

#ifndef GUIFRAME_EXPORTS
		cosmos::MSXML_DOMDocument* doc = (cosmos::MSXML_DOMDocument*)pdoc;
		auto result = doc->getNode( xml_path );
		if( ! result.get() )
			return ret;
		ret = result->getText();
		ret = string_trim(ret);
#endif//#ifndef GUIFRAME_EXPORTS
		return ret;

	}


	int bs_config::get_config_int( void* pnode, const std::string& xml_path , int default)
	{
		std::string ret = get_config_string(pnode, xml_path);
		if( ret.size() == 0 )
			return default;
#ifndef GUIFRAME_EXPORTS
		return cosmos::utf8_string::str2int(ret);
#else
		return 0;
#endif//#ifndef GUIFRAME_EXPORTS

	}

	std::shared_ptr<bs_config> bs_config::get()
	{
		std::shared_ptr<bs_config> ret;
		if( ! m_config )
		{
			m_config = new bs_config();
			m_config->init();
		}
		ret.reset( m_config, [](bs_config*){});
		return ret;
	}

	int bs_config::read_file( const std::string& file_path,std::vector<char> &ret)
	{
		int count = 0;
		do{

			count ++;
			if( count > MAX_RETRY_COUNT )
				return -2;
			FILE* f = 0;
			fopen_s( &f, file_path.c_str(), "rb");
			if( ! f )
			{
				Sleep(1*1000);
				continue;
			}
			
			fseek(f, 0, SEEK_END);
			int size = (int)ftell(f);
			fseek(f, 0, SEEK_SET);
			if( size > 0 )
			{
				ret.resize(size, 0 );
				fread(&ret[0], size, 1, f);
			}
			fclose(f);
			return 0;
		}while(count <= MAX_RETRY_COUNT );
		return -3;
	}

	int bs_config::write_file( const std::string& file_path,const std::string& content )
	{
		void* p = 0;
		if( content.size() > 0 )
			p = (void*)&content[0];
		return write_file(file_path, p, (int)content.size());
	}

	int bs_config::write_file( const std::string& file_path, const void* pbuf, int buf_len )
	{
		FILE*  f = 0;
		fopen_s( &f, file_path.c_str(), "w+b");
		if( ! f)
		{
			return -1;
		}
		if( buf_len > 0 )
		{
			fwrite( pbuf, buf_len, 1, f);
		}
		fclose(f);
		return 0;
	}

	int bs_config::get_files(const std::string& file_folder, const std::string& filter, std::vector<std::string>& files, bool recursive)
	{
		files.clear();
		auto act = [&files](const std::string& s)
		{
			files.push_back(s);
		};
		return get_files(file_folder, filter, act, recursive);
	}

	int bs_config::get_files( const std::string& file_folder, const std::string& filter, 
		std::function<void(const std::string&)> on_file,
		bool recursive)
	{
		WIN32_FIND_DATAA fd = {0};
		HANDLE hFind = 0;

		//////////////////////////////////////////////////////////////////////////
		//子目录
		std::vector<std::string> sub_folders;
		auto ret = get_folders(file_folder, "*.*", sub_folders);

		if( recursive )
		{
			std::for_each(sub_folders.begin(), sub_folders.end(), 
				[&on_file,&recursive,&filter,&file_folder](const std::string& sub_folder)
			{
				auto sub = file_folder;
				sub += "\\";
				sub += sub_folder;

				bs_config::get_files( sub, filter, on_file, recursive );
				//std::for_each( sub_files.begin(), sub_files.end(),[&files2,sub_folder](const std::string&f )
				//{
				//	files2.push_back( sub_folder + "\\" + f);
				//});
			});
		}

		//////////////////////////////////////////////////////////////////////////
		//get files
		auto folder = file_folder;
		if( folder[folder.size()-1] != '\\')
			folder += "\\";
		auto sub_folder = folder;
		folder += filter;
		hFind = FindFirstFileA( folder.c_str(), &fd);
		if (hFind == INVALID_HANDLE_VALUE) 
		{
#ifndef GUIFRAME_EXPORTS
			//LOG_DEBUG("FindFirstFile failed (%d)\n", GetLastError());
#endif
			return 0;// (files.size() > 0) ? 0 : -1;
		} 
		do
		{
			auto is_dir = fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
			if(is_dir)
			{
				//m_ss << string_format("%30s\t%10s\n", fd.cFileName,"<DIR>");
				sub_folders.push_back( fd.cFileName );
			}else
			{
				//m_ss << string_format("%30s\t%10lld\n", fd.cFileName,fd.nFileSizeLow);
				std::string s (sub_folder);
				s += fd.cFileName;
				
				//files.push_back( s );
				on_file(s);
			}
		}while(::FindNextFileA(hFind, &fd)); 
		FindClose(hFind);
		return 0;// (files.size() > 0) ? 0 : -1;
	}

	int bs_config::get_folders( const std::string& file_folder, const std::string& filter, std::vector<std::string>& files ,bool recursive)
	{
		WIN32_FIND_DATAA fd = {0};
		HANDLE hFind = 0;
		files.clear();

		auto folder = file_folder;
		if( folder[folder.size()-1] != '\\')
			folder += "\\";
		folder += filter;
		hFind = FindFirstFileA( folder.c_str(), &fd);
		if (hFind == INVALID_HANDLE_VALUE) 
		{
#ifndef GUIFRAME_EXPORTS
			//LOG_DEBUG("FindFirstFile failed (%d)\n", GetLastError());
#endif
			return -1;
		} 
		std::vector<std::string> sub_folders;
		do
		{
			auto is_dir = fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
			if( is_dir)
			{
				std::string s = fd.cFileName;
				if( s != "." && s != ".." )
					files.push_back( fd.cFileName );
			}
		}while(::FindNextFileA(hFind, &fd)); 
		FindClose(hFind);
		return 0;
	}

	bool bs_config::file_exists( const std::string& path )
	{
		return PathFileExistsA( path.c_str() ) ? true: false;
	}

	int bs_config::delete_file( const std::string& file_path )
	{
		return DeleteFileA(file_path.c_str()) ? 0 : -1;
	}

	std::string bs_config::trim_url( const std::string& text )
	{
		if( _strnicmp( text.c_str(), "http", 4) != 0 )
			return text;
#define URL_TRIM_LEN 10
		if( text.length() < URL_TRIM_LEN )
			return text;
		return text.substr(0, URL_TRIM_LEN);
	}

	int bs_config::read_file_line( FILE* f,std::string & line )
	{
		char buf[4096];
		if( ! fgets(buf, sizeof(buf), f) )
			return -1;
		//////////////////////////////////////////////////////////////////////////
		//删除后面的 '\n'
		std::string s_line = buf;
		if( s_line.size() > 2 )
		{
			if( s_line[s_line.size()-1] == '\n')
				s_line = s_line.substr(0, s_line.size()- 1 );
		}
		line = s_line;
		return 0;
	}

	int bs_config::message_box( const std::string& text, const std::string& caption )
	{
		return MessageBoxA(0, text.c_str(), caption.c_str(), MB_OK | MB_ICONEXCLAMATION);
	}

	int bs_config::read_file_lines( const std::string& file_path, std::vector<std::string> &ret )
	{
		FILE* f = 0;
		fopen_s( &f, file_path.c_str(), "rt");
		if( ! f )
			return -1;
		std::string line;
		while( read_file_line(f, line) == 0 )
		{
			ret.push_back(line);
		}
		fclose(f);
		return 0;
	}

	void bs_config::term()
	{
		delete m_config;
		m_config = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	// persist config
	news_persist_config::news_persist_config()
	{
		m_path = bs_config::get_exe_folder();
		m_path += "log\\bsnews.ini";
		init();
	}
	news_persist_config::news_persist_config(const std::string& ini_path)
	{
		m_path = ini_path;
		init();
	}
	int news_persist_config::init()
	{
		load();
		return 0;
	}

	std::string news_persist_config::get_config( const std::string& name )
	{
		return m_configs[name];
	}

	int news_persist_config::set_config( const std::string& name, const std::string& value )
	{
		m_configs[name] = value;
		return 0;
	}

	int news_persist_config::save()
	{
		FILE* f = 0;
		fopen_s( &f, m_path.c_str(), "wt");
		if( ! f)
			return -1;
		for(auto it = m_configs.begin(); it!=m_configs.end();it++)
		{
			if( it->second.length() == 0 )
				continue;
			auto line = string_format("%s=%s\n", it->first.c_str(), it->second.c_str());
			fputs( line.c_str(), f );
		}
		fclose(f);
		return 0;
	}

	int news_persist_config::load()
	{
		FILE* f = 0;
		fopen_s( &f, m_path.c_str(), "rt");
		if( ! f)
			return -1;
		std::string line;
		while( bs_config::read_file_line(f, line) == 0 )
		{
			auto fields = string_split( line, "=" );
			if( fields.size() < 2 )
				continue;
			m_configs[fields[0]]=fields[1];
		}
		fclose(f);
		return 0;
	}


	std::string news_persist_config::calculate_next_reboot_time()
	{
		SYSTEMTIME st;
		CTime t_now = CTime::GetCurrentTime();
		CTimeSpan one_day(1,0,0,0);
		t_now += one_day;
		ZeroMemory(&st, sizeof(st));
		st.wYear = t_now.GetYear();
		st.wMonth = t_now.GetMonth();
		st.wDay = t_now.GetDay();
		st.wHour = 19;//晚7点

		return time_to_string(&st);
	}

	void news_persist_config::set_next_reboot_time()
	{
		auto reboot_time = calculate_next_reboot_time();
		bs_config::get()->m_persist_config.set_config(CONFIG_REBOOT_TIME, reboot_time);
		bs_config::get()->m_persist_config.save();

		//LOG_INFO(" next reboot time was set to %s", reboot_time.c_str());
	}

	std::string news_persist_config::get_config_reboot_time()
	{
		return get_config(CONFIG_REBOOT_TIME);
	}

	int news_persist_config::get_windcode_index()
	{
		auto s = get_config(CONFIG_WINDCODE_INDEX);
		if( s.size() == 0 )
			return 0;
		return string_to_int(s);
	}

	void news_persist_config::set_windcode_index( int i )
	{
		set_config(CONFIG_WINDCODE_INDEX, my_int_to_string(i));
	}

}

#endif//_WIN32

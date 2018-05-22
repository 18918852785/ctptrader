#ifdef _WIN32
#include <windows.h>
#include <Shlwapi.h>
#endif
#include <memory>
#include <string>
#include <vector>
#include <functional>

#include "proc_monitor.h"
#include "../string/utf8_string.h"
#include "../string/string_define.h"
#include "../thread/ithread.h"
#include "../dll_loader/dll_factory.h"
#include "../signalrcpp/hub.h"

namespace signalr_cpp{
	static dll::factory_ptr hub_dll;
	static thread::ithread_ptr m_thread;

	proc_monitor::proc_monitor()
		:m_target_process_id(0)
		,m_client_process_handle(0)
	{
	}

	proc_monitor::~proc_monitor()
	{

	}
	void ods(const char*s)
	{
#ifdef _WIN32
		OutputDebugStringA(s);
#endif
	}

	int command_line_get_arg(const std::string& name, int def = 0)
	{
		auto args = string_split(api_GetCommandLineA(), string_format("/%s:", name.c_str()), true);
		if (args.size() < 2)
			return def;
		auto s_id = string_split(args[1], " ")[0];
		auto ret = string_to_int(s_id);
		if (ret<= 0)
		{
			return def;
		}
		return ret;
	}

	int proc_monitor::start()
	{

		//从命令行解析出 要监控的进程id: /client_processid:7878
		auto args = string_split(api_GetCommandLineA(), "/client_processid:", true);
		if (args.size() < 2)
		{
			ods("arg size <2 ");
			return -1;
		}
#ifdef _WIN32
		{
			auto s_id = string_split(args[1]," ")[0];
			m_target_process_id = string_to_int(s_id);
			if (m_target_process_id <= 0)
			{
				ods("not int");
				return -1;
			}
			auto dwAccess = PROCESS_ALL_ACCESS;
			m_client_process_handle = OpenProcess(dwAccess, FALSE, m_target_process_id);
			if (!m_client_process_handle)
			{
				ods("open process failed");
				return -2;
			}
			//
			if( ! m_thread )
			{
				static dll::dll_hub_factory g_hub;
				m_thread = g_hub.get()->create_thread_ptr();
			}
			m_thread->add_timer("monitor_client_close",
				1000,1000,
				[this]()
			{
				auto ret = WaitForSingleObject( m_client_process_handle, 500 );
				if( ret == WAIT_OBJECT_0 )
				{
					m_thread->delete_timer("monitor_client_close");

					//客户端已经关闭，关闭自己吧
					TerminateProcess( GetCurrentProcess(), -2);
					return;
				}
			}
			);
		}
#endif
		return 0;
	}

}
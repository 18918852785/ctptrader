#pragma once

//请包含:
//#include <windows.h>
//#include <Shlwapi.h>

#include <memory>
#include <string>

#include "../string/string_define.h"
#ifdef _WIN32
#else
#include <dlfcn.h>
#endif

//////////////////////////////////////////////////////////////////////////
//dll_loader
#define dll_loader_ptr std::shared_ptr<dll_loader>
typedef void*   (*FUNC_get_dll)();
typedef void (*FUNC_free_dll)(void*);

class dll_loader
{
public:
	dll_loader( const char* dll_name)
		: m_dll_name(dll_name)
		, m_h_dll(0)
		, m_func_get_dll(0)
		, m_func_free_dll(0)
	{
		load_dll();
	}

	int load_dll()
	{
		//显示ui
		char exepath[260];
		api_GetModuleFileNameA(api_GetModuleHandle(0), exepath, sizeof(exepath));
		api_PathRemoveFileSpecA(exepath);
		api_PathCombineA(exepath, exepath, m_dll_name.c_str());

		auto h_dll = api_LoadLibraryA( exepath );
		if( h_dll == 0 )
		{
			h_dll = api_LoadLibraryA( m_dll_name.c_str() );
		}
		if ( h_dll )
		{
			{
				FUNC_get_dll func_get_dll = nullptr;
				func_get_dll = (FUNC_get_dll)api_GetProcAddress( h_dll, "get_dll" );
				if ( func_get_dll )
				{
					m_func_get_dll = (void*)func_get_dll;
				}
				else
				{
#ifdef _WIN32
#else
					printf("get '%s.get_dll() failed:%s\n", m_dll_name.c_str(), dlerror());
#endif
				}
			}
			{
				FUNC_free_dll func_free_dll = nullptr;
				func_free_dll = (FUNC_free_dll)api_GetProcAddress( h_dll, "free_dll" );
				if ( func_free_dll )
				{
					m_func_free_dll = (void*)func_free_dll;
				}
			}
			if(m_func_get_dll)
				return 0;//ok
		}
		return -1;
	}

	void* call_get_dll()
	{
		if ( m_func_get_dll )
			return ((FUNC_get_dll) m_func_get_dll )();
		return 0;
	}

	template<typename T>
	std::shared_ptr<T> call_get_dll_ptr()
	{
		if(m_func_free_dll)
			return std::shared_ptr<T> ( (T*)call_get_dll(), 
			(FUNC_free_dll)m_func_free_dll);
		return std::shared_ptr<T> ( (T*)call_get_dll()); 
	}

	static dll_loader_ptr create( const char* dll_name )
	{
		dll_loader_ptr ret( new dll_loader(dll_name) );
		return ret;
	}

	std::string m_dll_name;
	std::string m_dll_fullpath;
	int         m_h_dll;
	void*       m_func_get_dll;
	void*       m_func_free_dll;
};

#ifdef _WIN32
#define DLL_NAME_INDICATOR_LOG "indicator-log.dll"
#define DLL_NAME_INDICATOR_CONFIG "indicator-config.dll"
#define DLL_NAME_INDICATOR_ACE "indicator-ace.dll"
#define DLL_NAME_INDICATOR_MESSAGE_BUS "indicator-message-bus.dll"
#define DLL_NAME_INDICATOR_STRING "indicator-string.dll"
#define DLL_NAME_INDICATOR_UI_CONTROL "indicator-ui-control.dll"
#define DLL_NAME_INDICATOR_ZMQ "indicator-zmq.dll"
#define DLL_NAME_INDICATOR_FUTURE "indicator-future.dll"
#define DLL_NAME_INDICATOR_MONITOR_CPP "indicator-monitor-cpp.dll"
#define DLL_NAME_SIGNALRCPP_HUB "signalrcpp_hub.dll"
#else

#define DLL_NAME_INDICATOR_LOG "indicator_log.dll"
#define DLL_NAME_INDICATOR_CONFIG "indicator_config.dll"
#define DLL_NAME_INDICATOR_ACE "indicator_ace.dll"
#define DLL_NAME_INDICATOR_MESSAGE_BUS "indicator_message_bus.dll"
#define DLL_NAME_INDICATOR_STRING "indicator_string.dll"
#define DLL_NAME_INDICATOR_UI_CONTROL "indicator_ui_control.dll"
#define DLL_NAME_INDICATOR_ZMQ "indicator_zmq.dll"
#define DLL_NAME_INDICATOR_FUTURE "indicator_future.dll"
#define DLL_NAME_INDICATOR_MONITOR_CPP "indicator_monitor_cpp.dll"
#define DLL_NAME_SIGNALRCPP_HUB "signalrcpp_hub.dll"

#endif

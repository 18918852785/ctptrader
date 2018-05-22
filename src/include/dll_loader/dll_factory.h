#pragma once

#if 1==0//包含windows.h会和 winsock.h 冲突
#include <windows.h>
#include <shlwapi.h>
#endif

#include "dll_loader.h"
#include "../thread/ithread.h"

//////////////////////////////////////////////////////////////////////////
//#include "../signalrcpp/hub.h"
namespace signalrcpp
{
	struct hub_factory;
	typedef std::shared_ptr<hub_factory> hub_factory_ptr;
}

//////////////////////////////////////////////////////////////////////////
#include <assert.h>

namespace dll{
	class factory
	{
	public:
		factory(const char* dll_name)
			:hub_factory(0)
		{
			if(dll_name)
			{
				name = dll_name;
			}
		}

		signalrcpp::hub_factory* get(){
			load_dll();
			return hub_factory;
		}

	protected:
		void load_dll()
		{
			if(hub_dll || hub_factory)
				return;
			//////////////////////////////////////////////////////////////////////////
			hub_dll = dll_loader::create( name.c_str() );
			assert(hub_dll);
			if(hub_dll)
			{
				hub_factory = (signalrcpp::hub_factory*) hub_dll->call_get_dll() ;
				assert(hub_factory);
			}
		}

		std::string name;
		dll_loader_ptr   hub_dll;
		signalrcpp::hub_factory*  hub_factory;
	};
	typedef std::shared_ptr<factory> factory_ptr;

	class dll_hub_factory: public factory
	{
	public:
		dll_hub_factory():factory(DLL_NAME_SIGNALRCPP_HUB)
		{
		}
	};
	typedef std::shared_ptr<dll_hub_factory> dll_hub_factory_ptr;

	//inline factory_ptr create_hub_dll(){
	//	return factory_ptr (new dll::dll_hub_factory());
	//}
}
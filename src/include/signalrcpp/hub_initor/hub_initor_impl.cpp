
#include "hub_initor_impl.h"

#ifdef _WIN32
//////////////////////////////////////////////////////////////////////////
#include <windows.h>
//////////////////////////////////////////////////////////////////////////
#endif


namespace signalrcpp_hub_initor{

	//////////////////////////////////////////////////////////////////////////
	//signalrcpp_init
	hub_initor_impl::hub_initor_impl( void )
	{
#ifdef _WIN32
		CoInitializeEx(0,2/*appartment*/);
#endif
	}

	hub_initor_impl::~hub_initor_impl( void )
	{
#ifdef _WIN32
		CoUninitialize();
#endif
	}

	void hub_initor_impl::init()
	{
#ifdef _WIN32
		CoInitializeEx(0,2/*appartment*/);
#endif
	}

}


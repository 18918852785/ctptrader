/*
封装 signalrcpp.exe com 对象
*/
#pragma once
#include <memory>

#include "../hub_initor.h"

namespace signalrcpp_hub_initor
{
	//////////////////////////////////////////////////////////////////////////
	class hub_initor_impl: public signalrcpp::hub_initor
	{
	public:
		hub_initor_impl(void);
		~hub_initor_impl(void);

		static void init();
	};
	
}

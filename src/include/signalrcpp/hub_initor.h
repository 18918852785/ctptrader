/*
封装 signalrcpp.exe com 对象
*/
#pragma once
#include <memory>

namespace signalrcpp
{
	//////////////////////////////////////////////////////////////////////////
	//signalrcpp_init：初始化 com/ace/socket 
	struct hub_initor
	{

	};
	typedef std::shared_ptr<hub_initor> hub_initor_ptr;
}

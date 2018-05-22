#pragma once
#include "hub_response.h"

namespace signalrcpp{
	struct hub_request{
		//发送的数据
		const char* app_class;
		const char* app_command;
		void*       data;
		int         data_len;
		int         from_thread;//客户端是否从同一个线程，发起hub调用

		//返回的数据
		hub_response_ptr    response;
	};
	typedef std::shared_ptr<hub_request> hub_request_ptr;

}
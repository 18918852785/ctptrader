#pragma once

#include "../../thread/thread_event.h"

//////////////////////////////////////////////////////////////////////////
namespace signalrcpp_hub_com{
	class hub_com_request_impl: public signalrcpp::hub_request
	{
	public:
		hub_com_request_impl()
		{
			app_class = 0;
			app_command = 0;
			data=0;
			data_len=0;
			from_thread=false;
			client_seq=0;
		}
		//////////////////////////////////////////////////////////////////////////
		//通知数据返回的信号量
		::thread::thread_event_ptr data_arrived_event;

		//////////////////////////////////////////////////////////////////////////
		//客户端维护的序号
		long long   client_seq;
	};

}




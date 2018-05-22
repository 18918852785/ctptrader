/*
封装 signalrcpp.exe com 对象
*/
#pragma once

#include "../hub.h"

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

#include "../../thread/thread_mutex.h"
#include "../hub_request.h"
#include "hub_ace_impl.h"

namespace signalrcpp_hub_ace_service
{
	class hub_ace_service: public signalrcpp_hub_ace::hub_ace_impl
	{
		void        init(signalrcpp::hub_args* pargs);
	public:
		hub_ace_service(signalrcpp::hub_args* pargs);
		
		long        broadcast(void* msg, int msg_len);
		long        post(signalrcpp::hub_request_ptr m);
		long        send(signalrcpp::hub_request_ptr m, unsigned long timeout_millseconds);
		long        set_callback(signalrcpp::hub_callback callback);

	protected:
		signalrcpp::hub_callback   m_app_callback;

		volatile long         m_post_seq;
		::thread::thread_mutex_ptr                       m_post_mutex;
		std::unordered_map<long long, signalrcpp::hub_request_ptr>   m_post_msgs;

		long send_message(const char* class_name, const char* object_name, const void* data, int data_len);
	};
}

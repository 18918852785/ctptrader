#pragma once

#include "../../../indicator-ace/header/indicator-ace.h"
#include "../hub.h"

namespace signalrcpp_hub_ace
{

	class hub_ace_impl: public signalrcpp::hub
	{
	protected:
		indicator::socket_init_ptr m_ace;
		messagebus::client::messagebus_client_thread_ptr m_client;
		std::string                  m_app_class;
		std::string                  m_class, m_object;

		signalrcpp::hub_callback m_callback;

		int         send_login();

		void        init(signalrcpp::hub_args* pargs);
	public:
		hub_ace_impl(signalrcpp::hub_args* pargs);

		long        start(signalrcpp::hub_args* pargs);
		long        stop();
		long        set_callback(signalrcpp::hub_callback callback){ m_callback = callback; return 0;}

		long        send_filter(const char* class_1, const char* app_1);
		long        broadcast(void* msg, int msg_len);
		long        post(signalrcpp::hub_request_ptr m);
		long        send(signalrcpp::hub_request_ptr m, unsigned long timeout_millseconds);

		signalrcpp::hub_request_ptr create_request();

		void        message_loop();
	protected:
		long        handle_app_call(const std::string& app_class, const std::string& app_command, const void* data, int data_len);
		long        handle_app_call_result(const std::string& app_class, const std::string& app_command, const void* data, int data_len);
	};


}

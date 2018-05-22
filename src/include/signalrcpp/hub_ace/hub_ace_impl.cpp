#include <assert.h>
#include "hub_ace_impl.h"
#include "../../../indicator-config/header/indicator-config.h"

#include "../../../indicator-log/header/indicator-log.h"
#include "../../../indicator-log/header/indicator-log-func.h"
#include "../../../include/string/string_map.h"
#include "../../../include/string/string_util.h"
#include "../../../include/string/string_map_serializer.h"
#include "../../../include/signalrcpp/hub_initor.h"

#include "../../thread/thread_mutex.h"
#include "../../logger/print.h"
#include "../hub_com/hub_com_request_impl.h"
#include "../hub_com/hub_com_response_impl.h"

#include <stdio.h>
//////////////////////////////////////////////////////////////////////////
extern indicator::indicator_ace_ptr      ace;
extern indicator::indicator_config_ptr   config;

//////////////////////////////////////////////////////////////////////////

namespace signalrcpp_hub_com {
	void message_loop();
}

//////////////////////////////////////////////////////////////////////////
namespace signalrcpp_hub_ace
{
	//////////////////////////////////////////////////////////////////////////

	class hub_request_impl: public signalrcpp_hub_com::hub_com_request_impl
	{
	public:
	};

	//////////////////////////////////////////////////////////////////////////
	class hub_response_impl: public signalrcpp_hub_com::hub_com_response_impl
	{
	public:
	};

	//////////////////////////////////////////////////////////////////////////
	cosmos::string_map_ptr app_map(const std::string& app_class1, const std::string& app_command1, const void* data, int data_len)
	{
		std::string app_class,app_command,app_action;

		auto m = cosmos::string_map_create(app_class1.c_str());
		m->set(500, (char*)data, data_len);
		return m;
	}

	//////////////////////////////////////////////////////////////////////////
	hub_ace_impl::hub_ace_impl(signalrcpp::hub_args * pargs)
	{
		init(pargs);
	}

	//////////////////////////////////////////////////////////////////////////
	long hub_ace_impl::send_filter(const char* class_1, const char* app_1)
	{
		char buf[1024];
		//
		//添加
		char* todayCanUse = "+";
		if(class_1)
			m_class = class_1;
		if(app_1)
			m_object = app_1;

		char filter[1024];
		sprintf(filter,"%s:%s", m_class.c_str(),m_object.c_str());
		sprintf(buf, "201=%s,307=%s,999=MA", todayCanUse, filter);

		INDICATOR_LOG_FUNC(module_id,indicator_log_debug,"filter : "<<buf);

		if( ! m_client )
		{
			INDICATOR_LOG_FUNC(module_id,indicator_log_error,"client not connected");
			return -1;
		}

		{
			this->m_client->send(buf, strlen(buf));
		}
		return 0;
	}

	int hub_ace_impl::send_login()
	{
		if( m_app_class.size() == 0 )
			return 0;

		char buf[1024];
		//
		//添加
		sprintf(buf, "111=%s,999=L", m_app_class.c_str() );

		INDICATOR_LOG_FUNC(module_id,indicator_log_debug,"login : "<<buf);

		if( ! m_client )
		{
			INDICATOR_LOG_FUNC(module_id,indicator_log_error,"client not connected");
			return -1;
		}

		{
			this->m_client->send(buf, strlen(buf));
		}

		return 0;
	}

	void hub_ace_impl::init(signalrcpp::hub_args * pargs)
	{
		auto ac = config->getAppClass();
		if (ac)
			this->m_app_class = ac->ansi_str()->char_buffer();
		if (pargs)
		{
			if (pargs->app_class)
				this->m_class = pargs->app_class;
			if (pargs->app_command)
				this->m_object = pargs->app_command;
		}
	}

	long hub_ace_impl::start(signalrcpp::hub_args* pargs1)
	{
		init(pargs1);

		assert(m_callback);
		//禁用vld
		static bool m_vld_enabled  = true;
		if ( m_vld_enabled )
		{
#ifdef _DEBUG
			//VLDGlobalDisable();
#endif
			m_vld_enabled = false;
		}

		if (!m_ace)
		{
			m_ace = ace->create_socket_init();
			indicator::FUNC_ON_SOCKET_CHANGED func = [this](indicator::on_socket_changed_args* pargs)
			{
				if (pargs->reason == indicator::on_socket_changed::OPEN)
				{
					send_login();
					send_filter(0, 0);
				}

				signalrcpp::hub_response_ptr res(new hub_response_impl());
				switch (pargs->reason)
				{
				case  indicator::on_socket_changed::OPEN:
				{
					res->event_id = HUB_CONNECTED;
					if(m_callback) m_callback(res);
					break;
				}
				case  indicator::on_socket_changed::CLOSED:
				{
					res->event_id = HUB_DISCONNECTED;
					if (m_callback) m_callback(res);
					break;
				}
				case indicator::on_socket_changed::RECV_DATA:
				{
					res->event_id = HUB_ON_DATA;
					std::string class_name;
					std::string object_name;
					cosmos::string_item_ptr value;
					{
						//string_map_serializer d((const char*)pargs->lParam, (int)pargs->wParam);	
						//d.parse();
						auto pbuf = (const char*)pargs->lParam;
						auto d = cosmos::string_map_from_string(pbuf, (int)(intptr_t)pargs->wParam);

						if(d)
						{
							auto s306 = d->get(306);
							auto s307 = d->get(307);
							auto s500 = d->get(500);
							if (s306)
								class_name = s306->c_str();
							if (s307)
								object_name = s307->c_str();
							if (s500)
								value = s500;

							//////////////////////////////////////////////////////////////////////////
							//删除数据500, 数据已经通过v3进行传递
							d->remove(500);

							//////////////////////////////////////////////////////////////////////////
							//把所有参数，序列化到 variant中，方便别人处理
							if (class_name.size() == 0)
							{
								auto s_fields = d->to_string();
								//////////////////////////////////////////////////////////////////////////
								class_name = s_fields->c_str();
							}
						}//map
					}
					{

						res->classs_name = signalrcpp::struct_string_impl::create(class_name);
						res->object_name = signalrcpp::struct_string_impl::create(object_name);
						if (value)
						{
							res->data = signalrcpp::struct_string_impl::create(value->c_str(),value->size());
							//PRINTF("ace impl: res=%p, data len=%d\n", res.get(), res->data->len);
						}
					}
					if (m_callback) m_callback(res);
					break;
				}
				}
			};
			///必须分两步：创建，开始；否则 com对象无法连接到服务器
			m_client = ace->create_client(func);
			m_client->start();
		}
		return 0;
	}

	long hub_ace_impl::stop()
	{
		if( m_client )
		{
			m_client->stop();
			m_client = nullptr;
		}
		m_ace = 0;

		return 0;
	}

	long hub_ace_impl::send(signalrcpp::hub_request_ptr res, unsigned long timeout_millseconds)
	{
		//PRINTF("ace impl: send:%p\n", res.get());

		std::string s_class, s_object;
		s_class  = res->app_class;
		s_object = res->app_command;

		//////////////////////////////////////////////////////////////////////////
		//app call
		auto app_call_result = strstr(s_class.c_str(), "999=App1") != 0;

		if( strstr(s_class.c_str(), "=") 
			&& strstr(s_class.c_str(), ",") 
			&& ! app_call_result)
		{
			return handle_app_call( s_class, s_object, res->data, res->data_len);
		}
		//////////////////////////////////////////////////////////////////////////
		//app call result
		if( app_call_result )
		{
			return handle_app_call_result(s_class, s_object, res->data, res->data_len);
		}

		if( ! m_client )
			return -1;
		//////////////////////////////////////////////////////////////////////////
		//广播
		if(s_class.size()==0)
			s_class = m_class;
		if(s_object.size() == 0 )
			s_object = m_object;

		//发送请求：更新行情
		//更新行情
		{
			auto m = cosmos::string_map_create(nullptr);
			m->set(306, s_class.c_str());
			m->set(307, s_object.c_str());
			m->set(999,"MA1");
			{
				m->set(500, (char*)res->data, res->data_len);
			}

			{
				auto request = string_map_to_bin_string(m.get());
				m_client->send( request->c_str(), request->size() );
			}
		}

		return 0;
	}

	long hub_ace_impl::handle_app_call( const std::string& app_class1, const std::string& app_command1, const void* data, int data_len )
	{
		auto m = app_map(app_class1, app_command1, data, data_len);
		//////////////////////////////////////////////////////////////////////////
		//报文
		m->set(999,"App");
		//////////////////////////////////////////////////////////////////////////
		//发送
		if( ! m_client )
		{
			INDICATOR_LOG_FUNC(module_id,indicator_log_error,"client not connected");
			return -1;
		}

		{
			auto s_str = cosmos::string_map_serializer::to_binary_string(m.get());
			this->m_client->send( s_str.c_str(), s_str.size());
		}
		return 0;
	}

	long hub_ace_impl::handle_app_call_result( const std::string& app_class1, const std::string& app_command1, const void* data, int data_len )
	{
		auto m = app_map(app_class1, app_command1, data, data_len);

		//////////////////////////////////////////////////////////////////////////
		//发送
		if( ! m_client )
		{
			INDICATOR_LOG_FUNC(module_id,indicator_log_error,"client not connected");
			return -1;
		}

		{
			auto s_str = cosmos::string_map_serializer::to_binary_string(m.get());
			this->m_client->send( s_str.c_str() , s_str.size());
		}
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	long hub_ace_impl::broadcast(void* msg, int msg_len)
	{
		assert(false);
		return -1;
	}

	long hub_ace_impl::post(signalrcpp::hub_request_ptr m)
	{
		assert(false);
		return -1;
	}

	signalrcpp::hub_request_ptr create_hub_request_ptr()
	{
		signalrcpp::hub_request_ptr ret(new hub_request_impl());
		return ret;
	}

	signalrcpp::hub_request_ptr hub_ace_impl::create_request()
	{
		return create_hub_request_ptr();
	}

	void hub_ace_impl::message_loop()
	{
#ifdef _WIN32
		signalrcpp_hub_com::message_loop();
#else
		//getch();
#endif
	}

}


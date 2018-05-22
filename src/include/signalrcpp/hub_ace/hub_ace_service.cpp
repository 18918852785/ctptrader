#ifdef _WIN32
#include <windows.h>
#include <Shlwapi.h>
#endif

#include "hub_ace_service.h"
#include <stdio.h>
#include "../hub_com/hub_com_request_impl.h"
#include "../../string/string_map.h"
#include "../../logger/print.h"
#include "../../thread/thread_cond.h"
#include "../../dll_loader/dll_factory.h"

#ifdef _WIN32
#include <windows.h>
#endif
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#define POST_LOCK()    ::thread::thread_scoped_lock_ptr locker( g_hub.get()->create_scoped_lock_ptr(m_post_mutex.get()));

#define MSG_IMPL(a_)   ((signalrcpp_hub_com::hub_com_request_impl*)a_.get())
#define MSG_PTR        MSG_IMPL(req)
#define POST_MSG_PTR   MSG_IMPL(m_post_msgs[seq])

//////////////////////////////////////////////////////////////////////////
static dll::dll_hub_factory g_hub;

namespace signalrcpp_hub_ace_service
{
	hub_ace_service::hub_ace_service(signalrcpp::hub_args* pargs)
		:hub_ace_impl(pargs)
		,m_post_seq(0)
	{
		static dll::dll_hub_factory g_hub;
		m_post_mutex = g_hub.get()->create_mutex_ptr();

		init(pargs);
	}

	void hub_ace_service::init(signalrcpp::hub_args * pargs)
	{
	}

	long hub_ace_service::set_callback(signalrcpp::hub_callback callback)
	{ 
		m_app_callback = callback;

		auto ret = signalrcpp_hub_ace::hub_ace_impl::set_callback([this](signalrcpp::hub_response_ptr res)
		{
			//通知别人数据来了
			if(res)
			{
				//取得 client_seq
				auto fields = cosmos::string_map_create(res->classs_name->ptr);
				/*
				//////////////////////////////////////////////////////////////////////////
				//客户端请求 app
				COMMAND_APP_CALL_CLASS = 121,
				COMMAND_APP_CALL_COMMAND = 122,
				COMMAND_APP_CALL_ACTION = 123,
				COMMAND_APP_CALL_CLIENT_SEQ = 124,
				COMMAND_APP_CALL_DATA = 500,
				COMMAND_APP_CALL_RESULT = 500,

				COMMAND_APP_CALL_SERVER_RESULT = 131,
				COMMAND_APP_CALL_SERVER_SEQ = 132,
				*/
				auto v121 = fields->get(121);
				auto v122 = fields->get(122);
				auto v123 = fields->get(123);
				auto v124 = fields->get(124);
				auto v131 = fields->get(131);
				if( v121 && v122 && v123 && v124 && v131 )
				{
					long long server_result = _atoi64(v131->c_str());
					long long seq = _atoi64(v124->c_str());
					if( server_result == 2 //处理完成
						|| server_result  ==1 //app not found
						)
					{
						void* post_msg_ptr = 0;
						{
							POST_LOCK();//加锁队列
							if( m_post_msgs.count(seq) > 0 )//查找client seq
							{
								//发出app处理完成事件
								auto event = POST_MSG_PTR->data_arrived_event;
								if( event )
								{
									POST_MSG_PTR->response = res;
									event->set_event();
								}
								post_msg_ptr = POST_MSG_PTR;
							}
						}
						//PRINTF("ace service: res=%p, data len=%d, seq=%d, post msg=%p\n", res.get(), res->data->len,seq,post_msg_ptr);
					}
				}
			}

			//////////////////////////////////////////////////////////////////////////
			if( m_app_callback )
			{
				m_app_callback( res );
			}
		});
		return ret;
	}

	long hub_ace_service::send_message(const char* class_name, const char* object_name, const void* data, int data_len)
	{
		auto res = this->create_request();
		res->app_class = class_name;
		res->app_command = object_name;
		res->data = (void*)data;
		res->data_len = data_len;
		return signalrcpp_hub_ace::hub_ace_impl::send(res, -1);
	}

	long hub_ace_service::broadcast(void* msg, int msg_len)
	{
		return send_message(m_class.c_str(),m_object.c_str(),msg, msg_len);
	}

	long hub_ace_service::post(signalrcpp::hub_request_ptr msg)
	{
		std::string c, o;
		if( msg->app_class )
		{
			c = msg->app_class;
			if(msg->app_command)
				o = msg->app_command;
		}else
		{
			c = m_class.c_str();
			o = m_object.c_str();
		}
		if( c.find("121=") != -1
			&& c.find(",") != -1
			&& c.find("999=") != -1)//已经是 121=,122=,123=,999=App格式了
		{
			//999=App格式
		}else//拼装 header 
		{

			char post_to_class[2048]={0};
			sprintf_s(post_to_class, sizeof(post_to_class),
				"121=%s,122=%s,123=send,124=%ld",
				c.c_str(),
				o.c_str(),
				m_post_seq);
			c = post_to_class;
			o.clear();
		}

		//////////////////////////////////////////////////////////////////////////
		auto req = msg;
		MSG_PTR->app_class = c.c_str();
		MSG_PTR->app_command = o.c_str();
		auto hr = signalrcpp_hub_ace::hub_ace_impl::send(req,-1);
		return hr;
	}

	long hub_ace_service::send(signalrcpp::hub_request_ptr req, unsigned long timeout_millseconds)
	{
		//////////////////////////////////////////////////////////////////////////
		//创建数据返回信号量
		static dll::dll_hub_factory g_hub;
		MSG_PTR->data_arrived_event = g_hub.get()->create_event_ptr();
		long seq=0;
		{
			POST_LOCK();//加锁请求队列
			api_InterlockedIncrement(&m_post_seq);
			seq = m_post_seq;
			MSG_PTR->client_seq = seq;
			m_post_msgs[MSG_PTR->client_seq] = req;//请求保存起来
		}
		
		PRINTF("ace service: req:%p, seq=%d\n", req.get(), seq);

		auto hr = post(req);

		if( hr != 0 )
		{
			MSG_PTR->data_arrived_event = nullptr;//释放事件
			
			{
				POST_LOCK();//加锁请求队列
				m_post_msgs.erase(seq);
			}

			PRINTF("ace service: req:%p, seq=%d. hr=%p\n", req.get(), seq, hr);
			return hr;//failed
		}
		{
			//等待服务返回
			auto ret = MSG_PTR->data_arrived_event->wait(timeout_millseconds);

			MSG_PTR->data_arrived_event = nullptr;//释放事件
			{
				POST_LOCK();//加锁请求队列
				m_post_msgs.erase(seq);
			}
			PRINTF("ace service: req:%p, seq=%d. hr=0, wait=%d\n", req.get(), seq, hr, ret);

			if( ret != WAIT_OBJECT_0 )
				return ret;//failed

		}
		return 0;
	}

}


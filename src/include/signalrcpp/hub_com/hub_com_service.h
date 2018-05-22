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

namespace signalrcpp_hub_com
{
	//////////////////////////////////////////////////////////////////////////
	//memory_buffer: 二进制数据
	typedef std::string                     memory_buffer;
	typedef std::shared_ptr<memory_buffer>  memory_buffer_ptr;


	typedef struct _APP_GUID {
		unsigned long  Data1;
		unsigned short Data2;
		unsigned short Data3;
		unsigned char  Data4[ 8 ];
	} APP_GUID;

	//////////////////////////////////////////////////////////////////////////
	//signalrcpp_app
	class                                                      CEventSink;
	typedef std::shared_ptr<CEventSink>                        CEventSink_ptr;
	class                                                      CComPtr_App;
	typedef std::shared_ptr<CComPtr_App>                       CComPtr_App_ptr;

	class hub_com_service
	{
		CEventSink_ptr        m_event;
		CComPtr_App_ptr       m_app;
		unsigned long         m_app_thread_id;//m_app com对象创建时候的 thread id
		unsigned int          m_app_cookie;
		std::string           m_class, m_object;//filter

		volatile long         m_post_seq;
		APP_GUID              m_cls_id, m_iid; 
	public:

		signalrcpp::hub_callback   m_callback;

		hub_com_service();
		~hub_com_service();

	public://server
		long run_server(const char* parameter, bool use_ctrl_handler);

	public://hub client
		long run_hub( const char* filter_class, const char* filter_object, const char* parameter,
			std::function<void(unsigned long cookie)> before_loop,
			unsigned long *cookie,
			bool           use_ctrl_handler
			);

		long hub_start(const char* filter_class, const char* filter_object, const char* parameter, unsigned long *cookie);
		long hub_stop();
	
	public://broadcast/send
		long hub_broadcast            (void* data, int data_len);
		long hub_broadcast_from_thread(void* data, int data_len);


		long hub_post( signalrcpp::hub_request_ptr m );
		long hub_send( signalrcpp::hub_request_ptr m, unsigned long timeout=-1);
	public:
		//windows消息循环
		static void message_loop( bool use_ctrl_handler );//消息循环

	public://com ptr
		long  get_dispatch(void** ppDisp);
		long  register_app(unsigned long* pCookie);
		long  get_dispatch_from_cookie(unsigned long cookie, void** ppDisp);

		void  on_callback(signalrcpp::hub_response_ptr pargs);

	private:
		bool        is_same_thread();//当前线程，和 创建 app com的线程，是否是同一个？
		static int  CtrlHandler( unsigned long fdwCtrlType );
		bool        m_use_ctrl_handler;
		static bool m_ctrl_c_pressed;

	private:
		::thread::thread_mutex_ptr                       m_post_mutex;
		std::unordered_map<long long, signalrcpp::hub_request_ptr>   m_post_msgs;
		void*                                            m_git;

		long init_git();
	};

	typedef std::shared_ptr<hub_com_service> hub_com_ptr;


	signalrcpp::hub* hub_create(signalrcpp::hub_args* pargs = 0);//创建hub对象
	signalrcpp::hub_ptr hub_create_ptr(signalrcpp::hub_args* pargs = 0);//创建hub对象
}

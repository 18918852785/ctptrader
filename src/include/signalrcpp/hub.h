/*
封装 signalrcpp.exe com 对象
*/
#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>

#include "../thread/ithread.h"

#include "hub_initor.h"
#include "hub_request.h"
#include "hub_response.h"

#define HUB_CONNECTED    1
#define HUB_DISCONNECTED 2
#define HUB_ON_DATA      3

namespace thread
{
	struct thread_event;
	typedef std::shared_ptr<thread_event> thread_event_ptr;

	struct thread_mutex;
	typedef std::shared_ptr<thread_mutex> thread_mutex_ptr;

	struct thread_scoped_lock;
	typedef std::shared_ptr<thread_scoped_lock> thread_scoped_lock_ptr;
}


namespace signalrcpp
{
	//////////////////////////////////////////////////////////////////////////
	//signalrcpp_hub
	enum hub_type
	{
		HT_UNKNOWN = 0,
		HT_ACE_SOCKET,//直接通过ace连接到服务器
		HT_ACE_SERVICE,//基于 ACE_SOCKET,提供 app broadcast/post/send 功能
		HT_COM_SERVICE,//通过 signalrcpp.exe com对象访问 hub
	};
	struct hub_args
	{
		int     dwSize;
		hub_type type;
		const char* parameter;
		const char* app_class;
		const char* app_command;
	};

	typedef std::function<void(hub_response_ptr pargs)> hub_callback;

	struct hub
	{
		virtual long        start(hub_args* pargs) = 0;
		virtual long        stop() = 0;
		virtual long        set_callback(hub_callback callback) = 0;

		virtual long        send_filter(const char* class_1, const char* app_1) =0;
		virtual long        broadcast(void* msg, int msg_len) = 0;
		virtual long        post(hub_request_ptr m)=0;
		virtual long        send(hub_request_ptr m, unsigned long timeout_millseconds)=0;

		virtual hub_request_ptr create_request()=0;

		virtual void        message_loop() = 0;
	};

	typedef std::shared_ptr<hub> hub_ptr;

	struct hub_factory{
		//////////////////////////////////////////////////////////////////////////
		//hub
		virtual hub_ptr create_hub(hub_args* pargs) = 0;

		//////////////////////////////////////////////////////////////////////////
		//thread
		virtual ::thread::ithread_ptr create_thread_ptr() = 0;
		virtual ::thread::ithread*    create_thread() = 0;
		virtual void        destroy_thread(::thread::ithread* p) = 0;

		//////////////////////////////////////////////////////////////////////////
		//event
		virtual ::thread::thread_event_ptr create_event_ptr()=0;
		virtual ::thread::thread_mutex_ptr create_mutex_ptr()=0;
		virtual ::thread::thread_scoped_lock_ptr create_scoped_lock_ptr(::thread::thread_mutex* m)=0;
		virtual hub_initor_ptr create_initor()=0;
	};


	typedef std::shared_ptr<hub_factory> hub_factory_ptr;

}

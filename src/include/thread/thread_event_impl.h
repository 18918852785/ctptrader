#pragma once

#include "thread_event.h"

#ifdef _WIN32
#else//linux
#define USE_EVENT_STD 1
#endif

namespace thread
{

	//////////////////////////////////////////////////////////////////////////
	class thread_event_impl : public thread_event
	{
	public:
		thread_event_impl();
		~thread_event_impl();

		void           set_event();
		unsigned long  wait(long millseconds);

		void*          m_obj;
	};
	typedef std::shared_ptr<thread_event_impl> thread_event_impl_ptr;



}//namespace

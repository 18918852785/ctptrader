#pragma once
#include <memory>

#ifdef _WIN32
#else//linux
#define USE_EVENT_STD 1
#endif

namespace thread
{

	//////////////////////////////////////////////////////////////////////////
	struct thread_event
	{
		virtual void           set_event() = 0;
		virtual unsigned long  wait(long millseconds) = 0;

		void*          m_obj;
	};
	typedef std::shared_ptr<thread_event> thread_event_ptr;

}//namespace

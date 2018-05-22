#include "thread_event_impl.h"
#include <algorithm>
#include <assert.h>

#ifdef _WIN32
	#include <Windows.h>
	#include <process.h>
#else
#include <thread>
#include <mutex>                // std::mutex, std::unique_lock
#include <condition_variable>    // std::condition_variable
#endif
#include "../string/string_define.h"

void unittest_printf(const char*fmt, ...);

namespace thread
{

#ifdef _WIN32
	thread_event_impl::thread_event_impl()
	{
		m_obj = CreateEvent(NULL,  // no security
			FALSE, // auto-reset
			FALSE, // non-signaled initially
			NULL); // unnamed
	}

	thread_event_impl::~thread_event_impl()
	{
		if(m_obj)
			CloseHandle((HANDLE)m_obj);
		m_obj=0;
	}

	void thread_event_impl::set_event()
	{
		if(m_obj)
			SetEvent((HANDLE)m_obj);
	}

	unsigned long thread_event_impl::wait( long millseconds )
	{
		if(!m_obj)
			return 0;
		return WaitForSingleObject((HANDLE)m_obj, (DWORD)millseconds);
	}
#endif// _WIN32

#if USE_EVENT_STD  == 1 //std c11
	//http://www.cnblogs.com/haippy/p/3252041.html
	//https://stackoverflow.com/questions/1677070/cross-platform-equivalent-to-windows-events
	//https://stackoverflow.com/questions/33098419/linux-posix-equivalent-for-win32s-createevent-setevent-waitforsingleobject
	class Event;
	typedef Event* Event_handle;
	#define k_INFINITE (-1)

	class Event
	{
		friend thread_event_impl;

		Event(void) : signalled(false) { }

		volatile bool signalled;
		std::mutex m_mutex;
		std::condition_variable m_cond;
	};

	thread_event_impl::thread_event_impl()
	{
		m_obj = new Event();
	}

	thread_event_impl::~thread_event_impl()
	{
		if(m_obj)
			delete ((Event*)m_obj);
		m_obj=0;
	}

	void thread_event_impl::set_event()
	{
		if (m_obj)
		{
			auto evt = (Event*)m_obj;
			std::unique_lock<std::mutex> lock(evt->m_mutex);
			evt->signalled = true;
			evt->m_cond.notify_all();
		}
	}

	unsigned long thread_event_impl::wait( long t)
	{
		if(!m_obj)
			return 0;

		auto evt = (Event*)m_obj;
		int ret = -1;
		std::unique_lock<std::mutex> lock(evt->m_mutex);
		if (t == k_INFINITE)
		{
			while (!evt->signalled)
			{
				evt->m_cond.wait(lock);
			}
			evt->signalled = false;
			ret = 0;
		}
		else
		{
			bool run = true;
			//slightly more complex code for timeouts
			while (!evt->signalled && run)
			{
				auto v_ret = evt->m_cond.wait_for(lock, std::chrono::milliseconds(t));
				switch (v_ret)
				{
				case std::cv_status::no_timeout:
					ret = 0;
					evt->signalled = false;
					run = false;
					break;
				case std::cv_status::timeout:
					ret = WAIT_TIMEOUT;//258
					run = false;
					break;
				default:
					ret = -1;
					break;
				}
			}

		}
		return ret;
	}

#endif


}//namespace thread

#include "thread_mutex_impl.h"
#include <algorithm>
#include <assert.h>

#ifdef _WIN32
	#include <Windows.h>
	#include <process.h>
#else
	#include <mutex>
#endif
#include "../string/string_define.h"

#if USE_MUTEX_STD == 1//linux
typedef std::mutex STD_MUTEX;
#endif

namespace thread
{
#ifdef _WIN32
	//////////////////////////////////////////////////////////////////////////
	//mutext
	thread_mutex_impl::thread_mutex_impl(const char* name)
	{
		m_obj = new CRITICAL_SECTION();
		InitializeCriticalSection( (CRITICAL_SECTION*)m_obj);
	}

	thread_mutex_impl::~thread_mutex_impl()
	{
		::DeleteCriticalSection((CRITICAL_SECTION*)m_obj);

		delete ((CRITICAL_SECTION*)m_obj);
		m_obj = 0;
	}

	long thread_mutex_impl::lock()
	{
		::EnterCriticalSection((CRITICAL_SECTION*)m_obj);
		return 0;
	}

	long thread_mutex_impl::lock(long millseconds)
	{
		DWORD dwStart = GetTickCount();
		while( GetTickCount() - dwStart < (DWORD)millseconds )
		{
			BOOL b = ::TryEnterCriticalSection((CRITICAL_SECTION*)m_obj);
			if( b )
				return 0;//ok
		}
		return WAIT_TIMEOUT;
	}

	long thread_mutex_impl::unlock()
	{
		::LeaveCriticalSection((CRITICAL_SECTION*)m_obj);
		return 0;
	}
#endif//win32

#if USE_MUTEX_API == 1
	//////////////////////////////////////////////////////////////////////////
	//mutext
	thread_mutex_impl::thread_mutex_impl(const char* name)
	{
		if (name)
			m_name = name;
		m_obj = api_CreateMutex(nullptr, TRUE, m_name.c_str());
	}

	thread_mutex_impl::~thread_mutex_impl()
	{
		if(m_obj)
			api_CloseHandle(m_obj);
		m_obj = 0;
	}

	long thread_mutex_impl::lock()
	{
		if(m_obj)
			return	api_WaitForSingleObject(m_obj, INFINITE);
		while (!m_obj && m_name.size()>0)
		{
			m_obj = api_CreateMutex(nullptr, TRUE, m_name.c_str());
			if (m_obj)
				break;
			api_Sleep(200);
		}
		return m_obj?WAIT_OBJECT_0: WAIT_FAILED;
	}

	long thread_mutex_impl::lock(long millseconds)
	{
		return lock();
	}

	long thread_mutex_impl::unlock()
	{
		if(m_obj)
			return api_ReleaseMutex(m_obj) ? 0 : -1;
		return -1;
	}

#endif//USE_API


#if USE_MUTEX_STD == 1//linux
	thread_mutex_impl::thread_mutex_impl(const char* name )
		{
			assert(!name);
			m_obj = new STD_MUTEX();
		}
	thread_mutex_impl::~thread_mutex_impl()
		{
			if(m_obj)
			{
				delete ((STD_MUTEX*)m_obj);
				m_obj=0;
			}
		}

		long      thread_mutex_impl::lock()
		{
			((STD_MUTEX*)m_obj)->lock();
			return 0;
		}
		long      thread_mutex_impl::lock(long millseconds)
		{
			lock();
			return 0;
		}
		long           thread_mutex_impl::unlock()
		{
			((STD_MUTEX*)m_obj)->unlock();
			return 0;
		}

#endif//USE_STD



}//namespace thread

#include "thread_mutex_global.h"
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


namespace thread
{
	//////////////////////////////////////////////////////////////////////////
	//mutext
	thread_mutex_global::thread_mutex_global(const char* name)
	{
#ifdef _WIN32
		m_obj = new CRITICAL_SECTION();
		InitializeCriticalSection( (CRITICAL_SECTION*)m_obj);
#else
		if (name)
			m_name = name;
		m_obj = api_CreateMutex(nullptr, TRUE, m_name.c_str());
#endif
	}

	thread_mutex_global::~thread_mutex_global()
	{
#ifdef _WIN32
		::DeleteCriticalSection((CRITICAL_SECTION*)m_obj);
	
		delete ((CRITICAL_SECTION*)m_obj);
#else
		if(m_obj)
			api_CloseHandle(m_obj);
#endif
		m_obj = 0;
	}

	long thread_mutex_global::lock()
	{
#ifdef _WIN32
		::EnterCriticalSection((CRITICAL_SECTION*)m_obj);
		return 0;
#else
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
#endif
	}

	long thread_mutex_global::lock(long millseconds)
	{
#ifdef _WIN32
		DWORD dwStart = GetTickCount();
		while( GetTickCount() - dwStart < (DWORD)millseconds )
		{
			BOOL b = ::TryEnterCriticalSection((CRITICAL_SECTION*)m_obj);
			if( b )
				return 0;//ok
		}
		return WAIT_TIMEOUT;
#else
		return lock();
#endif
	}

	long thread_mutex_global::unlock()
	{
#ifdef _WIN32
		::LeaveCriticalSection((CRITICAL_SECTION*)m_obj);
		return 0;
#else
		if(m_obj)
			return api_ReleaseMutex(m_obj) ? 0 : -1;
		return -1;
#endif
	}


}//namespace thread

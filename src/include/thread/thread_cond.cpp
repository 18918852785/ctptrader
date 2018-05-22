#include "thread_cond.h"
#include "thread_mutex_impl.h"
#include <algorithm>

#ifdef _WIN32
	#include <Windows.h>
	#include <process.h>
#else
#include <mutex>
#include <chrono>
#include <condition_variable>    // std::condition_variable
#endif
#include "../string/string_define.h"

#if USE_SCOPELOCK_STD == 1//linux
typedef std::mutex STD_MUTEX;
typedef std::unique_lock<STD_MUTEX> SCOPED_LOCK;
#endif

#if USE_COND_STD == 1
typedef std::condition_variable STD_COND;
#endif

void unittest_printf(const char*fmt, ...);

namespace thread
{
	//子线程函数
#ifdef _WIN32
	static unsigned int __stdcall ThreadFun(void* pM)  
#else
	static void* ThreadFun(void* pM)  
#endif
	{  
		thread_group* pthis = (thread_group*)pM;
		pthis->m_func();
		pthis->m_func = 0;


#ifdef THREAD_TRACE
		unittest_printf("thread end\n");
#endif
		return 0;  
	}  


	void thread_group::create_thread( std::function<void()> f )
	{
		m_func = f;

		m_thread = (HANDLE)api_beginthreadex(0,0,ThreadFun, this, 0, 0);
		m_thread_id = api_GetCurrentThreadId();//GetThreadId(m_thread);
#ifdef THREAD_TRACE
		unittest_printf("thread start:handle=0x%X, id=%d\n", m_thread, m_thread_id);
#endif
	}

	void thread_group::join_all()
	{
#ifdef _WIN32
		if(m_thread)
		{
			//::MsgWaitForMultipleObjects(1,&m_thread,TRUE,INFINITE,QS_ALLEVENTS);
			WaitForSingleObject( (HANDLE)m_thread, INFINITE );
			CloseHandle((HANDLE)m_thread);
			m_thread = 0;
		}
#else
		if (m_thread)
		{
			api::WaitForThread((HANDLE)m_thread, INFINITE);
			//api::CloseHandle((HANDLE)m_thread);
			m_thread = 0;
		}
#endif
	}


	//////////////////////////////////////////////////////////////////////////
	//thread_scoped_lock_impl
#ifdef _WIN32
	thread_scoped_lock_impl::thread_scoped_lock_impl( thread_mutex* m )
		:m_mutex(m)
	{
		m->lock();
	}

	thread_scoped_lock_impl::~thread_scoped_lock_impl()
	{
		m_mutex->unlock();
	}
#endif// _WIN32

#if USE_SCOPELOCK_STD  == 1
	thread_scoped_lock_impl::thread_scoped_lock_impl(thread_mutex* m)
	{
		auto p = (STD_MUTEX*)((thread_mutex_impl*)m)->m_obj;
		m_obj = new SCOPED_LOCK(*p);
	}

	thread_scoped_lock_impl::~thread_scoped_lock_impl()
	{
		delete ((SCOPED_LOCK*)m_obj);
	}
#endif//USE_SCOPELOCK_STD 

	//////////////////////////////////////////////////////////////////////////
	//condition

	typedef struct {

	}pthread_condattr_t;

	//////////////////////////////////////////////////////////////////////////
	//condition 3
	//http://www.cs.wustl.edu/~schmidt/win32-cv-1.html
	//Strategies for Implementing POSIX Condition Variables on Win32
#if 1
	typedef HANDLE pthread_mutex_t;

	typedef struct
	{
		int waiters_count_;
		// Number of waiting threads.

#ifdef _WIN32
		CRITICAL_SECTION waiters_count_lock_;
#endif
		// Serialize access to <waiters_count_>.

		HANDLE sema_;
		// Semaphore used to queue up threads waiting for the condition to
		// become signaled. 

		HANDLE waiters_done_;
		// An auto-reset event used by the broadcast/signal thread to wait
		// for all the waiting thread(s) to wake up and be released from the
		// semaphore. 

		size_t was_broadcast_;
		// Keeps track of whether we were broadcasting or signaling.  This
		// allows us to optimize the code if we're just signaling.
	} my_pthread_cond_t;

	int 
		pthread_cond_init (my_pthread_cond_t *cv,
		const pthread_condattr_t *)
	{
#ifdef _WIN32
		cv->waiters_count_ = 0;
		cv->was_broadcast_ = 0;
		cv->sema_ = CreateEvent(NULL,  // no security
			FALSE, // auto-reset
			FALSE, // non-signaled initially
			NULL); // unnamed
		InitializeCriticalSection (&cv->waiters_count_lock_);
		cv->waiters_done_ = CreateEvent (NULL,  // no security
			FALSE, // auto-reset
			FALSE, // non-signaled initially
			NULL); // unnamed
#endif
		return 0;
	}
#if 0
#define my_SignalObjectAndWait SignalObjectAndWait
#else
	DWORD my_SignalObjectAndWait(
		thread_scoped_lock* hObjectToSignal,
		HANDLE hObjectToWaitOn,
		DWORD dwMilliseconds,
		BOOL bAlertable
		)
	{
#ifdef _WIN32
		((thread_scoped_lock_impl*)hObjectToSignal)->m_mutex->unlock();
		
		DWORD dwRet = WaitForSingleObjectEx(hObjectToWaitOn, 
			//INFINITE,
			dwMilliseconds, 
			bAlertable);
		if( dwRet != WAIT_OBJECT_0 )
		{
			((thread_scoped_lock_impl*)hObjectToSignal)->m_mutex->lock();
		}
		return dwRet;
#endif
	}
#endif//my_SignalObjectAndWait
	int
		pthread_cond_wait (my_pthread_cond_t *cv, 
		thread_scoped_lock *external_mutex,
		DWORD timeout_millseconds)
	{
#ifdef _WIN32

		// Avoid race conditions.
		EnterCriticalSection (&cv->waiters_count_lock_);
		cv->waiters_count_++;
		LeaveCriticalSection (&cv->waiters_count_lock_);

		// This call atomically releases the mutex and waits on the
		// semaphore until <pthread_cond_signal> or <pthread_cond_broadcast>
		// are called by another thread.
		if( WAIT_OBJECT_0 != my_SignalObjectAndWait ( external_mutex, cv->sema_, timeout_millseconds, TRUE) )
		{
			return WAIT_TIMEOUT;
		}

		// Reacquire lock to avoid race conditions.
		EnterCriticalSection (&cv->waiters_count_lock_);

		// We're no longer waiting...
		cv->waiters_count_--;

		// Check to see if we're the last waiter after <pthread_cond_broadcast>.
		int last_waiter = cv->was_broadcast_ && cv->waiters_count_ == 0;

		LeaveCriticalSection (&cv->waiters_count_lock_);

		// If we're the last waiter thread during this particular broadcast
		// then let all the other threads proceed.
		if (last_waiter)
			// This call atomically signals the <waiters_done_> event and waits until
			// it can acquire the <external_mutex>.  This is required to ensure fairness. 
		{
			//if( WAIT_TIMEOUT == SignalObjectAndWait (cv->waiters_done_, external_mutex, timeout_millseconds, FALSE) )
			//	return WAIT_TIMEOUT;
		}
		else
		{
		}
		((thread_scoped_lock_impl*)external_mutex)->m_mutex->lock();
#endif
		return 0;
	}

	int
		pthread_cond_signal (my_pthread_cond_t *cv)
	{
#ifdef _WIN32

		EnterCriticalSection (&cv->waiters_count_lock_);
		int have_waiters = cv->waiters_count_ > 0;
		LeaveCriticalSection (&cv->waiters_count_lock_);

		// If there aren't any waiters, then this is a no-op.  
		//if (have_waiters)
		SetEvent(cv->sema_);
#endif
		return 0;
	}

	int
		pthread_cond_broadcast (my_pthread_cond_t *cv)
	{
#ifdef _WIN32
		// This is needed to ensure that <waiters_count_> and <was_broadcast_> are
		// consistent relative to each other.
		EnterCriticalSection (&cv->waiters_count_lock_);
		int have_waiters = 0;

		if (cv->waiters_count_ > 0) {
			// We are broadcasting, even if there is just one waiter...
			// Record that we are broadcasting, which helps optimize
			// <pthread_cond_wait> for the non-broadcast case.
			cv->was_broadcast_ = 1;
			have_waiters = 1;
		}

		//if (have_waiters) 
		{
			// Wake up all the waiters atomically.
			SetEvent(cv->sema_);//, cv->waiters_count_, 0);

			LeaveCriticalSection (&cv->waiters_count_lock_);

			// Wait for all the awakened threads to acquire the counting
			// semaphore. 
			//WaitForSingleObject (cv->waiters_done_, 0/*INFINITE*/);
			// This assignment is okay, even without the <waiters_count_lock_> held 
			// because no other waiter threads can wake up to access it.
			cv->was_broadcast_ = 0;
		}
#endif
		return 0;
	}
#endif

#ifdef _WIN32
	thread_cond::thread_cond()
	{
		m_obj = new my_pthread_cond_t();
		api_ZeroMemory(m_obj, sizeof(my_pthread_cond_t));
		pthread_cond_init((my_pthread_cond_t*)m_obj,0);
	}

	thread_cond::~thread_cond()
	{
		if( ! m_obj )
			return;
		auto cv = (my_pthread_cond_t*)m_obj;
		CloseHandle(cv->sema_);
		DeleteCriticalSection(&cv->waiters_count_lock_);
		CloseHandle(cv->waiters_done_);

		delete cv;
		m_obj = 0;
	}


	int thread_cond::wait( thread_scoped_lock* m, timespec* timeout )
	{
		DWORD millseconds = 0;
		if( timeout )
		{
			millseconds = (DWORD)(timeout->tv_sec*1000 + timeout->tv_nsec);
		}
		//////////////////////////////////////////////////////////////////////////
		return (int)pthread_cond_wait( (my_pthread_cond_t*)m_obj, m, millseconds);
	}

	void thread_cond::notify()
	{
		pthread_cond_broadcast( (my_pthread_cond_t*)m_obj);
	}
#endif//win32


#if USE_COND_STD == 1

	thread_cond::thread_cond()
	{
		m_obj = new STD_COND();
	}

	thread_cond::~thread_cond()
	{
		auto cv = (STD_COND*)m_obj;
		delete cv;
		m_obj = 0;
	}


	int thread_cond::wait(thread_scoped_lock* m, timespec* timeout)
	{
		DWORD millseconds = 0;
		if (timeout)
		{
			millseconds = (DWORD)(timeout->tv_sec * 1000 + timeout->tv_nsec);
		}
		auto cv = (STD_COND*)m_obj;
		auto lk = (SCOPED_LOCK*)((thread_scoped_lock_impl*)m)->m_obj;
		
		int ret = -1;
		if (millseconds)
		{
			switch (cv->wait_for(*lk, std::chrono::milliseconds(millseconds)))
			{
			case std::cv_status::no_timeout:
				ret = 0;
				break;
			case std::cv_status::timeout:
				ret = WAIT_TIMEOUT;//258
				break;
			default:
				ret = -1;
				break;
			}
		}
		else
		{
			cv->wait(*lk);
			ret = 0;
		}
		//////////////////////////////////////////////////////////////////////////
		return ret;
	}

	void thread_cond::notify()
	{
		auto cv = (STD_COND*)m_obj;
		cv->notify_all();
	}

#endif//USE_COND_PTHREAD 
}//namespace thread

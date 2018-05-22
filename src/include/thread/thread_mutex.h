#pragma once
#include <memory>
#include <string>

#ifdef _WIN32
#else//linux
	#define USE_MUTEX_API 0
	#define USE_MUTEX_STD 1
#endif

namespace thread
{

	struct thread_mutex
	{
		virtual long           lock() = 0;
		virtual long           lock(long millseconds) = 0;
		virtual long           unlock() = 0;
	};
	typedef std::shared_ptr<thread_mutex> thread_mutex_ptr;

	struct thread_scoped_lock
	{
	};
	typedef std::shared_ptr<thread_scoped_lock> thread_scoped_lock_ptr;



}//namespace

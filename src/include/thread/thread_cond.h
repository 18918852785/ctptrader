#pragma once
#include <memory>
#include <unordered_map>
#include <queue>
#include <string>
#include <functional>

#include "thread_mutex.h"

#ifdef _WIN32
#else//linux
#define USE_SCOPELOCK_STD 1
#define USE_COND_STD 1
#endif

namespace thread
{
	struct timespec 
	{
		time_t tv_sec;
		long   tv_nsec;
	};

	class thread_cond
	{
	public:
		thread_cond();
		~thread_cond();

		int wait(thread_scoped_lock* m, timespec* timeout);
		void notify();

		void* m_obj;
	};

	class thread_group
	{
	public:
		thread_group():m_thread(0),m_thread_id(0)
		{

		}
		~thread_group(){join_all();}
		void join_all();

		void create_thread(std::function<void()> f);
		void* m_thread;
		unsigned long m_thread_id;

		std::function<void()> m_func;
	};
	typedef std::shared_ptr<thread_group> thread_group_ptr;



}//namespace

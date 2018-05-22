#pragma once
#include "thread_mutex.h"

namespace thread
{

	class thread_mutex_impl : public thread_mutex
	{
	public:
		thread_mutex_impl(const char* name = nullptr);
		~thread_mutex_impl();

		long           lock();
		long           lock(long millseconds);
		long           unlock();

		void* m_obj;
	private:
		std::string m_name;
	};
	typedef std::shared_ptr<thread_mutex_impl> thread_mutex_impl_ptr;


	class thread_scoped_lock_impl :public thread_scoped_lock
	{
	public:
		thread_scoped_lock_impl(thread_mutex* m);
		~thread_scoped_lock_impl();

#if USE_SCOPELOCK_STD  == 1
		void* m_obj;
#else
		thread_mutex* m_mutex;
#endif
	};
	typedef std::shared_ptr<thread_scoped_lock_impl> thread_scoped_lock_impl_ptr;

}//namespace

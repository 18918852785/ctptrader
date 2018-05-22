#pragma once

namespace thread
{

	class thread_mutex_global
	{
	public:
		thread_mutex_global(const char* name = nullptr);
		~thread_mutex_global();

		long           lock();
		long           lock(long millseconds);
		long           unlock();

		void* m_obj;
	private:
		std::string m_name;
	};

}//namespace

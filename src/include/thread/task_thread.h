#pragma once

#define TASK_THREAD_ENABLE 0
#define CONDATION_WAIT_TIMEOUT_SECONDS     3
#define CONDATION_WAIT_TIMEOUT_MILLSECONDS 0


#include <string>
#include <unordered_map>
#include <queue>
#include <functional>
#include "thread_mutex.h"
#include "thread_cond.h"

#define THREAD_UNITTEST 1
#include "ithread.h"

namespace thread
{
	class delayed_task
	{
	public:
		delayed_task(std::shared_ptr<thread_mutex> task_queue_mutex)
			:m_task_queue_mutex(task_queue_mutex)
		{

		}
		class task_item
		{
		public:
			std::string            m_id;
			volatile time_t        m_first_delay_millseconds;//第一次执行，要等待的时间
			volatile time_t        m_delayed_millseconds;    //以后执行要等待的时间
			std::function<void()>  m_task;

		    volatile time_t                 m_next_run_time;
			volatile bool                   m_deleted, m_running;
			volatile long                   m_run_count;
			bool                            run();
			void                            set_next_run_time();
		};
		typedef std::shared_ptr<task_item>  task_item_ptr;

		void remove_task(const std::string& task_id);
		void put_task(const std::string& task_id, int first_delay_millseconds, int delay_millseconds, std::function<void()> func);

		void run();
		void get_ready_task( std::vector<std::string>& task_ids);
		void remove_task();

		std::unordered_map<std::string, std::shared_ptr<task_item>>   m_tasks;
		std::shared_ptr<thread_mutex>                                 m_task_queue_mutex;
	};

	class task_thread : public ithread
	{
	public:
		class task_item
		{
		public:
			enum TASK_ID{ TASK_UNKNOWN = 0, TASK_QUIT = 1, TASK_USER = 2};
			TASK_ID                         id;
			std::function<void(void* args)> run;
		};

	public:
		task_thread();
		~task_thread();

		bool start_thread();
		void stop_thread();
	
		public://ithread
			int start(){ return start_thread()?0:-1;}
			int stop(){ stop_thread(); return 0; }
			int add_task (std::function<void(     )> func);
			int add_task2(std::function<void(void*)> func, void* args2);
			int add_task3(std::function<void(void*,void*)> func, void* args2, void* args3);
			int add_task4(std::function<void(void*,void*,void*)> func, void* args2, void* args3, void* args4);
			int add_task5(std::function<void(void*,void*,void*,void*)> func, void* args2, void* args3, void* args4, void* args5);

			void unit_test();
	private:
		//////////////////////////////////////////////////////////////////////////
		// 线程入口函数
		void run();
	public:
		void add_task(const std::string& task_id, int first_delay_millseconds, int delay_millseconds, std::function<void()> func);
		void add_task(task_item task);

	public:
		//timer
		void add_timer(const std::string& task_id, int first_delay_millseconds, int delay_millseconds, std::function<void()> func);
		void delete_timer(const std::string& task_id);
		void job_yield();
	private:
		thread_group_ptr									m_threads;
		std::shared_ptr<std::queue< task_item >>            m_task_queue;
		std::shared_ptr<thread_mutex>						m_task_queue_mutex;
		std::shared_ptr<thread_cond>						m_cond;

		std::shared_ptr<delayed_task>						m_delayed_task;
		bool                                                m_running;
	};

	typedef std::shared_ptr<task_thread>					task_thread_ptr;
}//namespace
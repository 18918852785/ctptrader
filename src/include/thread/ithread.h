#pragma once

#include <functional>
#include <memory>

namespace thread
{
	struct ithread
	{
		virtual int start() = 0;
		virtual int stop() = 0;
		virtual int add_task (std::function<	void(     )> task) = 0;
		virtual int add_task2(std::function<void(void*)> task, void* args2) = 0;
		virtual int add_task3(std::function<void(void*,void*)> func, void* args2, void* args3) = 0;
		virtual int add_task4(std::function<void(void*,void*,void*)> func, void* args2, void* args3, void* args4) = 0;
		virtual int add_task5(std::function<void(void*,void*,void*,void*)> func, void* args2, void* args3, void* args4, void* args5) = 0;
		
		//timer
		virtual void add_timer(
			const std::string& task_id    /*字符串id,外部管理，标识timer*/, 
			int first_delay_millseconds   /*第一次执行的延时. 0表示立即执行 */, 
			int delay_millseconds         /*以后执行的延时. 0表示只执行一次就自动被删除timer*/,
			std::function<void()> func    /*timer的回调*/ ) = 0;

		virtual void delete_timer(const std::string& task_id) = 0;

		virtual void        unit_test()=0;
	};
	typedef std::shared_ptr<ithread> ithread_ptr;

}
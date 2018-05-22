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
			const std::string& task_id    /*�ַ���id,�ⲿ������ʶtimer*/, 
			int first_delay_millseconds   /*��һ��ִ�е���ʱ. 0��ʾ����ִ�� */, 
			int delay_millseconds         /*�Ժ�ִ�е���ʱ. 0��ʾִֻ��һ�ξ��Զ���ɾ��timer*/,
			std::function<void()> func    /*timer�Ļص�*/ ) = 0;

		virtual void delete_timer(const std::string& task_id) = 0;

		virtual void        unit_test()=0;
	};
	typedef std::shared_ptr<ithread> ithread_ptr;

}
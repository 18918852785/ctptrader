/*

*/
#pragma once

namespace signalr_cpp
{
	//命令行参数: /client_processid:7878
	//监控 进程7878,当它退出后,自己也退出
	class proc_monitor
	{
	public:
		proc_monitor();
		virtual ~proc_monitor();

		int start();

		unsigned long m_target_process_id;
		void* m_client_process_handle ;
	};
}
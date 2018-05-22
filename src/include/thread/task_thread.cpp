#include "task_thread.h"
#include <algorithm>
#include <stdarg.h>
#include <assert.h>

#ifdef _WIN32
#include <windows.h>
#include <Shlwapi.h>
#else
#endif

#include "../string/string_define.h"
#include "../dll_loader/dll_factory.h"
#include "../signalrcpp/hub.h"
#include "api.h"
#if 0
#include "../logger/print.h"
#else
#define PRINTF(...) fprintf(stderr, __VA_ARGS__)
#endif

#if THREAD_LOG == 1
	#include "bs_log.h"
#else //THREAD_LOG == 0 
	#define LOG_FUNCTION(...)
	#define LOG_INFO(...)
	#define LOG_DEBUG(...)
#endif//THREAD_LOG

#include <time.h>

#ifdef _WIN32
	#include <Windows.h>
#else
	#include <apr.h>
	#include <apr_file_io.h>
	#include <apr_errno.h>
	#include <apr_pools.h>
	#include <apr_file_info.h>
	#include <apr_env.h>
	#include <apr_global_mutex.h>
	#include <apr_portable.h>
	#include <apr_atomic.h>
	
	#include <unistd.h>

	#include <dlfcn.h>

	#include "apr_handle.h"
	#include "../string/string_conv.h"

	static bool bapr_initialized = false;
	static apr_pool_t *g_pool = NULL;
	class apr_initializer{
	public:
		apr_initializer() {
			if (!bapr_initialized)
			{
				bapr_initialized = true;
				apr_initialize();

				if (apr_pool_create(&g_pool, NULL) != APR_SUCCESS)
				{
					PRINTF("apr_pool_create() failed");
				}

			}
		}
	};
	static apr_initializer g_apr_initializer;

#endif


signalrcpp::hub_factory* get_task_thread_factory();
	void unittest_printf(const char*fmt, ...);

namespace thread
{

//////////////////////////////////////////////////////////////////////////
//task_thread


	task_thread::task_thread()
		:m_running(false)
		,m_task_queue(new std::queue< task_item >())
		,m_cond(new thread_cond())
	{
		m_task_queue_mutex = get_task_thread_factory()->create_mutex_ptr();
		m_delayed_task.reset( new delayed_task(m_task_queue_mutex) );
	}

	task_thread::~task_thread()
	{
		stop_thread();
	}

	bool task_thread::start_thread()
	{
		if( m_running )
			return true;
		m_running = true;

		auto f = std::bind(&task_thread::run, this);
		m_threads.reset(new thread_group());
		m_threads->create_thread( f );
		return true;
	}

	void task_thread::stop_thread()
	{
		if( ! m_running )
			return;

#ifdef THREAD_TRACE
		unittest_printf("stop thread begin\n");
#endif

		//通知子线程退出
		task_item t;
		t.id = task_item::TASK_QUIT;
		add_task( t );
		
#ifdef THREAD_TRACE
		unittest_printf("stop thread : waiting thread to end...\n");
#endif

		m_threads = nullptr;
#ifdef THREAD_TRACE
		unittest_printf("stop thread end\n");
#endif

		m_running = false;
	}

	void task_thread::run()
	{
		LOG_FUNCTION();
		//////////////////////////////////////////////////////////////////////////
		//初始化com
#ifdef _WIN32
		CoInitializeEx(0,2/*appartment*/);
#endif
		while(1)
		{
			m_delayed_task->run();

			task_item task;
			task.id = task_item::TASK_UNKNOWN;
			{//获取任务
				thread_scoped_lock_ptr lock( get_task_thread_factory()->create_scoped_lock_ptr( m_task_queue_mutex.get()));
				if ( m_task_queue->empty() )
				{
					//等待任务
					timespec t ={ CONDATION_WAIT_TIMEOUT_SECONDS , CONDATION_WAIT_TIMEOUT_MILLSECONDS};
#ifdef THREAD_TRACE
					unittest_printf("wait cond begin\n");
#endif
					auto dwRet = m_cond->wait( lock.get(), &t );

#ifdef THREAD_TRACE
					unittest_printf("wait cond end->%d\n", dwRet);
#endif
					//Sleep(1*1000);
				}else
				{
					//取出task
					task = m_task_queue->front();
					m_task_queue->pop();	
				}
			}
			//执行 task
			if( task.id == task_item::TASK_QUIT )
			{
#ifdef THREAD_TRACE
				unittest_printf("TASK_QUIT\n");
#endif

				LOG_INFO("request thread: got task _quit");
				break;//while true
			}
			//执行
			if( task.id == task_item::TASK_USER )
			{
				static int count = 0;
				LOG_DEBUG("#%d: request thread: got user task, run begin", ++count);
				task.run(this);
			}else
			{
				//time out
			}
		}//while(1)

#ifdef _WIN32
		CoUninitialize();
#endif
	}

	void task_thread::add_task ( task_item task )
	{
		static int count = 0 ;
		LOG_DEBUG("count=%d, task id = ", ++ count , task.id );
		auto pdll = get_task_thread_factory();
		assert(pdll);
		thread_scoped_lock_ptr lock( pdll->create_scoped_lock_ptr(m_task_queue_mutex.get()));
		m_task_queue->push( task );
		m_cond->notify();
	}

	int task_thread::add_task( std::function<void( )> func )
	{
		task_item ti;
		ti.id = task_item::TASK_USER;
		ti.run = [func](void* obj)
		{
			func();
		};
		add_task( ti );
		return 0;
	}

	int task_thread::add_task2( std::function<void(void*)> func , void* args2)
	{
		task_item ti;
		ti.id = task_item::TASK_USER;
		ti.run = [func,args2](void*)
		{
			func(args2);
		};
		add_task( ti );
		return 0;
	}

	int task_thread::add_task3( std::function<void(void*,void*)> func , void* args2, void*arg3)
	{
		task_item ti;
		ti.id = task_item::TASK_USER;
		ti.run = [func,args2,arg3](void*)
		{
			func(args2,arg3);
		};
		add_task( ti );
		return 0;
	}

	int task_thread::add_task4( std::function<void(void*,void*,void*)> func , void* args2, void*args3, void*args4)
	{
		task_item ti;
		ti.id = task_item::TASK_USER;
		ti.run = [func,args2,args3,args4](void*)
		{
			func(args2,args3,args4);
		};
		add_task( ti );
		return 0;
	}

	int task_thread::add_task5( std::function<void(void*,void*,void*,void*)> func , void* args2, void*args3, void*args4, void*args5)
	{
		task_item ti;
		ti.id = task_item::TASK_USER;
		ti.run = [func,args2,args3,args4,args5](void*)
		{
			func(args2,args3,args4,args5);
		};
		add_task( ti );
		return 0;
	}

	void task_thread::add_timer( const std::string& task_id, int first_delay_millseconds,  int delay_millseconds, std::function<void()> func )
	{
		thread_scoped_lock_ptr lock( get_task_thread_factory()->create_scoped_lock_ptr(m_task_queue_mutex.get()));
		m_delayed_task->put_task( task_id, first_delay_millseconds, delay_millseconds, func);
		m_cond->notify();
	}

	void task_thread::delete_timer( const std::string& task_id )
	{
		thread_scoped_lock_ptr lock( get_task_thread_factory()->create_scoped_lock_ptr(m_task_queue_mutex.get()));
		m_delayed_task->remove_task(task_id);
		m_cond->notify();
	}

	void task_thread::job_yield()
	{

	}


	void delayed_task::remove_task( const std::string& task_id )
	{
		auto it = m_tasks.find(task_id);
		if( it == m_tasks.end())
			return;
		//设置删除标志
		it->second->m_deleted = true;
	}

	void delayed_task::put_task( const std::string& task_id, int first_delay_millseconds, int delay_millseconds, std::function<void()> func )
	{
		task_item_ptr i;
		auto it = m_tasks.find(task_id);
		if( it == m_tasks.end() )
		{
			i.reset( new task_item() );
			m_tasks[task_id] = i;
		}
		else
			i = it->second;
		i->m_id = task_id;
		i->m_task = func;
		i->m_first_delay_millseconds = first_delay_millseconds;
		i->m_delayed_millseconds = delay_millseconds;
		i->m_deleted = false;
		i->m_running = false;
		i->m_run_count = 0;

		//计算下次运行时间
		time_t current;
		time(&current);
		i->m_next_run_time = current + first_delay_millseconds/1000 + 1;

		//auto it = m_tasks.find(task_id);
		//if( it == m_tasks.end() )
		//	m_tasks[task_id] = i;
		//else
		//	i = it->second;

		LOG_DEBUG( "task=%s, first=%dms, delay=%dms, m_tasks count=%d", 
			task_id.c_str(),
			first_delay_millseconds,
			delay_millseconds,
			m_tasks.size());
	}

	void delayed_task::run()
	{
		std::vector<std::string> ready_tasks;

		get_ready_task(ready_tasks);

		std::for_each( ready_tasks.begin(), ready_tasks.end(), [this](std::string& it){
			{
				task_item_ptr ptask;
				//////////////////////////////////////////////////////////////////////////
				{
					//设置成正在运行
					thread_scoped_lock_ptr lock( get_task_thread_factory()->create_scoped_lock_ptr(m_task_queue_mutex.get()));
					auto t = m_tasks.find(it);
					if( t == m_tasks.end() )
						return;
					if( t->second->m_deleted )
						return;
					t->second->m_running = true;
					ptask = t->second;
				}
				if( ! ptask )
					return;

				//////////////////////////////////////////////////////////////////////////
				LOG_DEBUG("task run begin: %s", ptask->m_id.c_str());
				auto b_ret = ptask->run(); 
				LOG_DEBUG("task run end  : %s", ptask->m_id.c_str());

				//////////////////////////////////////////////////////////////////////////
				{
					//设置成不在运行
					thread_scoped_lock_ptr lock( get_task_thread_factory()->create_scoped_lock_ptr(m_task_queue_mutex.get()));
					auto t = m_tasks.find(it);
					if( t == m_tasks.end() )
						return;
					if( t->second->m_deleted )
						return;
					t->second->m_running = false;
					if( b_ret == false )
						t->second->m_deleted = true;
				}
			}
		});

		remove_task();
	}

	void delayed_task::remove_task()
	{
		thread_scoped_lock_ptr lock( get_task_thread_factory()->create_scoped_lock_ptr(m_task_queue_mutex.get()));
		//删除deleted task
		std::vector<std::string> ended_tasks;
		for(auto it= m_tasks.begin(); it != m_tasks.end(); it++)
		{
			if( it->second->m_deleted && 
				it->second->m_running == false)
				ended_tasks.push_back( it->first );
		}
		std::for_each( ended_tasks.begin(), ended_tasks.end(), [this](std::string& it)
		{
			LOG_DEBUG(" remove task: %s", it.c_str());
			m_tasks.erase(it);
		});
	}

	void delayed_task::get_ready_task( std::vector<std::string>& task_ids )
	{
		{
			//收集需要运行的task
			//计算时间
			time_t current;
			time(&current);

			auto pdll = get_task_thread_factory();
			assert(pdll);
			assert(m_task_queue_mutex);
			if (!pdll || !m_task_queue_mutex)
				return;
			thread_scoped_lock_ptr lock( pdll->create_scoped_lock_ptr(m_task_queue_mutex.get()));
			for( auto it = m_tasks.begin(); it != m_tasks.end(); it++)
			{
				if( it->second->m_deleted == false && current >= it->second->m_next_run_time )////时间到了？
				{
					task_ids.push_back( it->first );
				}
			}
		}
	}


	bool delayed_task::task_item::run()
	{
		//计算时间
		time_t current;
		time(&current);

		//时间到了？
		if( current >= m_next_run_time )
		{

			//计算下次运行时间
			set_next_run_time();
			size_t next_saved = (size_t)m_next_run_time;
			
			//执行任务
			api_InterlockedIncrement(&m_run_count);
			m_task();

			//m_task()内部，是否修改了自己？
			if( next_saved == m_next_run_time ||
				m_run_count > 0)//没有修改 timer
			{
				if( m_delayed_millseconds == 0 )//一次性timer
					return false;//让外面删除自己
				//////////////////////////////////////////////////////////////////////////
				//从现在开始，重新计算时间
				set_next_run_time();
			}else
			{
				//timer被外部修改
			}
		}
		return true;
	}

	void delayed_task::task_item::set_next_run_time()
	{
		//计算时间
		time_t current;
		time(&current);

		//计算下次运行时间
		__int64 delay = (__int64) m_delayed_millseconds;
		if( delay <= 0 )
		{
			delay = current + 1 + 1ll*365*24*3600*1000;//1年
		}
		m_next_run_time = time(&current) + delay/1000;
	}

}//namespace thread

void unittest_printf(const char*fmt, ...)
{
#ifdef _WIN32
	va_list arg = (va_list)((char*)(&fmt) + sizeof(const char*)); 

	//time
	SYSTEMTIME st;
	GetLocalTime(&st);

	//多线程调用 PRINTF,会崩溃
	PRINTF("tid=%05d:%02d:%02d:%02d.%03d ", 
		GetCurrentThreadId(),
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	vprintf(fmt, arg);
#else
	//init log folder
	time_t timep;
	time(&timep);
	struct tm *p;
	p = localtime(&timep);

	fprintf(stderr, "tid=%05p:%02d:%02d:%02d ", 
		api::GetCurrentThreadId(),
		p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec
		);

	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

#endif//
}


#if THREAD_UNITTEST == 1
void thread::task_thread::unit_test()
{
	fprintf(stderr, "thread test : create\n");

	thread::ithread* th = get_task_thread_factory()->create_thread();//创建线程
	api_Sleep(300);
	class TestRun
	{
	public:
		TestRun(bool autodelete=true):m_autodelete(autodelete)
		{

		}
		thread::ithread* th;
		int m_i;
		bool m_autodelete;
		std::string m_id;
		void run()
		{
			unittest_printf("run i=%d\n", m_i);
			if( m_autodelete )
				delete this;
		}
		void run_loop()
		{
			static int c=0;c++;
			unittest_printf("run loop i=%d %d\n", m_i,c);

			th->add_timer( m_id, 1000, 0, std::bind(&TestRun::run_loop, this));

			if( m_autodelete )
				delete this;
		}
		void run2(void* args)
		{
			unittest_printf("run i=%d, args2=%p\n", m_i, args);
			if( m_autodelete )
				delete this;
		}
	};

	TestRun* tr = new TestRun();
	tr->m_i = 1;

	th->add_task( std::bind(&TestRun::run, tr) ); //添加 task1

	tr = new TestRun();
	tr->m_i = 2;
	th->add_task2( std::bind(&TestRun::run2, tr, std::placeholders::_1), (void*)(INT_PTR)0x123 ); //添加 task2

	//////////////////////////////////////////////////////////////////////////
	{
		int i = 8899;
		TestRun obj_tr;
		obj_tr.m_i = 7788;
		obj_tr.m_id = "run-once";

		std::function<void()> func = [i, obj_tr]()
		{
			unittest_printf("i=%d, tr.i=%d\n",i, obj_tr.m_i);
		};
		th->add_task( func );
	}

	//////////////////////////////////////////////////////////////////////////
	//timer
	if(1){
	
	int i1=0;
	for(int i=0;i<3;i++)
	{
		char id[128];sprintf_s(id, sizeof(id),"timer-%d",i);
		std::string s_id(id);
		i1=i;
		th->add_timer( id, 1000, 0, [s_id](){
			unittest_printf("timer-%s\n", s_id.c_str());
		});
	}
	}
	
	if(1){
	for(int i=0;i<3;i++)
	{
		tr = new TestRun(false/*auto delete*/);
		tr->th = th;
		tr->m_i = 2;
		char id[128];sprintf_s(id, sizeof(id), "self-timer-%d",i);
		tr->m_id = id;
		th->add_timer( id, 1000, 0, std::bind(&TestRun::run_loop, tr));
	}
	}
	//////////////////////////////////////////////////////////////////////////
	//timer
	tr = new TestRun(false/*auto delete*/);
	tr->m_i = 5;
	th->add_timer("timer_5_seconds", 
		1*1000, //第一次延时1秒执行
		5*1000, //以后每5秒执行一次
		std::bind(&TestRun::run, tr)
		); //添加 timer
	//th->delete_timer("timer_5_seconds");//删除timer

	api_Sleep(12*1000);

	fprintf(stderr, "thread test : stop\n");
	th->stop(); //停止线程

	fprintf(stderr, "thread test : destroy\n");
	get_task_thread_factory()->destroy_thread(th);//销毁线程

	fprintf(stderr, "thread test end\n");
}



#endif//THREAD_UNITTEST

#ifdef _WIN32
#else//linux

static std::string g_command_line(" ");

const char* api::GetCommandLineA()
{
	char* ret = " ";
	if (g_command_line.size() != 0)
		return &g_command_line[0];

	return ret;
}

const char * api::SetCommandLineA(const char * cmd)
{
	g_command_line.clear();
	if (cmd)
		g_command_line = cmd;
	if (g_command_line.size() == 0)
		g_command_line = " ";
	return &g_command_line[0];
}

const char * api::SetCommandLineA(int argc, char * argv[])
{
	g_command_line.clear();
	for (int i = 0; i < argc; i++)
	{
		g_command_line += " ";
		g_command_line += argv[i];
	}
	if (g_command_line.size() == 0)
		g_command_line = " ";
	PRINTF("set command line to : %s\n ", g_command_line.c_str());
	return &g_command_line[0];
}

LONG api::InterlockedIncrement(LONG volatile *lpAddend)
{
#ifdef _WIN32
	LONG ret = ::InterlockedIncrement(lpAddend);
#else
	LONG ret = apr_atomic_inc32((apr_uint32_t*)lpAddend);
#endif
	return ret;
}

LONG api::InterlockedDecrement(LONG volatile *lpAddend)
{
#ifdef _WIN32
	LONG ret = ::InterlockedDecrement(lpAddend);
#else
	LONG ret = (LONG)apr_atomic_dec32((apr_uint32_t*)lpAddend);
#endif
	return ret;
}

HANDLE api::CreateMutex(MY_LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName)
{
#ifdef _WIN32
	HANDLE mutex = ::CreateMutex(lpMutexAttributes, bInitialOwner, lpName);
	return mutex;
#else
	apr_global_mutex_t * mutex = 0;
	apr_status_t result = apr_global_mutex_create(
		&mutex,
		lpName,
		APR_LOCK_POSIXSEM,
		//APR_LOCK_PROC_PTHREAD ,
		//APR_LOCK_SYSVSEM ,
		//APR_LOCK_FLOCK,
		//APR_LOCK_DEFAULT,
		g_pool);
	if (APR_SUCCESS != result)
	{
		char err[2048];
		apr_strerror(result, err, sizeof(err));
		PRINTF("create mutex failed:%d %s\n", result, err);
		switch (result)
		{
		case APR_EBADMASK:
			PRINTF("APR_EBADMASK");
			break;
		}
		return NULL;
	}
	return new APR_HANDLE(mutex);
#endif
}

DWORD api::WaitForSingleObject(HANDLE h, DWORD dwMilliseconds)
{
#ifdef _WIN32
	return ::WaitForSingleObject(h, dwMilliseconds);
#else
	if (h == 0)
		return WAIT_FAILED;
	if (INVALID_HANDLE_VALUE == h)
		return WAIT_FAILED;

	APR_HANDLE *apr_h = (APR_HANDLE*)h;
	if (apr_h->type != 1)//mutex
		return WAIT_FAILED;
	auto m = apr_h->getMutex(h);
	apr_status_t result = apr_global_mutex_lock(m);
	if(APR_SUCCESS == result)
		return WAIT_OBJECT_0;
	return WAIT_FAILED;
#endif
}

DWORD api::WaitForThread(HANDLE h, DWORD dwMilliseconds)
{
#ifdef _WIN32
	return ::WaitForSingleObject(h, dwMilliseconds);
#else
	if (h == 0)
		return WAIT_FAILED;
	if (INVALID_HANDLE_VALUE == h)
		return WAIT_FAILED;
	void* ret = 0;
	auto i = pthread_join((pthread_t)h, &ret);
	assert(i == 0);
	if(i)
		return WAIT_FAILED;
	return 0;
#endif
}

BOOL api::ReleaseMutex(HANDLE h)
{
#ifdef _WIN32
	return ::ReleaseMutex(h);
#else	
	if (h == 0)
		return FALSE;
	if (INVALID_HANDLE_VALUE == h)
		return FALSE;

	APR_HANDLE *apr_h = (APR_HANDLE*)h;
	if (apr_h->type != 1)//mutex
		return FALSE;
	auto m = apr_h->getMutex(h);
	apr_status_t result = apr_global_mutex_unlock(m);
	if (APR_SUCCESS == result)
		return TRUE;
	return FALSE;
#endif
}

BOOL api::CloseHandle(HANDLE h)
{
#ifdef _WIN32
	BOOL result = ::CloseHandle(h);
	return result;
#else
	if (h == 0)
		return FALSE;
	if (INVALID_HANDLE_VALUE == h)
		return FALSE;

	APR_HANDLE *apr_h = (APR_HANDLE*)h;
	BOOL bret = apr_h->CloseHandle_();
	delete apr_h;
	return bret;
#endif
}

VOID api::Sleep(DWORD millSeconds)
{
#ifdef _WIN32
	::Sleep(millSeconds);
#else
	usleep(millSeconds * 1000);
#endif//
}

DWORD api::GetTickCount()
{
#ifdef _WIN32
	DWORD dure = ::GetTickCount();
#else
	//get the current number of microseconds since january 1st 1970
	DWORD dure = (DWORD)time(NULL);
#endif
	return dure;
}

DWORD api::GetCurrentProcessId()
{
	return getpid();
}

HMODULE api::LoadLibraryA(LPCSTR lpLibFileName)
{
	if (!lpLibFileName)
		return NULL;
	char c_folder[MAX_PATH];
	char c_lib[MAX_PATH];
	auto p_c_folder  = &c_folder[0];
	auto p_c_lib = &c_lib[0];

	strcpy(c_folder,lpLibFileName);
	//replace '\\' with '/'
	{
		auto p = &c_folder[0];
		while(*p)
		{
			if( *p == '\\' )
				*p = '/';
			p++;
		}
	}
	PathRemoveFileSpecA(c_folder);
	strcpy(c_lib, PathFindFileNameA(lpLibFileName));

	char* full = NULL;
	//当前目录?
	if (strlen(c_folder) == 0)
		strcpy(c_folder, ".");
	//删除lib后面的 .dll
	{
		int len = strlen(c_lib);
		if (len > 4)
		{
			char* p = c_lib + len - 4;
			if (strcmp(p, ".dll") == 0)
				*p = 0;
		}
	}
	//如果目录后面有 '/',那么删除掉'/',否则 full会多一个 '/'
	int len = strlen(c_folder);
	if (len)
	{
		if (c_folder[len - 1] == PATH_SPLIT_CHAR)
			c_folder[len - 1] = 0;
	}
	//全路径
	full = get_lib_name(c_folder, c_lib);

	//加载dll
	auto l = load_library(full);
	if( ! l )
	{
		PRINTF("load lib failed: %d\n %s\n %s\n", errno, full, dlerror());
	}

	free_lib_name(full);//释放内存
	return l;
}

HMODULE api::GetModuleHandle(LPCTSTR lpModuleName)
{
#ifdef _WIN32
	return ::GetModuleHandleA(lpModuleName);
#else//mac
	//todo: mac
	return NULL;
#endif
}

FARPROC api::GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	return (FARPROC)find_symbol(hModule, lpProcName);
}

DWORD api::GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize)
{
#ifdef _WIN32
	return ::GetModuleFileNameA(hModule, lpFilename, nSize);
#else
	if (lpFilename && nSize)
		*lpFilename = 0;
	char path[_max_path] = { 0 };
	if (!get_exe_fullpath(path))
		return 0;
	if (lpFilename && nSize)
		strcpy(lpFilename, path);
	return lpFilename ? strlen(lpFilename) : 0;
#endif
}



LPSTR api::PathAppendA(LPSTR dest, LPCSTR append)
{
	if (!dest)
		return (LPSTR)append;
	strcat(dest, "\\");
	strcat(dest, append);
	return dest;
}

LPCSTR api::PathFindFileNameA(LPCSTR pszPath)
{
	if (!pszPath)
		return NULL;
	//找到最后一个 '/'
	auto plast = strrchr(pszPath, '/');
	if (plast)
		return plast + 1;
	return pszPath;
}


BOOL api::PathRemoveFileSpecA(LPSTR pszPath)
{
#ifdef _WIN32
#else
	if (!pszPath)
		return FALSE;
	//找到最后一个 '/'
	auto plast = strrchr(pszPath, '/');
	if (plast)
		*plast = 0;
#endif
	return TRUE;
}

LPSTR api::PathCombineA(LPSTR pszDest, LPCSTR pszDir, LPCSTR pszFile)
{
	char ret[MAX_PATH] = { 0 };
	if (!pszDest)
		goto quit;

	if (pszDir)
		strcpy(ret, pszDir);
	if (pszFile)
	{
		auto len = strlen(ret);
		if (len > 0 && ret[len - 1] != '/')
		{
			ret[len] = '/';
			ret[len + 1] = 0;
		}
		strcat(ret, pszFile);
	}
	if (pszDest)
		strcpy(pszDest, ret);
quit:
	return pszDest;
}

BOOL api::PathFileExistsA(LPCSTR pszPath)
{
	return access(pszPath, R_OK) == 0;
}

BOOL api::CreateDirectoryA(LPCSTR lpPathName, MY_LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	DWORD dwError = 0;
	if (APR_SUCCESS == apr_dir_make(lpPathName, APR_OS_DEFAULT, g_pool))
		dwError = ERROR_SUCCESS;
	else {
		dwError = api_GetLastError();
	}
	return (0 == dwError);
}

char * api::get_lib_name(const char * path, const char * libname)
{
	uint32_t len = (nullptr == path) ? (strlen(libname) + 1) : (strlen(path) + strlen(PATHSEP) + strlen(libname) + 1);
	if (nullptr == strstr(libname, DLLSUFFIX))
	{
		len += strlen(DLLPREFIX);
		len += strlen(DLLSUFFIX);
		char* fullname = new char[len];

		if (path)
			sprintf(fullname, "%s%s%s%s%s", path, PATHSEP, DLLPREFIX, libname, DLLSUFFIX);
		else
			sprintf(fullname, "%s%s%s", DLLPREFIX, libname, DLLSUFFIX);
		return fullname;
	}
	else
	{
		char* fullname = new char[len];
		if (path)
			sprintf(fullname, "%s%s%s", path, PATHSEP, libname);
		else
			sprintf(fullname, "%s", libname);
		return fullname;
	}
}

void api::free_lib_name(char *& fullname)
{
	if (fullname)
	{
		delete[]fullname;
		fullname = nullptr;
	}
}

HMODULE api::load_library(const char * fullname)
{
#ifdef _WIN32
	return ::LoadLibraryA(fullname);
#else
	auto ld = RTLD_LAZY;
	ld = RTLD_NOW;
	return (HMODULE)dlopen(fullname, ld);
#endif
}

void api::free_library(HMODULE lib)
{
	if (!lib)
		return;
#ifdef XP_WIN
	::FreeLibrary((HMODULE)lib);
#else
	dlclose((void*)lib);
#endif	
}

void * api::find_symbol(HMODULE lib, const char * symbol)
{
#ifdef XP_WIN
	return (void*)::GetProcAddress((HMODULE)lib, symbol);
#else
	return dlsym((void*)lib, symbol);
#endif	
}


bool api::get_exe_fullpath(char * exeName)
{
	return api_get_exe_fullpath_(exeName);
}
BOOL api::FileTimeToSystemTime(CONST FILETIME * lpFileTime, LPSYSTEMTIME lpSystemTime)
{
#ifdef _WIN32
	BOOL ret = ::FileTimeToSystemTime(lpFileTime, lpSystemTime);
	return ret;
#else
	__int64  tempTime = lpFileTime->dwHighDateTime << 32 + lpFileTime->dwLowDateTime;
	tempTime = (tempTime - 116444736000000000LL) / 10000000;
	struct tm* pTm = gmtime((time_t*)&tempTime);
	if (pTm != NULL)
	{
		lpSystemTime->wSecond = pTm->tm_sec;
		lpSystemTime->wMinute = pTm->tm_min;
		lpSystemTime->wHour = pTm->tm_hour;
		lpSystemTime->wDay = pTm->tm_mday;
		lpSystemTime->wMonth = pTm->tm_mon + 1;
		lpSystemTime->wYear = pTm->tm_year + 1900;
		lpSystemTime->wDayOfWeek = pTm->tm_wday;
		free(pTm);
	}
	return pTm != NULL ? TRUE : FALSE;
#endif
}


HANDLE api::_beginthreadex(
	void *security,
	unsigned stack_size,
#ifdef _WIN32
	unsigned(*start_address)(void *),
#else
	void* (*start_address)(void *),
#endif
	void *arglist,
	unsigned initflag,
	unsigned *thrdaddr
)
{
#ifdef _WIN32
	return (HANDLE)_beginthreadex(security, stack_size,
		(unsigned int(__stdcall*)(void*))
		start_address,
		arglist, initflag, thrdaddr);
#else
	pthread_t      threadId = 0;//(pthread_t*)thrdaddr;
	int             rc = 0;

	//pthread_attr_t  attr = { 0 };
	//if (rc = pthread_attr_init(&attr))
	//	return RC_THREAD_NOT_CREATED;  // EINVAL, ENOMEM
	typedef void* (*pthread_func_t)(void*);
	pthread_func_t func = (pthread_func_t)start_address;
	if (rc = pthread_create( &threadId, 0, func,arglist))
		return RC_THREAD_NOT_CREATED;      // EINVAL, EAGAIN

	return (HANDLE)(threadId);
#endif
}

unsigned long api::GetCurrentThreadId()
{
#ifdef _WIN32
	return (int)::GetCurrentThreadId();
#else//mac
	pthread_t t = pthread_self();
	int size = sizeof(int);
	if (size > sizeof(pthread_t))
		size = sizeof(pthread_t);
	int ret;
	memcpy(&ret, &t, size);
	return ret;
#endif//mac	return 0;
}

void api::ZeroMemory(void *buf, unsigned int size)
{
	memset(buf, 0, size);
}

VOID  api::SetLastError(DWORD dwErrCode)
{
#ifdef _WIN32
	::SetLastError(dwErrCode);
#else
	errno = dwErrCode;
#endif
}

//apr_handle
APR_HANDLE::APR_HANDLE(apr_file_t *f)
	:type(0), pointer(f)
{
}

APR_HANDLE::APR_HANDLE(apr_global_mutex_t *m)
	: type(1), pointer(m)
{
}

BOOL APR_HANDLE::CloseHandle_()
{
	BOOL bret = FALSE;
	switch (type)
	{
	case 0://file
		bret = CloseFile();
		break;
	case 1://mutex
		bret = CloseMutex();
		break;
	}

	return bret;
}

apr_file_t *APR_HANDLE::getFile(HANDLE h)
{
	if (INVALID_HANDLE_VALUE == h)
		return NULL;
	return (apr_file_t*)((APR_HANDLE*)h)->pointer;
}

apr_global_mutex_t* APR_HANDLE::getMutex(HANDLE h)
{
	if (NULL == h)
		return NULL;
	return (apr_global_mutex_t *)((APR_HANDLE*)h)->pointer;
}

BOOL APR_HANDLE::CloseFile()
{
	HANDLE  h = (HANDLE)pointer;
	//
	DWORD	dwError = ERROR_SUCCESS;
	if (INVALID_HANDLE_VALUE == h)
		return TRUE;
	apr_status_t	result = apr_file_close((apr_file_t*)h);
	h = INVALID_HANDLE_VALUE;
	if (APR_SUCCESS != result)
	{
		dwError = APR_TO_OS_ERROR(result);
		api_SetLastError(dwError);
		PRINTF("apr file close failed:%d", dwError);
		return FALSE;
	}

	return TRUE;
}

BOOL APR_HANDLE::CloseMutex()
{
	apr_status_t result = apr_global_mutex_destroy(
		(apr_global_mutex_t*)pointer);
	pointer = NULL;
	if (APR_SUCCESS != result)
	{
		DWORD dwError = APR_TO_OS_ERROR(result);
		api_SetLastError(dwError);
		PRINTF("apr mutex destroy failed:%d", dwError);
		return FALSE;
	}
	return TRUE;
}
//apr_handle end
#endif////linux

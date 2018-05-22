#include "../include/string/string_define.h"
//#include "../include/signalrcpp/hub_ace/hub_ace_service.h"
#include "../include/signalrcpp/hub_initor/hub_initor_impl.h"

#include <assert.h>

#ifdef _WIN32
#include <windows.h>
#include <shlwapi.h>
#endif

#include "../include/dll_loader/dll_loader.h"

//#include "../indicator-config/header/indicator-config.h"

//#include "../indicator-log/header/indicator-log.h"
//#include "../indicator-ace/header/indicator-ace.h"

//////////////////////////////////////////////////////////////////////////
#include "../include/thread/task_thread.h"
#include "../include/thread/thread_event_impl.h"
#include "../include/thread/thread_mutex_impl.h"

#include "../include/signalrcpp/hub.h"

//////////////////////////////////////////////////////////////////////////
namespace signalrcpp_hub_com{
	signalrcpp::hub* hub_create(signalrcpp::hub_args* pargs=0);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
namespace signalrcpp_hub{
class hub_ace_initor{
public:
	hub_ace_initor(signalrcpp::hub_args* pargs);
};

typedef std::shared_ptr<hub_ace_initor> hub_ace_init_ptr;
//////////////////////////////////////////////////////////////////////////
class task_factory_impl : public signalrcpp::hub_factory
{
public:

	void        destroy_thread(::thread::ithread* p)
	{
		delete ((thread::task_thread*)p);
	}

	::thread::ithread_ptr create_thread_ptr()
	{
		::thread::ithread_ptr p(new ::thread::task_thread());
		p->start();
		return p;
	}


	::thread::thread_event_ptr create_event_ptr()
	{
		return ::thread::thread_event_ptr(new ::thread::thread_event_impl());
	}
	::thread::thread_mutex_ptr create_mutex_ptr()
	{
		return ::thread::thread_mutex_ptr(new ::thread::thread_mutex_impl());
	}

	::thread::thread_scoped_lock_ptr create_scoped_lock_ptr(::thread::thread_mutex* m)
	{
		return ::thread::thread_scoped_lock_ptr(new ::thread::thread_scoped_lock_impl(m));
	}

	signalrcpp::hub_initor_ptr create_initor()
	{
		return std::make_shared<signalrcpp_hub_initor::hub_initor_impl>();
	}
	::thread::ithread* create_thread()
	{
		auto ret = new thread::task_thread();
		ret->start();
		return ret;
	}
	//////////////////////////////////////////////////////////////////////////
	virtual signalrcpp::hub_ptr create_hub(signalrcpp::hub_args* pargs)
	{
		assert(false);
		return nullptr;
	}
};

class hub_factory_impl : public task_factory_impl
{
public:
	virtual signalrcpp::hub_ptr create_hub(signalrcpp::hub_args* pargs);
};
}//namespace signalrcpp_hub

//////////////////////////////////////////////////////////////////////////
//get_dll
extern "C"
{
	DLLEXPORT signalrcpp::hub_factory* get_dll();
};

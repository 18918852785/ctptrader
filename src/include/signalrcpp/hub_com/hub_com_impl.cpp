#ifdef _WIN32
#include <windows.h>
#include <Shlwapi.h>
#endif

#include <assert.h>

#include "../hub_response.h"
#include "../hub_initor/hub_initor_impl.h"
#include "hub_com_service.h"
#include "hub_com_request_impl.h"
#include "hub_com_response_impl.h"
#include "../../string/string_map.h"
#include "../../logger/print.h"
#include "../../thread/thread_cond.h"

#include "../../dll_loader/dll_factory.h"

#include "hub_com_impl.h"

#ifdef _WIN32

#define _ATL_ATTRIBUTES
#define _ATL_APARTMENT_THREADED
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <atlbase.h>
#include <atlcom.h>
using namespace ATL;
typedef LONG HRESULT;

//#include <atlstr.h>
#include <comutil.h>

#include "../util.h"
using namespace cosmos;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#define POST_LOCK()    ::thread::thread_scoped_lock_ptr locker( g_hub.get()->create_scoped_lock_ptr(m_post_mutex.get()));

#define MSG_IMPL(a_)   ((hub_com_request_impl*)a_.get())
#define MSG_PTR        MSG_IMPL(m)
#define POST_MSG_PTR   MSG_IMPL(m_post_msgs[seq])


//////////////////////////////////////////////////////////////////////////
static dll::dll_hub_factory g_hub;

namespace signalrcpp_hub_com {
	void message_loop()
	{
		hub_com_service::message_loop(true);
	}
}

namespace signalrcpp_hub_com{
	bool hub_com_service::m_ctrl_c_pressed = false;


[event_receiver( com )]
class CEventSink
{
public: 
	hub_com_service * m_wrapper;
	CEventSink(hub_com_service *w):m_wrapper(w)
	{

	}

	HRESULT on_status_changed(
		long    event_id,
		VARIANT parameter1,
		VARIANT parameter2,
		VARIANT parameter3,
		VARIANT parameter4,
		long*   outRet
		)
	{
		auto s1 = signalrcpp::util::variant2utfstring(parameter1);
		auto s2 = signalrcpp::util::variant2utfstring(parameter2);
		auto data = signalrcpp::util::variant2bytearray(parameter3);
		auto s4 = signalrcpp::util::variant2utfstring(parameter4);
		
		//LOG_DEBUG("on_status_changed->id=%d, parameter1=[%s],parameter2=[%s],parameter3 size=%d\n",
		//	event_id, 
		//	s1.c_str(),
		//	s2.c_str(),
		//	data.size()
		//	);

		//////////////////////////////////////////////////////////////////////////
		auto len = data.size();

		signalrcpp::hub_response_ptr args( new hub_com_response_impl() );
		auto pargs = (hub_com_response_impl*)args.get();
		pargs->event_id = event_id;
		pargs->classs_name_ = s1;
		pargs->object_name_ = s2;
		hub_com_response_impl::set_data( pargs->data_, data );
		hub_com_response_impl::set_data( pargs->raw_,  s4 );
		pargs->set_pointer();

		if( m_wrapper )
		{
			m_wrapper->on_callback(args);
		}
		return S_OK;
	}
	void Advise(IUnknown* pObj)
	{
		__hook(&signalrcpp::_ISrAppEvents::on_status_changed, (::IUnknown*)pObj, &CEventSink::on_status_changed);        
	}
	void UnAdvise(IUnknown* pObj)
	{
		__unhook(&signalrcpp::_ISrAppEvents::on_status_changed, (::IUnknown*)pObj, &CEventSink::on_status_changed);
	}
};

hub_com_service::hub_com_service(void)
	:m_app_cookie(0)
	,m_use_ctrl_handler(false)
	,m_post_seq(0)
	,m_app_thread_id(0)
	,m_git(0)
{
	static dll::dll_hub_factory g_hub;
	m_post_mutex = g_hub.get()->create_mutex_ptr();
	m_cls_id = *(APP_GUID*) &signalrcpp::CLSID_SrApp;
	m_iid    = *(APP_GUID*) &signalrcpp::IID_ISrApp;
}


hub_com_service::~hub_com_service(void)
{
	hub_stop();
	if(m_git)
	{
		((IGlobalInterfaceTable*)m_git)->Release();
		m_git = 0;
	}
}

LONG hub_com_service::run_server(const char* parameter, bool use_ctrl_handler)
{
	//////////////////////////////////////////////////////////////////////////
	//create
	CComPtr<signalrcpp::ISrApp> spObj;
	IID& cls_id = (IID&)m_cls_id;
	HRESULT hr = spObj.CoCreateInstance( cls_id );
	if( hr != S_OK )
		return hr;
	CEventSink sink(this);
	sink.Advise(spObj.p);

	//////////////////////////////////////////////////////////////////////////
	CComBSTR strRet;
	{
		CComPtr<IDispatch> pDisp;
		CComBSTR name(L"Server");
		hr = spObj->create( name, &pDisp);
		if( hr != S_OK )
			return hr;
	}
	CComBSTR bParam(parameter);
	hr = spObj->start( bParam, &strRet );
	if( hr != S_OK )
		return hr;

	//////////////////////////////////////////////////////////////////////////
	hr = spObj->get_version(L"", &strRet);
	if( hr != S_OK )
		return hr;
	PRINTF("server get_version->%s\n", signalrcpp::util::bstr2utfstring(strRet).c_str());

	//////////////////////////////////////////////////////////////////////////
	hub_com_service::message_loop(use_ctrl_handler);

	//////////////////////////////////////////////////////////////////////////
	spObj->stop(L"", &strRet);

	//////////////////////////////////////////////////////////////////////////
	//destroy
	sink.UnAdvise(spObj.p);

	return hr;
}


int hub_com_service::CtrlHandler( unsigned long fdwCtrlType )
{
	switch( fdwCtrlType )
	{
		// Handle the CTRL-C signal.
	case CTRL_C_EVENT:
		PRINTF( "Ctrl-C event\n\n" );
		m_ctrl_c_pressed = true;
		Beep( 750, 300 );
		return( TRUE );

		// CTRL-CLOSE: confirm that the user wants to exit.
	case CTRL_CLOSE_EVENT:
		m_ctrl_c_pressed = false;

		Beep( 600, 200 );

		PRINTF( "Ctrl-Close event\n\n" );
		return( TRUE );

		// Pass other signals to the next handler.
	case CTRL_BREAK_EVENT:
		Beep( 900, 200 );
		PRINTF( "Ctrl-Break event\n\n" );
		return FALSE;

	case CTRL_LOGOFF_EVENT:
		Beep( 1000, 200 );
		PRINTF( "Ctrl-Logoff event\n\n" );
		return FALSE;

	case CTRL_SHUTDOWN_EVENT:
		Beep( 750, 500 );
		PRINTF( "Ctrl-Shutdown event\n\n" );
		return FALSE;

	default:
		return FALSE;
	}
}

void hub_com_service::message_loop(bool use_ctrl_handler)
{
	//启动一个timer,让 GetMessage 返回
	auto timer_id = SetTimer(0, 1, 1000, 0);
	if( use_ctrl_handler )
	{
		if( SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE ) )
		{
			PRINTF( "\nThe Control Handler is installed.\n" );
			PRINTF( "\n -- Now try pressing Ctrl+C or Ctrl+Break, or" );
			PRINTF( "\n try logging off or closing the console...\n" );
			PRINTF( "\n(...waiting in a loop for events...)\n\n" );

		}
		else
		{
			PRINTF( "\nERROR: Could not set control handler");
			return;
		}
	}

	PRINTF("windows message loop begin...\n");
	BOOL bRet = FALSE;
	MSG msg;
	while( (bRet = GetMessage( &msg, 0, 0, 0 )) != 0) 
	{
		if (bRet == -1) 
		{ // handle the error and possibly exit
			break;
		} else 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if( m_ctrl_c_pressed )
			break;
	}

	KillTimer(0, timer_id);
}

long hub_com_service::hub_start(const char* filter_class, const char* filter_object, const char*param1, unsigned long *pcookie)
{
	m_app_thread_id = GetCurrentThreadId();//记录创建app com对象的线程号

	m_class = filter_class ? filter_class: "";
	m_object = filter_object?filter_object:"";

	m_app.reset( new CComPtr_App() );

	IID& cls_id = (IID&)m_cls_id;

	HRESULT hr = m_app->CoCreateInstance(cls_id);
	if( hr != S_OK )
		return hr;
	m_event.reset( new CEventSink(this));
	m_event->Advise( m_app->p );
	
	//////////////////////////////////////////////////////////////////////////
	{
		CComPtr<IDispatch> pDisp;
		CComBSTR name(L"Hub");
		CComBSTR strRet;
		auto hr = m_app->p->create( name, &pDisp);
		if( hr != S_OK )
			return hr;
	}
	//////////////////////////////////////////////////////////////////////////
	{
		CComBSTR parameter,strRet;
		hr = m_app->p->get_version(parameter, &strRet);
		if (hr != S_OK)
			return hr;
		PRINTF("hub get_version->%s\n", signalrcpp::util::bstr2utfstring(strRet).c_str());
	}

	//////////////////////////////////////////////////////////////////////////
	//filter
	{
		CComBSTR s_class( m_class.c_str() );
		CComBSTR s_object(m_object.c_str());
		CComBSTR s_ret;
		hr = m_app->p->add_filter( s_class, s_object, &s_ret );
		if( hr != S_OK )
			return hr;
		PRINTF("add filter[%s:%s] done->%s\n", m_class.c_str(), m_object.c_str(), signalrcpp::util::bstr2utfstring(s_ret).c_str());
	}
	//////////////////////////////////////////////////////////////////////////
	std::string param;
	if( param1 )
		param = param1;
	//////////////////////////////////////////////////////////////////////////
	//把当前进程的id，传递给 signalrcpp 进程
	{
		char s_pid[128] = {0};
		sprintf_s(s_pid, sizeof(s_pid)-1, " /client_processid:%d ", ::GetCurrentProcessId());
		param += s_pid;
	}
	//////////////////////////////////////////////////////////////////////////
	{
		CComBSTR bParam(param.c_str());
		CComBSTR strRet;
		hr = m_app->p->start(bParam, &strRet);
		if (hr != S_OK)
			return hr;
	}
	//////////////////////////////////////////////////////////////////////////
	//cookie
	unsigned long cookie = 0;
	hr = register_app(&cookie);
	if( hr ) return -1;//fail
	if( pcookie ) *pcookie = cookie;
	m_app_cookie = cookie;
	return hr;
}


LONG hub_com_service::run_hub(
	const char* filter_class, const char* filter_object, const char*param,
	std::function<void(unsigned long cookie)> before_loop,
	unsigned long *pcookie,
	bool           use_ctrl_handler)
{
	m_class = filter_class;
	m_object = filter_object;

	//////////////////////////////////////////////////////////////////////////
	//create
	unsigned long cookie = 0;
	auto hr = hub_start(filter_class, filter_object, param, &cookie);
	if( hr )
		return hr;
	if(pcookie) *pcookie = cookie;

	//////////////////////////////////////////////////////////////////////////
	//
	if( before_loop )
		before_loop(cookie);

	//////////////////////////////////////////////////////////////////////////
	hub_com_service::message_loop(use_ctrl_handler);

	hr = hub_stop();
	return hr;
}

long hub_com_service::hub_stop()
{
	if( ! m_event )
		return 0;
	//////////////////////////////////////////////////////////////////////////
	CComBSTR strRet;
	m_app->p->stop(L"", &strRet);

	//////////////////////////////////////////////////////////////////////////
	//destroy
	m_event->UnAdvise( m_app->p );

	m_event = nullptr;
	m_app = nullptr;

	return 0;

}

long hub_com_service::hub_broadcast( void* data, int data_len )
{
	if( !m_app)
		return -1;
	CComBSTR s_c( m_class.c_str()), s_o(m_object.c_str());

	CComVariant m;
	auto hr = signalrcpp::util::bytearray2variant(data, data_len, m);
	if( hr ) return hr;

	CComVariant s_ret;
	hr = m_app->p->send_message( s_c, s_o, m, &s_ret);
	return hr;
}


long hub_com_service::hub_post( signalrcpp::hub_request_ptr msg)
{
	if( !m_app)
		return -1;
	std::string c, o;
	if( msg->app_class )
	{
		c = msg->app_class   ? msg->app_class   : "";
		o = msg->app_command ? msg->app_command : "";
	}else
	{
		c = m_class.c_str();
		o = m_object.c_str();
	}
	if( c.find("121=") != -1
		&& c.find(",") != -1
		&& c.find("999=") != -1)//已经是 121=,122=,123=,999=App格式了
	{
		//999=App格式
	}else//拼装 header 
	{

		char post_to_class[2048]={0};
		sprintf_s(post_to_class, sizeof(post_to_class),
			"121=%s,122=%s,123=send,124=%ld",
			c.c_str(),
			o.c_str(),
			m_post_seq);
		c = post_to_class;
		o.clear();
	}

	CComVariant m;
	auto hr = signalrcpp::util::bytearray2variant(msg->data, msg->data_len, m);
	if( hr ) return hr;

	CComVariant s_ret;
	CComBSTR s_c(c.c_str());
	CComBSTR s_o(o.c_str());
	
	//////////////////////////////////////////////////////////////////////////
	msg->from_thread = ! is_same_thread();
	if( msg->from_thread )
	{
		CComPtr<signalrcpp::ISrApp> pApp;
		auto hr = get_dispatch_from_cookie(m_app_cookie, (void**)&pApp);
		if( hr || ! pApp ) return hr;//fail
		hr = pApp->send_message(s_c, s_o, m, &s_ret);
	}else
		hr = m_app->p->send_message( s_c, s_o, m, &s_ret);
	return hr;
}

long hub_com_service::hub_broadcast_from_thread( void* data, int data_len )
{
	CComPtr<signalrcpp::ISrApp> pApp;
	auto hr = get_dispatch_from_cookie(m_app_cookie, (void**)&pApp);
	if( hr || ! pApp ) return hr;//fail
	
	CComBSTR s_c( m_class.c_str()), s_o(m_object.c_str());

	CComVariant m;
	hr = signalrcpp::util::bytearray2variant(data, data_len, m);
	if( hr ) return hr;

	CComVariant s_ret;
	hr = pApp->send_message( s_c, s_o, m, &s_ret);
	return hr;
}

long hub_com_service::get_dispatch( void** ppDisp )
{
	if( !m_app )
		return E_FAIL;
	return m_app->p->QueryInterface(IID_IDispatch, ppDisp);
}

long hub_com_service::register_app(DWORD* pCookie)
{
	if(pCookie)
		*pCookie = 0;
	if( ! m_app )
		return E_FAIL;


	init_git();
	if( ! m_git )
		return E_FAIL;

	signalrcpp::ISrApp* pfac = 0;
	IID& iid = (IID&)m_iid;
	auto hr = m_app->p->QueryInterface( iid, (void**)&pfac);
	if( hr != S_OK )
	{
		//LOG_FUNC(module_id, xp_log_error, "QueryInterface(IID_IApp) failed, hr=" << hr);
		return hr;
	}else
	{
		unsigned long c = 0;
		IID& iid = (IID&)m_iid;
		hr = ((IGlobalInterfaceTable*)m_git)->RegisterInterfaceInGlobal(pfac, iid, &c);
		if( hr != S_OK )
		{
			//LOG_FUNC(module_id, xp_log_error, "RegisterInterfaceInGlobal(IID_IApp) failed, hr=" << hr);
			return hr;
		}
		if( pCookie )
			*pCookie = c;
	}
	return hr;

}

long hub_com_service::get_dispatch_from_cookie( unsigned long cookie, void** ppDisp )
{
	if( ppDisp )
		*ppDisp = 0;
	if( ! cookie || ! m_app )
		return E_FAIL;
	init_git();
	if( ! m_git )
		return E_FAIL;
	CComPtr<signalrcpp::ISrApp> pThis;
	IID& iid = (IID&)m_iid;
	auto hr = ((IGlobalInterfaceTable*)m_git)->GetInterfaceFromGlobal( cookie, iid, ppDisp );
	if( hr != S_OK  )
	{
		//LOG_FUNC(module_id,xp_log_error,"GetInterfaceFromGlobal failed,hr="<<hr);
		return hr;
	}

	return hr;
}

long hub_com_service::hub_send( signalrcpp::hub_request_ptr m , unsigned long timeout)
{
	//////////////////////////////////////////////////////////////////////////
	//创建数据返回信号量
	MSG_PTR->data_arrived_event = g_hub.get()->create_event_ptr();

	{
		POST_LOCK();
		InterlockedIncrement(&m_post_seq);
		MSG_PTR->client_seq = m_post_seq;
		m_post_msgs[MSG_PTR->client_seq] = m;//请求保存起来
	}
	auto hr = hub_post(m);
	if( hr != S_OK )
	{
		MSG_PTR->data_arrived_event = nullptr;//释放事件
		return hr;//failed
	}
	{
		//等待服务返回
		auto ret = MSG_PTR->data_arrived_event->wait(timeout);
	
		MSG_PTR->data_arrived_event = nullptr;//释放事件

		if( ret != WAIT_OBJECT_0 )
			return ret;//failed

	}
	return 0;
}

void hub_com_service::on_callback( signalrcpp::hub_response_ptr pargs )
{
	//通知别人数据来了
	if(pargs)
	{
		//取得 client_seq
		auto fields = string_map_create(pargs->classs_name->ptr);
		/*
		//////////////////////////////////////////////////////////////////////////
		//客户端请求 app
		COMMAND_APP_CALL_CLASS = 121,
		COMMAND_APP_CALL_COMMAND = 122,
		COMMAND_APP_CALL_ACTION = 123,
		COMMAND_APP_CALL_CLIENT_SEQ = 124,
		COMMAND_APP_CALL_DATA = 500,
		COMMAND_APP_CALL_RESULT = 500,

		COMMAND_APP_CALL_SERVER_RESULT = 131,
		COMMAND_APP_CALL_SERVER_SEQ = 132,
		*/
		auto v121 = fields->get(121);
		auto v122 = fields->get(122);
		auto v123 = fields->get(123);
		auto v124 = fields->get(124);
		auto v131 = fields->get(131);
		if( v121 && v122 && v123 && v124 && v131 )
		{
			long long server_result = _atoi64(v131->c_str());
			long long seq = _atoi64(v124->c_str());
			if( server_result == 2 //处理完成
				|| server_result  ==1 //app not found
				)
			{
				//查找client seq
				POST_LOCK();
				if( m_post_msgs.count(seq) > 0 )
				{
					//发出app处理完成事件
					auto event = POST_MSG_PTR->data_arrived_event;
					if( event )
					{
						POST_MSG_PTR->response = pargs;
						event->set_event();
					}
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	if( m_callback )
	{
		m_callback( pargs );
	}

}

bool hub_com_service::is_same_thread()
{
	return (m_app_thread_id == GetCurrentThreadId());
}

long hub_com_service::init_git()
{
	if( ! m_git)
	{
		auto hr = signalrcpp::util::git_create( (IGlobalInterfaceTable**)&m_git);
		if( hr ) return hr;//fail
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////
//signalrcpp_hub

hub_com_impl::hub_com_impl()
	:m_started(0),initor(0)
{
	initor = new signalrcpp_hub_initor::hub_initor_impl();
}

hub_com_impl::hub_com_impl(signalrcpp::hub_args* pargs)
	:m_started(0),initor(0)
{
#ifdef USE_HUB
	initor = new signalrcpp_hub_initor::hub_initor_impl();
	if(pargs)
		start(pargs);
#endif
}


hub_com_impl::~hub_com_impl()
{
	stop();

#ifdef USE_HUB
	delete (signalrcpp_hub_initor::hub_initor_impl*)initor;
#endif
	initor = 0;
}


void hub_com_impl::message_loop()
{
#ifdef USE_HUB
	hub_com_service::message_loop(true);
#endif
}

long hub_com_impl::start(signalrcpp::hub_args*pargs)
{
#ifdef USE_HUB
	if( m_started )
		return 0;
	m_started = true;
	unsigned long cookie = 0;
	auto hr = hub_app.hub_start( pargs->app_class, pargs->app_command, pargs->parameter, &cookie);
	//LOG_INFO("hub start %s = 0x%08X\n", hr?"FAILED":"OK", hr);
	return hr;
#else
	return 0;
#endif
}

long hub_com_impl::stop()
{
	if( ! m_started )
		return 0;
	m_started = false;
#ifdef USE_HUB
	hub_app.hub_stop();
#endif
	return 0;
}


long hub_com_impl::send_filter(const char* class_1, const char* app_1)
{
	assert(false);
	return -1;
}

void hub_com_response_impl::set_data( memory_buffer& b, void* data, int data_len )
{
	b.resize(data_len);
	if( data_len > 0 )
	{
		auto p = &b[0];
		memcpy(p, data, data_len);
	}
}

void hub_com_response_impl::set_data( memory_buffer& b, std::vector<char>& d )
{
	b.resize(d.size());
	if(d.size()>0)
		set_data(b, &d[0], (int)d.size());
}

void hub_com_response_impl::set_data( memory_buffer& b, std::string& d )
{
	b.resize(d.size());
	if(d.size()>0)
		set_data(b, &d[0], (int)d.size());
}


}

#endif//win32


//////////////////////////////////////////////////////////////////////////
namespace signalrcpp_hub_com {

	signalrcpp::hub_ptr hub_create_ptr(signalrcpp::hub_args* pargs)
	{
#ifdef _WIN32
		auto ret = std::make_shared<hub_com_impl>(pargs);
		return ret;
#else
		return signalrcpp::hub_ptr();
#endif
	}

	signalrcpp::hub* hub_create(signalrcpp::hub_args* pargs)
	{
#ifdef _WIN32
		auto ret = new hub_com_impl(pargs);
		return ret;
#else
		return 0;
#endif
	}

	void hub_com_response_impl::set_pointer(signalrcpp::string_ptr& to, memory_buffer& from)
	{
		to = signalrcpp::struct_string_impl::create(from);
	}

}


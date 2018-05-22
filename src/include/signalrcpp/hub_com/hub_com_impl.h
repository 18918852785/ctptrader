#include <assert.h>

#include "hub_com_service.h"
#include "hub_com_request_impl.h"
#include "hub_com_response_impl.h"
#include "../../string/string_map.h"

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


#import "../../../lib/signalrcpp.tlb"  \
	auto_search no_implementation named_guids \
	raw_interfaces_only raw_native_types embedded_idl rename_namespace("signalrcpp") \
	rename("App",  "SrApp") \
	rename("IApp", "ISrApp")\
	rename("_IAppEvents", "_ISrAppEvents")
[module(name="EventReceiver")];
[emitidl(false)]; // don't need any of COM server files 
// generated (.idl, .h, *.c, .tlb)


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
namespace signalrcpp_hub_com{
	class hub_com_impl: public signalrcpp::hub
	{
		bool             m_started;
		void*            initor;//signalr_cpp::signalrcpp_init

		hub_com_service  hub_app;
	public:
		hub_com_impl ();
		hub_com_impl (signalrcpp::hub_args*pargs);
		virtual ~hub_com_impl();

		virtual long        start(signalrcpp::hub_args* pargs);
		virtual long        stop();
		virtual long        set_callback(signalrcpp::hub_callback callback){ hub_app.m_callback = callback; return 0;}

		virtual long        send_filter(const char* class_1, const char* app_1);

		virtual long        broadcast(void* msg, int msg_len){return hub_app.hub_broadcast_from_thread(msg,msg_len);}
		virtual long        post(signalrcpp::hub_request_ptr m){return hub_app.hub_post(m);}
		virtual long        send(signalrcpp::hub_request_ptr m, unsigned long timeout_millseconds){return hub_app.hub_send(m, timeout_millseconds);}

		virtual signalrcpp::hub_request_ptr create_request(){return std::make_shared<hub_com_request_impl>();}

		virtual void        message_loop();
	};

	//////////////////////////////////////////////////////////////////////////
	//CComPtr_App
	class CComPtr_App: public CComPtr<signalrcpp::ISrApp>
	{

	};

}

#endif
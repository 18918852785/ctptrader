#if _MSC_VER >= 1700 //vs2012
#include <thread>
#else//vs2010
#include "../../include/thread/ithread.h"
#endif

#include <conio.h>
#include "MdSpi.h"

CThostFtdcMdApi* g_pMdApi;
// ****** Simnow **************************************************
char strTraderFront[] = "tcp://180.168.146.187:10000";
char strMdFront[] = "tcp://180.168.146.187:10010";
TThostFtdcBrokerIDType BrokerID = "9999";
TThostFtdcInvestorIDType InvestorID = "068854";
TThostFtdcPasswordType  Password = "123456";
// ****************************************************************
char *ppInstrumentID[1024] = { 0 };
int iInstrumentID = 4;
int iRequestID = 0;

//////////////////////////////////////////////////////////////////////////

void main()
{

	//////////////////////////////////////////////////////////////////////////

	auto action = [&](){
		g_pMdApi = CThostFtdcMdApi::CreateFtdcMdApi();
		g_pMdApi->RegisterSpi(new CMdSpi());
		g_pMdApi->RegisterFront(strMdFront);
		g_pMdApi->Init();
		g_pMdApi->Join();
	};

#if _MSC_VER >= 1700 //vs2012
    std::thread mdThread = std::thread(action);

    if(mdThread.joinable())
        mdThread.join();
#else//vs2010

	action();
	
	//auto th = ::thread::create_thread_ptr();
	//th->add_task(action);
#endif

	//printf("Press any key to quit.\n");
	//getch();
	return;
}


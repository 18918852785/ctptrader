#if _MSC_VER >= 1700 //vs2012
#include <thread>
#else//vs2010
#include "../../include/thread/ithread.h"
#endif

#include <conio.h>
#include "MdSpi.h"
#include "zmq_publisher.h"

CThostFtdcMdApi* g_pMdApi;
// ****** Simnow **************************************************
char strTraderFront[] = "tcp://180.168.146.187:10000";
char strMdFront[] = "tcp://180.168.146.187:10010";
TThostFtdcBrokerIDType BrokerID = "9999";
TThostFtdcInvestorIDType InvestorID = "068854";
TThostFtdcPasswordType  Password = "123456";
// ****************************************************************
char *ppInstrumentID[] = {"M1809","M1901","M1905"};
int iInstrumentID = 3;
int iRequestID = 0;

//////////////////////////////////////////////////////////////////////////
zmq_publisher g_pub;

void main()
{
	g_pub.init();
	iInstrumentID = g_pub.m_code_count;
	memcpy(&ppInstrumentID[0], &g_pub.m_codes[0], iInstrumentID*sizeof(char*));

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


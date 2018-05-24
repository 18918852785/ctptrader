
#include <conio.h>
#include "MdSpi.h"

CThostFtdcMdApi* g_pMdApi = nullptr;

// ****** Simnow **************************************************
//char strTraderFront[] = "tcp://180.168.146.187:10000";
char strMdFront[]       = "tcp://180.168.146.187:10010";
TThostFtdcBrokerIDType BrokerID = "9999";
TThostFtdcInvestorIDType InvestorID = "068854";
TThostFtdcPasswordType  Password = "123456";
// ****************************************************************
char *ppInstrumentID[] = {"m1809","m1901","m1905"}; //要获取的期货行情
int iInstrumentID = 3; //3个期货品种
int iRequestID = 0;

//////////////////////////////////////////////////////////////////////////
void main()
{
	g_pMdApi = CThostFtdcMdApi::CreateFtdcMdApi();
	g_pMdApi->RegisterSpi(new CMdSpi());
	g_pMdApi->RegisterFront(strMdFront);
	g_pMdApi->Init();
	g_pMdApi->Join();
}

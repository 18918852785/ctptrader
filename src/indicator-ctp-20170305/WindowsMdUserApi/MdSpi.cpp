#include "MdSpi.h"
#include <iostream>

//#include "../../indicator/zmq/zmq_handler.h"

using namespace std;

#pragma warning(disable : 4996)
extern CThostFtdcMdApi* g_pMdApi;
extern char strMdFront[];
extern TThostFtdcBrokerIDType BrokerID;
extern TThostFtdcInvestorIDType InvestorID;
extern TThostFtdcPasswordType Password;
extern char* ppInstrumentID[]; 
extern int iInstrumentID;
extern int iRequestID;

void CMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cerr << "--->>> "<< "OnRspError" << endl;
}

void CMdSpi::OnFrontDisconnected(int nReason)
{
    cerr << "--->>> " << "OnFrontDisconnected" << endl;
    cerr << "--->>> Reason = " << nReason << endl;
}

void CMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
    cerr << "--->>> " << "OnHeartBeatWarning" << endl;
    cerr << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

void CMdSpi::OnFrontConnected()
{
    cerr << "--->>> " << "OnFrontConnected" << endl;
    ReqUserLogin();
}

void CMdSpi::ReqUserLogin()
{
    CThostFtdcReqUserLoginField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, BrokerID);
    strcpy(req.UserID, InvestorID);
    strcpy(req.Password, Password);
    int iResult = g_pMdApi->ReqUserLogin(&req, ++iRequestID);
    cerr << "--->>> 发送用户登录请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cerr << "--->>> " << "OnRspUserLogin" << endl;
    if (bIsLast && pRspInfo->ErrorID == 0)
    {
        cerr << "--->>> 获取当前交易日 = " << g_pMdApi->GetTradingDay() << endl;
        SubscribeMarketData();
    }
}

void CMdSpi::SubscribeMarketData()
{
    int iResult = g_pMdApi->SubscribeMarketData(ppInstrumentID, iInstrumentID);
    cerr << "--->>> 发送行情订阅请求: " << ppInstrumentID[0] << " " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cerr << "OnRspSubMarketData:"<< pSpecificInstrument->InstrumentID <<", " << (bIsLast?" End":"") << endl;
}

void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cerr << "OnRspUnSubMarketData" << endl;
}

void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	//printf("ExchangeID=%9s,InstrumentID=%10s, UpdateTime=%s.%03d, LastPrice=%8.2f, Volume=%6d\n",
	char msg[1024];
	sprintf(msg,"%s,%s,9003,%s.%03d,%.1f,%d\n",
		"ctp",//pDepthMarketData->ExchangeID,
		pDepthMarketData->InstrumentID,
        pDepthMarketData->UpdateTime,
        pDepthMarketData->UpdateMillisec,
        pDepthMarketData->LastPrice,
        pDepthMarketData->Volume);
	cerr << msg;

//	zmq_handler::init_pub();
//	zmq_handler::publish_data(msg, strlen(msg));
}


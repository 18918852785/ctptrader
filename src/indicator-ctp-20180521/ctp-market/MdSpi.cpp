#include "StdAfx.h"

//#include "../../indicator/zmq/zmq_handler.h"
#include "../ctp-log/ctp-log.h"

using namespace std;


void CMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CTP_LOG_INFO( "--->>> "<< "OnRspError" << endl);
}

void CMdSpi::OnFrontDisconnected(int nReason)
{
    CTP_LOG_INFO("--->>> " << "OnFrontDisconnected" << endl
    <<"--->>> Reason = " << nReason << endl);
}

void CMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
    CTP_LOG_INFO("--->>> " << "OnHeartBeatWarning" << endl
    <<"--->>> nTimerLapse = " << nTimeLapse << endl);
}

void CMdSpi::OnFrontConnected()
{
    CTP_LOG_INFO("--->>> " << "OnFrontConnected" << endl);
    ReqUserLogin();
}

void CMdSpi::ReqUserLogin()
{
    CThostFtdcReqUserLoginField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, g->g_brokerId.c_str());
    strcpy(req.UserID, g->g_investorId.c_str());
    strcpy(req.Password, g->g_passWord.c_str());

    g->IncRequestID();
    int iResult = g->g_pMdApi->ReqUserLogin(&req, g->g_iRequestID.i);
    
    CTP_LOG_INFO("--->>> 发送用户登录请求: " << ((iResult == 0) ? "成功" : "失败") << endl);
}

void CMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    std::ostringstream msg;
    msg<<"--->>> " << "OnRspUserLogin" << endl;
    if (bIsLast && pRspInfo->ErrorID == 0)
    {
        msg<<"--->>> 获取当前交易日 = " << g->g_pMdApi->GetTradingDay() << endl;
        SubscribeMarketData();
    }
    CTP_LOG_INFO(msg.str());
}

void CMdSpi::SubscribeMarketData()
{
    int iResult = g->g_pMdApi->SubscribeMarketData(g->g_instrumentId.data, g->g_instrumentId.count);
    
    std::string msg;
    for(int i=0;i<g->g_instrumentId.count;i++){
        msg += g->g_instrumentId.data[i];
        msg+=",";
    }
    CTP_LOG_INFO("--->>> 发送行情订阅请求: " << msg.c_str() << " " << ((iResult == 0) ? "成功" : "失败") << endl);
}

void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CTP_LOG_INFO("OnRspSubMarketData" << endl);
}

void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CTP_LOG_INFO("OnRspUnSubMarketData" << endl);
}

void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
    if( g->on_market_data != nullptr){
	    ctp_market_callback_data d;
        d.InstrumentID = pDepthMarketData->InstrumentID;
        d.UpdateTime = pDepthMarketData->UpdateTime;
        d.UpdateMillisec = pDepthMarketData->UpdateMillisec;
        d.BuyPrice1 = pDepthMarketData->BidPrice1;
        d.SellPrice1 = pDepthMarketData->AskPrice1;
        d.ctp_p = pDepthMarketData;
        g->on_market_data(&d);
    }
	//zmq_handler::init_pub();
	//zmq_handler::publish_data(msg, strlen(msg));
}


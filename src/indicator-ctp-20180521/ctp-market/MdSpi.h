#pragma once
#include "../CTP636Common/ThostFtdcMdApi.h"

#include "ctp-market-global.h"

class ctp_market_global;
typedef std::shared_ptr<ctp_market_global> ctp_market_global_ptr;

class CMdSpi : public CThostFtdcMdSpi
{
public:
    CMdSpi(ctp_market_global_ptr a):g(a){}

    virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    virtual void OnFrontDisconnected(int nReason);
    virtual void OnHeartBeatWarning(int nTimeLapse);
    virtual void OnFrontConnected();
    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

    ctp_market_global_ptr  g;
private:
    void ReqUserLogin();
    void SubscribeMarketData();
};
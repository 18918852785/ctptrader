#pragma once
#include "ctp-market-export.h"
#include "ctp-market-callback.h"
#include "ctp-int.h"

class CTPMARKET_API ctp_market_global{
public:
    ctp_market_global();
    ~ctp_market_global();
    int init();

    CThostFtdcMdApi* g_pMdApi;
    ::thread::ithread_ptr g_market_thread;

    //std::string g_strTraderFront;
    std::string g_strMdFront;
    std::string g_brokerId;
    std::string g_investorId;
    std::string g_passWord;

    ctp_codes g_instrumentId;
    std::vector<std::string> g_instrumentId_data;

    ctp_int g_iRequestID;

    ctp_market_callback on_market_data;

    void IncRequestID(){ g_iRequestID.i++;}
public:
	std::unordered_map<std::string, ctp_price> g_prices;

};
// ctp-market.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "ctp-trader.h"
#include "ctp-trader-global.h"
#include "TraderSpi.h"

#include "../../include/string/utf8_string.h"
#include "../ctp-log/ctp-log.h"

ctp_trader::ctp_trader()
{
    g = std::make_shared<ctp_trader_global>();
    g->init();
}

int ctp_trader::set_codes( int count, const char** codes )
{
    g->g_instrumentId_data.clear();
    CTP_PRINT("set_codes count=%d\n", count);
    for(int i=0;i<count;i++)
    {
        CTP_PRINT("code#%3d : %s\n", i, codes[i]);
        g->g_instrumentId_data.push_back(codes[i]);
    }

    g->g_instrumentId.init(g->g_instrumentId_data);
    return 0;
}

int ctp_trader::set_codes( const char* code )
{
    std::vector<std::string> dels;
    dels.push_back(";");
    dels.push_back(",");
    auto arr = string_split(code, dels);
    
    ctp_codes cc;
    cc.init(arr);
    return set_codes( cc.count, (const char**)cc.data);
}

int ctp_trader::start()
{
    auto action = [&](){
        g->g_pTraderApi  = CThostFtdcTraderApi::CreateFtdcTraderApi();
        g->g_pTraderSpi  = new CTraderSpi(g);

        g->g_pTraderApi->RegisterSpi(g->g_pTraderSpi);
        g->g_pTraderApi->SubscribePublicTopic(THOST_TERT_QUICK);
        g->g_pTraderApi->SubscribePrivateTopic(THOST_TERT_QUICK);
        g->g_pTraderApi->RegisterFront((char*)g->g_strTraderFront.c_str());
        g->g_pTraderApi->Init();
        g->g_pTraderApi->Join();

#if 0
        g_pTraderApi2->RegisterSpi(pUserSpi);
        g_pTraderApi2->SubscribePublicTopic(THOST_TERT_QUICK);
        g_pTraderApi2->SubscribePrivateTopic(THOST_TERT_QUICK);
        g_pTraderApi2->RegisterFront(g_strTraderFront);
        g_pTraderApi2->Init();
        g_pTraderApi2->Join();
#endif
    };

#if _MSC_VER >= 1700 //vs2012
    std::thread mdThread = std::thread(action);

    if(mdThread.joinable())
        mdThread.join();
#else//vs2010

    //action();
    g->g_trader_thread.reset(new ::thread::task_thread());
    g->g_trader_thread->start();
    g->g_trader_thread->add_task(action);
#endif
    return 0;
}


void ctp_trader::set_trader_callback( ctp_trader_callback c )
{
    g->on_trader_data = c;
}

int ctp_trader::ReqQryInstrument()
{
    g->g_pTraderSpi->ReqQryInstrument();
    return  0;
}

int ctp_trader::ReqOrderInsert(const ctp_ReqOrderInsert* r)
{
    return g->g_pTraderSpi->ReqOrderInsert(r);
}

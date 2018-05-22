// ctp-market.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "ctp-market.h"
#include "ctp-market-global.h"

#include "../../include/string/utf8_string.h"


ctp_market::ctp_market()
{
    g = std::make_shared<ctp_market_global>();
    g->init();
}


int ctp_market::set_codes( const char* code )
{
    std::vector<std::string> dels;
    dels.push_back(";");
    dels.push_back(",");
    auto arr = string_split(code, dels);

    ctp_codes cc;
    cc.init(arr);
    return set_codes( cc.count, (const char**)cc.data);
}

int ctp_market::set_codes( int count, const char** codes )
{
    g->g_instrumentId_data.clear();
	g->g_prices.clear();
    for(int i=0;i<count;i++)
    {
		g->g_instrumentId_data.push_back(codes[i]);
		g->g_prices[codes[i]]=ctp_price();
	}

    g->g_instrumentId.init(g->g_instrumentId_data);
    return 0;
}

int ctp_market::start()
{

    auto action = [&](){
        g->g_pMdApi = CThostFtdcMdApi::CreateFtdcMdApi();
        g->g_pMdApi->RegisterSpi(new CMdSpi(g));
        g->g_pMdApi->RegisterFront((char*)g->g_strMdFront.c_str());
        g->g_pMdApi->Init();
        g->g_pMdApi->Join();
    };

#if _MSC_VER >= 1700 //vs2012
    std::thread mdThread = std::thread(action);

    if(mdThread.joinable())
        mdThread.join();
#else//vs2010

    //action();
    g->g_market_thread.reset(new ::thread::task_thread());
    g->g_market_thread->start();
    g->g_market_thread->add_task(action);
#endif
    return 0;
}

void ctp_market::set_market_callback( ctp_market_callback c )
{
    g->on_market_data = c;
}

int ctp_market::set_price(const char* code, double buy, double sell)
{
	ctp_price p(buy,sell);
	g->g_prices[code]=p;
	return 0;
}

int ctp_market::get_price(const char* code, double& buy, double & sel)
{
	buy=sel=0;
	auto i = g->g_prices.find(code);
	if(i==g->g_prices.end())
		return -1;

	buy = i->second.buy;
	sel = i->second.sell;
	if(buy<2 || sel <2)
		return -1;
	return 0;
}

int ctp_market::get_spread(double & spread1, double &spread2)
{
	spread1 = spread2=0;
	double buy1, sell1, buy2, sell2;
	auto ret = get_price(g->g_instrumentId_data[0].c_str(),buy1,sell1);
	if(ret!=0) return ret;
	
	ret = get_price(g->g_instrumentId_data[1].c_str(),buy2,sell2);
	if(ret!=0) return ret;

	spread1 = -sell2+buy1;
	spread2 = -buy2 + sell1;
	return ret;
}

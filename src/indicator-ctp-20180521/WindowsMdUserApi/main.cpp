
#include "../ctp-market/ctp-market.h"
#include "../ctp-market/ctp-market-callback.h"

#include <conio.h>
#include <assert.h>
#include "zmq_publisher.h"
#include "../ctp-log/ctp-log.h"

//////////////////////////////////////////////////////////////////////////
zmq_publisher g_pub;

int main()
{
    CTP_LOG_INIT("md");
	g_pub.init();

    //////////////////////////////////////////////////////////////////////////
    CTP_PRINT("code count=%d\n", g_pub.m_code_count);
    for(int i=0;i<g_pub.m_code_count;i++){
        CTP_PRINT("%3d: %s\n", i, g_pub.m_codes[i]);
    }

    //////////////////////////////////////////////////////////////////////////
    ctp_market m;

    auto ret = m.set_codes(g_pub.m_code_count, (const char**)&g_pub.m_codes[0]);
    assert(ret==0);
    if(ret!=0)
        return ret;

    auto action = [&](ctp_market_callback_data* d){
		m.set_price(d->InstrumentID, d->BuyPrice1, d->SellPrice1);
		//计算价差
		double spread1,spread2;
		auto i = m.get_spread(spread1,spread2);
		if( i != 0) {
            //让后面打印代码继续执行
        }

		//CTP_PRINT("ExchangeID=%9s,InstrumentID=%10s, UpdateTime=%s.%03d, LastPrice=%8.2f, Volume=%6d\n",
        char msg[1024];
        sprintf(msg,"%s,%16s,9003,%s.%03d, %8d, %-5d, %-5d, %-5d, %-5d\n",
            "ctp",//pDepthMarketData->ExchangeID,
            d->InstrumentID,
            d->UpdateTime,
            d->UpdateMillisec,
            //pDepthMarketData->LastPrice,
            d->Volume,
            (int)d->BuyPrice1,//买
            (int)d->SellPrice1, //卖
			(int)spread1,
			(int)spread2
            );
        CTP_PRINT(msg);
    };
    m.set_market_callback( action);
    ret = m.start();
    assert(ret==0);
    if(ret!=0)
        return ret;
    //////////////////////////////////////////////////////////////////////////


	printf("Press any key to quit.\n");
	getch(); exit(0);
	return 0;
}


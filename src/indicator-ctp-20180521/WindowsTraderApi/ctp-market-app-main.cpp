#include "../ctp-trader/ctp-trader.h"
#include "../ctp-market/ctp-market.h"
#include "../ctp-market/ctp-int.h"
#include "../strategy/ctp-strategy-range2.h"

#include "../../include/thread/thread_event_impl.h"
#include "../../include/thread/ithread.h"
#include "../../include/string/utf8_string.h"

#include <conio.h>
#include <assert.h>
#include "../gtest/flags.h"

#include "../ctp-trader/ctp-trader-global.h"
#include "../ctp-log/ctp-log.h"

#include "../ctp-market/ctp-market-saver.h"

DECLARE_FLAG(charp, ctp_market_code);

namespace ctp_market_app{
//负责提供期货行情
class my_ctp_market {
public:
    my_ctp_market(){}

    ctp_market m;
    ctp_market_saver saver; //行情存盘

    std::function<void(ctp_market_callback_data* d)> on_spread_changed;
    int start(){

        auto ret = saver.start();//存盘
        if(ret)return ret;

        m.set_codes(FLAG_ctp_market_code);//设置要订阅的行情代码

        auto pthis =this;

        //设置接收期货行情的函数: 有新行情的时候, 这个函数会被调用
        m.set_market_callback([&,pthis](ctp_market_callback_data* d){

            //保存期货代码和买/卖价格
            m.set_price(d->InstrumentID, d->BuyPrice1, d->SellPrice1);
            //发出价格改变通知
            pthis->on_spread_changed(d);

            //////////////////////////////////////////////////////////////////////////
            //打印价格信息
            char msg[1024];
            sprintf(msg,"%s%s,%16s,9003,%s.%03d, %8d, %-5d, %-5d\n",
                "",//pthis->m_has_dot?"\n":"",
                "ctp",//pDepthMarketData->ExchangeID,
                d->InstrumentID,
                d->UpdateTime,
                d->UpdateMillisec,
                //pDepthMarketData->LastPrice,
                d->Volume,
                (int)d->BuyPrice1, //买
                (int)d->SellPrice1 //卖
                );
            CTP_PRINT(msg);

            saver.save(d->ctp_p);//保存价格
        });
        //启动ctp行情: 
        return m.start();
    }
};


int main(void)
{
    int ret = -1;
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    my_ctp_market m;//行情价格

    //设置行情改变时的回调函数
    m.on_spread_changed =[&](ctp_market_callback_data* d){
        //行情改变了
    };

    ret = m.start();//启动行情
    if(ret)return ret;

    //////////////////////////////////////////////////////////////////////////
    printf("Press any key to quit.\n");
    getch(); exit(0);


    return 0;
}

}//namespace main_ctp_market

#include "../ctp-trader/ctp-trader.h"
#include "../ctp-market/ctp-market.h"
#include "../ctp-market/ctp-market-mock.h"
#include "../ctp-market/ctp-int.h"
#include "../strategy/ctp-strategy-range2.h"
#include "../../include/thread/thread_event_impl.h"
#include "../../include/thread/ithread.h"

#include <conio.h>
#include <assert.h>
#include "../gtest/flags.h"

#include "../ctp-trader/ctp-trader-global.h"
#include "../ctp-log/ctp-log.h"

namespace ctp_spread_print{
class my_ctp_trader{
public:
    int order_spread2(ctp_order* o){
        assert(false);
        return 0;
    }

    int start(){
        return 0;
    }
};

class my_ctp_market {
public:
    my_ctp_market():m_has_dot(false){}
    ctp_market_mock m;
    ctp_int m_last_spread1, m_last_spread2;
    ctp_int m_spread1, m_spread2;
    bool m_has_dot;
    std::function<void(const char* instrumentID,int spread1,int spread2,const char* s_time)> on_spread_changed;
    int start(){
        auto pthis =this;
        m.set_market_callback([&,pthis](ctp_market_callback_data* d){
            m.set_price(d->InstrumentID, d->BuyPrice1, d->SellPrice1);
            //计算价差
            double spread1,spread2;
            char s_spread[128]={0};
            auto i = m.get_spread(spread1,spread2);
            if( i != 0) {
                //让后面打印代码继续执行
            }else{
                pthis->m_spread1.set((int)spread1);
                pthis->m_spread2.set((int)spread2);
                pthis->on_spread_changed(d->InstrumentID,spread1,spread2,d->UpdateTime);
            }
            if( pthis->m_spread1.equal(pthis->m_last_spread1) &&
                pthis->m_spread2.equal(pthis->m_last_spread2) 
                ){
                    //pthis->m_has_dot =true;
                    //printf(".");
                    return;
            }

            pthis->m_last_spread1 = pthis->m_spread1;
            pthis->m_last_spread2 = pthis->m_spread2;

            //CTP_PRINT("ExchangeID=%9s,InstrumentID=%10s, UpdateTime=%s.%03d, LastPrice=%8.2f, Volume=%6d\n",
            char msg[1024];
            sprintf(s_spread,"%-5d, %-5d", (int)spread1, (int)spread2);
            sprintf(msg,"%s%s,%16s,9003,%s.%03d, %8d, %-5d, %-5d, %s\n",
                pthis->m_has_dot?"\n":"",
                "ctp",//pDepthMarketData->ExchangeID,
                d->InstrumentID,
                d->UpdateTime,
                d->UpdateMillisec,
                //pDepthMarketData->LastPrice,
                d->Volume,
                (int)d->BuyPrice1,//买
                (int)d->SellPrice1, //卖
                s_spread
                );
            CTP_PRINT(msg);

            pthis->m_has_dot=false;

        });
        return m.start();
    }
};

static void test_trade_time()
{
    //上午9：00--11：30；其中10：15--10：30为交易所中间休息
    //下午13：30--15：00；其中14：10--14：20为上海期货交易所中间休息，大连 郑州照常交易

    //测试交易时间段判断
    assert( ctp_strategy::is_trade_time("20180419 08:55:03.080") == false );
    assert( ctp_strategy::is_trade_time("20180419 08:55:03"    ) == false );
    assert( ctp_strategy::is_trade_time(         "08:55:03.080") == false );
    assert( ctp_strategy::is_trade_time(         "08:55:03"    ) == false );

    assert( ctp_strategy::is_trade_time("20180419 08:59:03.080") == false );
    assert( ctp_strategy::is_trade_time("20180419 08:59:03"    ) == false );
    assert( ctp_strategy::is_trade_time(         "08:59:03.080") == false );
    assert( ctp_strategy::is_trade_time(         "08:59:03"    ) == false );

    assert( ctp_strategy::is_trade_time("20180419 09:55:03.080") == true );
    assert( ctp_strategy::is_trade_time(         "09:55:03.080") == true );
    assert( ctp_strategy::is_trade_time(         "09:55:03"    ) == true );

    assert( ctp_strategy::is_trade_time(         "08:55:03"    ) == false );
    assert( ctp_strategy::is_trade_time(         "09:00:00"    ) == false );
    assert( ctp_strategy::is_trade_time(         "09:00:01"    ) == false );
    assert( ctp_strategy::is_trade_time(         "09:00:02"    ) == false );
    assert( ctp_strategy::is_trade_time(         "09:00:03"    ) == true  );
    assert( ctp_strategy::is_trade_time(         "10:14:03"    ) == true  );
    assert( ctp_strategy::is_trade_time(         "10:14:57"    ) == true  );
    assert( ctp_strategy::is_trade_time(         "10:14:58"    ) == false );
    assert( ctp_strategy::is_trade_time(         "10:14:59"    ) == false );

    assert( ctp_strategy::is_trade_time(         "10:15:00"    ) == false );
    assert( ctp_strategy::is_trade_time(         "10:15:03"    ) == false );

    assert( ctp_strategy::is_trade_time(         "10:30:00"    ) == false );
    assert( ctp_strategy::is_trade_time(         "10:30:01"    ) == false );
    assert( ctp_strategy::is_trade_time(         "10:30:02"    ) == false );
    assert( ctp_strategy::is_trade_time(         "10:30:03"    ) == true );

    assert( ctp_strategy::is_trade_time(         "11:29:57"    ) == true );
    assert( ctp_strategy::is_trade_time(         "11:29:58"    ) == false );
    assert( ctp_strategy::is_trade_time(         "11:29:59"    ) == false );
    assert( ctp_strategy::is_trade_time(         "11:30:00"    ) == false );
    assert( ctp_strategy::is_trade_time(         "11:30:03"    ) == false );

    assert( ctp_strategy::is_trade_time(         "13:30:00"    ) == false );
    assert( ctp_strategy::is_trade_time(         "13:30:01"    ) == false );
    assert( ctp_strategy::is_trade_time(         "13:30:02"    ) == false );
    assert( ctp_strategy::is_trade_time(         "13:30:03"    ) == true );

    assert( ctp_strategy::is_trade_time(         "14:59:03"    ) == true );
    assert( ctp_strategy::is_trade_time(         "14:59:57"    ) == true );
    assert( ctp_strategy::is_trade_time(         "14:59:58"    ) == false );
    assert( ctp_strategy::is_trade_time(         "14:59:59"    ) == false );
    assert( ctp_strategy::is_trade_time(         "15:00:00"    ) == false );
    assert( ctp_strategy::is_trade_time(         "15:00:01"    ) == false );
    assert( ctp_strategy::is_trade_time(         "15:00:02"    ) == false );
    assert( ctp_strategy::is_trade_time(         "15:00:03"    ) == false );

    assert( ctp_strategy::is_trade_time(         "21:00:00"    ) == false );
    assert( ctp_strategy::is_trade_time(         "21:00:01"    ) == false );
    assert( ctp_strategy::is_trade_time(         "21:00:02"    ) == false );
    assert( ctp_strategy::is_trade_time(         "21:00:03"    ) == true );

    assert( ctp_strategy::is_trade_time(         "23:29:57"    ) == true  );
    assert( ctp_strategy::is_trade_time(         "23:29:58"    ) == false );
    assert( ctp_strategy::is_trade_time(         "23:29:59"    ) == false );
    assert( ctp_strategy::is_trade_time(         "23:30:00"    ) == false);
    assert( ctp_strategy::is_trade_time(         "23:30:01"    ) == false);
    assert( ctp_strategy::is_trade_time(         "23:30:02"    ) == false);
    assert( ctp_strategy::is_trade_time(         "23:30:03"    ) == false);

}

int main(void)
{
    //////////////////////////////////////////////////////////////////////////
    test_trade_time();

    //////////////////////////////////////////////////////////////////////////
    my_ctp_trader t;


    //////////////////////////////////////////////////////////////////////////
    //ctp_strategy_range2 sr;
    //sr.set_code("SP m1809&m1901");

    //sr.on_order = std::bind(&my_ctp_trader::order_spread2, &t, 
    //    std::placeholders::_1
    //    , std::placeholders::_2
    //    , std::placeholders::_3
    //    , std::placeholders::_4
    //    , std::placeholders::_5
    //    , std::placeholders::_6
    //    );

    //auto ret=sr.start();
    //if(ret)return ret;


    //////////////////////////////////////////////////////////////////////////
    //交易账户

    auto ret = t.start();
    if(ret)return ret;

    //////////////////////////////////////////////////////////////////////////
    //行情价格
    my_ctp_market m;

    //t.m_spread_sell = &m.m_spread1;
    //t.m_spread_buy = &m.m_spread2;
    m.on_spread_changed =[&](const char* instrumentID,int s1,int s2,const char* s_time){
        //sr.on_spread_change(nullptr,s1,s2,s_time);
    };

    ret = m.start();
    if(ret)return ret;

    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////
    CTP_PRINT("Press any key to quit.\n");
    getch(); exit(0);


    return 0;
}

}//namespace ctp_spread_print

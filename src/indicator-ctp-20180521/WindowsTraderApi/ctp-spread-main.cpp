#include "../ctp-trader/ctp-trader.h"
#include "../ctp-market/ctp-market.h"
#include "../ctp-market/ctp-int.h"
#include "../strategy/ctp-strategy-range2.h"
#include "../../include/thread/thread_event_impl.h"
#include "../../include/thread/ithread.h"
#include "../../include/string/utf8_string.h"
#include "../ctp-log/ctp-log.h"

#include <conio.h>
#include <assert.h>
#include "../gtest/flags.h"

#include "../ctp-trader/ctp-trader-global.h"

DEFINE_FLAG(charp, ctp_market_code, "m1809,m1901,m1805","行情的代码列表,逗号/分号分割");
DEFINE_FLAG(charp, ctp_trade_code,  "SP m1809&m1901",   "交易的代码列表,逗号/分号分割");
DEFINE_FLAG(charp, range, "-1000,1000,3,100",/*-22,-32,3,5;-25,-35,3,5*/ "价格范围: 高位/低位; 超价: 用 buy+3 买, sell-3 卖; 利润:平仓条件");

namespace ctp_spread{
class my_ctp_trader{
public:
    my_ctp_trader():m_spread_sell(0),m_spread_buy(0){}

    std::vector<ctp_strategy_range2_ptr> strat_ranges;

    ctp_trader t;
    ::thread::thread_event_ptr evt;
    ctp_int* m_spread_sell;
    ctp_int* m_spread_buy;

    int get_current_buy(){
        return m_spread_buy->i;
    }
    int get_current_sell(){
        return m_spread_sell->i;
    }
    int get_price_det(){
        return 3;
    }
    void print_menu(){
        printf("Select command:\n");
        printf("spread : %d             %d\n", m_spread_sell->i, m_spread_buy->i);
        printf("1. buy,  open  :price = %d\n", get_current_sell() + get_price_det());
        printf("2. buy,  close \n");
        printf("3. sell, open  :price = %d\n", get_current_buy() - get_price_det());
        printf("4. sell, close \n");
    }
    int order_spread(bool buy, bool open){
        //////////////////////////////////////////////////////////////////////////
        ///报单录入请求
        int price=0;
        int c_price=0;
        if(buy)
        {
            c_price = get_current_sell();
            price = get_current_sell()+get_price_det();
        }
        else
        {
            c_price = get_current_buy();
            price = get_current_buy() - get_price_det();
        }
        return order_spread2("SP m1809&m1901", buy, open, price, ctp_strategy::get_time().c_str());
    }
    int order_spread2(const char* code, bool buy, bool open, int price,const char*s_time){
        int ret = -1;
        //////////////////////////////////////////////////////////////////////////
        ///报单录入请求
        ctp_order o;

        o.InstrumentID = code;
        o.open = open;
        o.buy = buy;
        o.price = price;
        o.time = s_time;

        ret = order_spread2(&o);
        return ret;
    }

    int order_spread2(ctp_ReqOrderInsert* o){
        int ret = -1;

        if(! o->InstrumentID || strstr(o->InstrumentID,"&")==nullptr){
                assert(false);
                return -1;
        }
        //////////////////////////////////////////////////////////////////////////
        ///报单录入请求
        t.g->IncOrderRef();
        o->order_ref = t.g->g_orderRef;

        t.g->IncRequestID();
        o->request_id = t.g->g_iRequestID;

        ret = t.ReqOrderInsert(o);
        //t.evt->wait(-1);
        return ret;
    }
    
    int fire_on_notify(const ctp_trader_callback_data* d){
        for(int i=0;i< strat_ranges.size();i++){
            strat_ranges[i]->on_notify(d);
        }
        return 0;
    }

    int start(){

        evt.reset(new ::thread::thread_event_impl());

        t.set_trader_callback( [&](const ctp_trader_callback_data* d){
            CTP_PRINT("trader callback: event id=%d\n", d->event_id);
            switch(d->event_id){
            case event_OnRspSettlementInfoConfirm:
                /////请求查询合约
                t.ReqQryInstrument();
                break;
            case event_OnRspQryTradingAccount:
                ///报单录入请求
                //t.ReqOrderInsert();
                evt->set_event();
                break;
            case     event_OnRspOrderAction:
                evt->set_event();
                break;
            case     event_OnRspOrderInsert:
                evt->set_event();
                break;
            case     event_OnRtnOrder:
                evt->set_event();
                break;
            case     event_OnRtnTrade:
                evt->set_event();
                break;
            default:
                break;
            }

            fire_on_notify(d);
        });
        t.set_codes(FLAG_ctp_trade_code);

        auto ret = t.start();
        assert(ret==0);
        if(ret!=0)return ret;
        //////////////////////////////////////////////////////////////////////////
        //等待账单确认
        evt->wait(-1);
        CTP_PRINT("账单确认完毕\n");
        return 0;
    }
};

class my_ctp_market {
public:
    my_ctp_market():m_has_dot(false){}
    ctp_market m; //接收期货行情
    ctp_int m_last_spread1, m_last_spread2;
    ctp_int m_spread1, m_spread2;
    bool m_has_dot;

    std::function<void(const char* instrumentID, int spread1,int spread2,const char* s_time)> on_spread_changed;

    int start(){
        m.set_codes(FLAG_ctp_market_code);

        auto pthis =this;
        m.set_market_callback([&,pthis](ctp_market_callback_data* d){
            m.set_price(d->InstrumentID, d->BuyPrice1, d->SellPrice1);
            //计算价差
            double spread1 = -1000, spread2 = 1000;
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
                    pthis->m_has_dot =true;
                    printf(".");
                    return;
            }

            pthis->m_last_spread1 = pthis->m_spread1;
            pthis->m_last_spread2 = pthis->m_spread2;

            //CTP_PRINT("ExchangeID=%9s,InstrumentID=%10s, UpdateTime=%s.%03d, LastPrice=%8.2f, Volume=%6d\n",
            char msg[1024];
            sprintf(s_spread,"%-5d, %-5d", (int)spread1, (int)spread2);
            sprintf(msg,"%s%s,%16s,9003,%s.%03d, %8d, %-5d, %-5d, %s\n",
                "",//pthis->m_has_dot?"\n":"",
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


static int stategy_start( ctp_strategy_range2 &sr, my_ctp_trader& t)
{
    sr.set_market_codes(FLAG_ctp_market_code);
    sr.set_trade_code(FLAG_ctp_trade_code);

    sr.set_on_order ( std::bind(&my_ctp_trader::order_spread2, &t, 
        std::placeholders::_1
        ));

    auto ret=sr.start();
    if(ret)return ret;

    return ret;
}

static std::vector<ctp_range> get_ranges(){
    std::vector<ctp_range>  ret;
    auto s_ranges = string_split(FLAG_range,";");
    assert(s_ranges.size()>0);
    if(s_ranges.size()==0) {
        goto err;
    }
    for(int i=0;i<s_ranges.size();i++){
        auto s = s_ranges[i];
        auto fields = string_split(s,",");
        if(fields.size()!=4){
            goto err;
        }
        ctp_range r;
        r.m_range_high.set(atoi(fields[0].c_str()));
        r.m_range_low .set(atoi(fields[1].c_str()));
        r.m_price_det .set(atoi(fields[2].c_str()));
        r.m_profit    .set(atoi(fields[3].c_str()));

        auto b = r.m_range_high.i > r.m_range_low.i &&
            r.m_price_det.i >= 0 &&
            r.m_profit.i >0;
        assert(b);
        if(!b)goto err;

        ret.push_back(r);
    }

    return ret;
err:
    ret.clear();
    CTP_LOG_ERROR("parse range error: "<< FLAG_range<< std::endl);
    exit(-1);
    return ret;
}

int main(void)
{
    //////////////////////////////////////////////////////////////////////////
    my_ctp_trader t;

    //////////////////////////////////////////////////////////////////////////
    auto ranges = get_ranges();
    int ret=0;
    for(int i=0;i<ranges.size();i++){
        ctp_strategy_range2_ptr sr(new ctp_strategy_range2());

        char s_id[128]; sprintf(s_id,"rs%d", i+1);
        sr->set_range ( ranges[i] );
        sr->set_id(s_id);
        ret = stategy_start(*sr, t);
        if(ret)return ret;

        t.strat_ranges.push_back(sr);
    }


    //////////////////////////////////////////////////////////////////////////
    //交易账户
    ret = t.start();
    if(ret)return ret;

    //////////////////////////////////////////////////////////////////////////
    //行情价格
    my_ctp_market m;

    t.m_spread_sell = &m.m_spread1;
    t.m_spread_buy = &m.m_spread2;
    m.on_spread_changed =[&](const char* instrumentID,int spread_buy,int spread_sell,const char* spread_time){
        for(int i=0;i< t.strat_ranges.size();i++){
            t.strat_ranges[i]->on_spread_change(instrumentID,spread_buy,spread_sell,spread_time);
        }
    };

    ret = m.start();
    if(ret)return ret;

    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////
    printf("Press any key to order.\n");
    getch();
    //clear_screen();

    bool cont=true;
    while(cont){
        t.print_menu();
        auto key = getch();
        switch(key){
        case '0':
        case '\r':
        case '\n':
            //clear_screen();
            break;
        case '1':
            t.order_spread(true,true);
            break;
        case '2':
            t.order_spread(true,false);
            break;
        case '3':
            t.order_spread(false,true);
            break;
        case '4':
            t.order_spread(false,false);
            break;
        default:
            cont=false;
            break;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    printf("Press any key to quit.\n");
    getch(); exit(0);


    return 0;
}

}//namespace ctp_spread

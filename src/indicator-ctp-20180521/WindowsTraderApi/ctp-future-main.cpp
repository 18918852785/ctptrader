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

DECLARE_FLAG(charp, ctp_market_code);
DECLARE_FLAG(charp, ctp_trade_code);
DECLARE_FLAG(charp, range);

namespace ctp_future{

//负责下单的类    
class my_ctp_trader{
public:
    my_ctp_trader():m_spread_sell(0),m_spread_buy(0){}

    std::vector<ctp_strategy_range2_ptr> strat_ranges;//区间交易策略

    ctp_trader t;//ctp下单

    ::thread::thread_event_ptr evt; 
    //当前买卖价格
    ctp_int* m_spread_sell; 
    ctp_int* m_spread_buy;


    //交易所回报信息
    int fire_on_notify(const ctp_trader_callback_data* d){
        for(int i=0;i< strat_ranges.size();i++){
            strat_ranges[i]->on_notify(d);//交易所回报信息
        }
        return 0;
    }

    //启动
    int start(){

        evt.reset(new ::thread::thread_event_impl());

        t.set_trader_callback( [&](const ctp_trader_callback_data* d){
            CTP_PRINT("trader callback: event id=%d\n", d->event_id);
            switch(d->event_id){
            case event_OnRspSettlementInfoConfirm://交割单确认完毕
                t.ReqQryInstrument();//请求查询合约
                break;
            case event_OnRspQryTradingAccount://账号信息查询完毕
                ///报单录入请求
                //t.ReqOrderInsert();
                evt->set_event();
                break;
            case     event_OnRspOrderAction://撤单发生错误的通知：比如，发出撤单的瞬间，报单已经全部成交
                evt->set_event();
                break;
            case     event_OnRspOrderInsert://柜台校验出错返回: 资金不足
                evt->set_event();
                break;
            case     event_OnRtnOrder:///报单进入交易所后的通知
                evt->set_event();
                break;
            case     event_OnRtnTrade:///成交通知
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

    //执行下单命令
    int order_future(bool buy, bool open, bool over_price){
        int ret = -1;
        //////////////////////////////////////////////////////////////////////////
        ///报单录入请求
        auto code = get_code();
        int price=0;
        int c_price=0;
        if(buy)
        {
            c_price = get_current_sell();
            if(over_price)
                price = get_current_sell()+get_price_det();
            else
                price = get_current_buy()-get_price_det();
        }
        else
        {
            c_price = get_current_buy();
            if(over_price)
                price = get_current_buy() - get_price_det();
            else
                price = get_current_sell() + get_price_det();
        }
        return order_spread3("m1809", buy, open, price, ctp_strategy::get_time().c_str());
    }

    //执行下单命令
    int order_spread3(const char* code, bool buy, bool open, int price,const char*s_time){
        int ret = -1;

        //if(!code || strstr(code,"&")==nullptr){
        //        assert(false);
        //        return -1;
        //}
        //////////////////////////////////////////////////////////////////////////
        ///报单录入请求
        ctp_order o;

        o.InstrumentID = code;
        o.open = open;
        o.buy = buy;
        o.price = price;
        o.time = s_time;

        ret = order_spread2(&o);
        //t.evt->wait(-1);
        return ret;
    }

    //执行下单命令
    int order_spread2(ctp_ReqOrderInsert* o){
        int ret = -1;
        t.g->IncOrderRef();//递增本地order id
        o->order_ref = t.g->g_orderRef;

        t.g->IncRequestID();
        o->request_id = t.g->g_iRequestID;//递增请求id

        ret = t.ReqOrderInsert(o);//下单
        //t.evt->wait(-1);
        return ret;
    }

    std::string get_code(){//返回第一个期货代码
        std::vector<std::string> dels;
        dels.push_back(";");
        dels.push_back(",");
        auto arr = string_split(FLAG_ctp_trade_code, dels);
        return arr[0];
    }

    int get_current_buy(){ //当前的买价
        return m_spread_buy->i;
    }
    int get_current_sell(){ //当前的卖价
        return m_spread_sell->i;
    }
    int get_price_det(){ //超价3个点买卖
        return 3;
    }
    void print_menu(){//命令行菜单
        printf("Select command:\n");
        printf("future price :          %d %d\n", m_spread_sell->i, m_spread_buy->i);
        printf("1. buy,  open  :price = %d\n", get_current_sell() + get_price_det());
        printf("2. buy,  close :price = %d\n", get_current_sell() + get_price_det());
        printf("3. sell, open  :price = %d\n", get_current_buy() - get_price_det());
        printf("4. sell, close :price = %d\n", get_current_buy() - get_price_det());
        
        printf("a. buy,  open  :price = %d\n", get_current_buy() - get_price_det());
        printf("b. buy,  close :price = %d\n", get_current_buy() - get_price_det());
        printf("c. sell, open  :price = %d\n", get_current_sell() + get_price_det());
        printf("d. sell, close :price = %d\n", get_current_sell() + get_price_det());
    }

};

//负责提供期货行情
class my_ctp_market {
public:
    my_ctp_market():m_has_dot(false){}

    ctp_market m;

    ctp_int m_last_spread1, m_last_spread2;//上次的价格
    ctp_int m_spread1, m_spread2;//本次价格

    bool m_has_dot;//打印 '.'

    std::function<void(const char* instrumentID, int spread1,int spread2,const char* s_time)> on_spread_changed;

    int start(){
        m.set_codes(FLAG_ctp_market_code);

        auto pthis =this;

        //设置接收期货行情的函数: 有新行情的时候, 这个函数会被调用
        m.set_market_callback([&,pthis](ctp_market_callback_data* d){

            //保存期货代码和买/卖价格
            m.set_price(d->InstrumentID, d->BuyPrice1, d->SellPrice1);
            //保存价格
            pthis->m_spread1.set((int)d->BuyPrice1);
            pthis->m_spread2.set((int)d->SellPrice1);
            //发出价格改变通知
            pthis->on_spread_changed(d->InstrumentID,(int)d->BuyPrice1,(int)d->SellPrice1,d->UpdateTime);

            if( pthis->m_spread1.equal(pthis->m_last_spread1) &&
                pthis->m_spread2.equal(pthis->m_last_spread2) ){
                    //价格相等,和上次一样, 打印一个 '.'
                    pthis->m_has_dot =true;
                    printf(".");
                    return;
            }

            pthis->m_last_spread1 = pthis->m_spread1;
            pthis->m_last_spread2 = pthis->m_spread2;

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

            pthis->m_has_dot=false;

        });
        return m.start();
    }
};


static int stategy_start( ctp_strategy_range2 &sr, my_ctp_trader& t)
{
    sr.set_market_codes(FLAG_ctp_market_code);
    sr.set_trade_code(FLAG_ctp_trade_code);

    //下单时调用的函数
    sr.set_on_order ( std::bind(&my_ctp_trader::order_spread2, &t, std::placeholders::_1 ));

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
    my_ctp_trader t;//交易账户

    //////////////////////////////////////////////////////////////////////////
    auto ranges = get_ranges();//从命令行取到区间定义
    int ret=0;
    for(int i=0;i<ranges.size();i++){//遍历区间定义

        ctp_strategy_range2_ptr sr(new ctp_strategy_range2());//新建区间交易策略

        //为区间设置 id
        char s_id[128]; sprintf(s_id,"rfuture%d", i+1);
        sr->set_range ( ranges[i] );
        sr->set_id(s_id);

        //启动区间交易策略
        ret = stategy_start(*sr, t);
        if(ret)return ret;//失败

        t.strat_ranges.push_back(sr);//保存区间交易策略
    }


    //////////////////////////////////////////////////////////////////////////    
    ret = t.start();//启动交易账户
    if(ret)return ret;

    //////////////////////////////////////////////////////////////////////////
    my_ctp_market m;//行情价格

    t.m_spread_sell = &m.m_spread1;
    t.m_spread_buy = &m.m_spread2;
    //设置行情改变时的回调函数
    m.on_spread_changed =[&](const char* instrumentID,int spread_buy,int spread_sell,const char* spread_time){
        //行情改变了, 调用价差交易策略的处理函数,可能触发下单
        for(int i=0;i< t.strat_ranges.size();i++){
            //通知策略,价格改变了
            t.strat_ranges[i]->on_spread_change(instrumentID,spread_buy,spread_sell,spread_time);
        }
    };

    ret = m.start();//启动行情
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
            t.order_future(true,true, true);
            break;
        case '2':
            t.order_future(true,false, true);
            break;
        case '3':
            t.order_future(false,true, true);
            break;
        case '4':
            t.order_future(false,false, true);
            break;

        case 'a':
            t.order_future(true,true, false);
            break;
        case 'b':
            t.order_future(true,false, false);
            break;
        case 'c':
            t.order_future(false,true, false);
            break;
        case 'd':
            t.order_future(false,false, false);
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

}//namespace ctp_future

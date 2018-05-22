#pragma once

#include "ctp-strategy.h"
#include "../ctp-market/ctp-int.h"
#include "ctp-strategy-range.h"
#include "ctp-strategy-range2-order.h"

#include <string>
#include <vector>

//能同时买卖的策略: 内部使用独立的买和卖两个策略
class ctp_strategy_range2 : public ctp_strategy_range{
public:
    ctp_strategy_range2()
        :m_buy(true)
        ,m_sell(false)
    {
        set_id("r2");
        m_buy.set_id(m_id+"b");
        m_sell.set_id(m_id+"s");
    }
    
    virtual void set_id(const std::string& i){
        ctp_strategy_range::set_id(i);
        m_buy.set_id(m_id+"b");
        m_sell.set_id(m_id+"s");
    }
    virtual void set_trading_day(const std::string& i,const std::string& sid){
        ctp_strategy_range::set_trading_day(i,sid);
        m_buy.set_trading_day(i,sid);
        m_sell.set_trading_day(i,sid);
    }
    virtual void set_market_codes(const std::string& i){
        ctp_strategy_range::set_market_codes(i);
        m_buy.set_market_codes(i);
        m_sell.set_market_codes(i);
    }
    virtual void set_trade_code(const std::string& i){
        ctp_strategy_range::set_trade_code(i);
        m_buy.set_trade_code(i);
        m_sell.set_trade_code(i);
    }
    virtual int set_range(const ctp_range &f){
        auto ret = ctp_strategy_range::set_range(f);
        if(ret)return ret;
        ret=m_buy.set_range(f);
        if(ret)return ret;
        ret=m_sell.set_range(f);
        return ret;
    }
    virtual void set_on_order(ctp_order_func f){ 
        ctp_strategy_range::set_on_order(f);
        m_buy.set_on_order(f);
        m_sell.set_on_order(f);
    }

    virtual int start();


    virtual int on_spread_change( const char* instrumentID, int spread_buy, int spread_sell ,const char* s_time);
    virtual int on_notify(const ctp_trader_callback_data* d);

private:

    virtual int do_buy_open();
    virtual int do_sell_open();
    virtual int do_close( const ctp_strategy_position& p );

    int on_spread_change();

    ctp_strategy_range2_order m_buy, m_sell;
};

typedef std::shared_ptr<ctp_strategy_range2> ctp_strategy_range2_ptr;
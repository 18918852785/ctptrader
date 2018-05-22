#pragma once

#include "ctp-strategy.h"
#include "../ctp-market/ctp-int.h"
#include "ctp-strategy-range.h"

#include <string>
#include <vector>

class ctp_trader_callback_data;

class ctp_strategy_range2_order : public ctp_strategy_range{
public:
    ctp_strategy_range2_order(bool buy):m_is_buy(buy)
    {
        set_id("r2");
    }
    

    virtual int start();


    virtual int on_spread_change( const char* instrumentID, int spread_buy, int spread_sell ,const char* s_time);
    virtual int on_notify(const ctp_trader_callback_data* d);
private:

    virtual int do_buy_open();
    virtual int do_sell_open();
    virtual int do_close( const ctp_strategy_position& p );
    virtual int do_close( const ctp_strategy_position& p,int profit );

    int on_spread_change();

    bool m_is_buy;
};

typedef std::shared_ptr<ctp_strategy_range2_order> ctp_strategy_range2_order_ptr;
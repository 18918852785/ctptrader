#pragma once

#include "ctp-strategy.h"
#include "../ctp-market/ctp-int.h"
#include "ctp-strategy-range.h"

#include <string>
#include <vector>


class ctp_strategy_range1 : public ctp_strategy_range{
public:
    ctp_strategy_range1()
    {
        set_id("r1");
    }
    

    virtual int start();
    virtual int set_range(const ctp_range &r){return ctp_strategy_range::set_range(r);}


    virtual int on_spread_change( const char* instrumentID, int spread_buy, int spread_sell ,const char* s_time);
private:

    virtual int do_buy_open();
    virtual int do_sell_open();
    virtual int do_close( const ctp_strategy_position& p );

    int on_spread_change();
};

typedef std::shared_ptr<ctp_strategy_range1> ctp_strategy_range1_ptr;
#pragma once

#include "ctp-strategy.h"
#include "../ctp-market/ctp-int.h"


class ctp_range{
public:

    ctp_int m_range_high, m_range_low;
    ctp_int m_price_det;
    ctp_int m_profit;

    std::string to_string()const;
};

class ctp_strategy_range : public ctp_strategy{
public:
	ctp_strategy_range(void);

	int start();

    virtual int on_spread_change(const char* instrumentID, int spread_buy, int spread_sell,const char* s_time);
    virtual int set_range(const ctp_range &r){m_range=r;return 0;}

    ctp_int m_buy, m_sell;
    std::string m_time;

    int get_current_buy();
    int get_current_sell();

    int get_price_det();

protected:
    virtual int on_spread_change();

    virtual int calc_profit( const ctp_strategy_position& p );

    virtual bool price_gt_high();
    virtual bool price_lt_low();

    virtual int do_buy_open();
    virtual int do_sell_open();
    virtual int do_close( const ctp_strategy_position& p );

    ctp_range m_range;
private:
    ctp_int m_last_buy, m_last_sell;
};


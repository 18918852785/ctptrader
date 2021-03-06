// ctp-strategy_range.cpp : 定义 DLL 应用程序的导出函数。
//

#include "ctp-strategy-range.h"

#include "../../include/string/utf8_string.h"
#include "../../include/config/bs_config.h"
#include "../ctp-log/ctp-log.h"

#include <assert.h>
#include <sstream>

#include "../gtest/flags.h"

ctp_strategy_range::ctp_strategy_range()
{
}

int ctp_strategy_range::start()
{
    auto ret = ctp_strategy::start();

    CTP_PRINT( " [%s] range is %s\n", m_id.c_str(), m_range.to_string().c_str());
    return ret;
}

int ctp_strategy_range::on_spread_change( const char* instrumentID, int spread_buy, int spread_sell ,const char* s_time)
{
    //是不是我们关心的价格?
    if( ! is_my_market_code(instrumentID)) return -1;

    //保存价格
    m_buy.set(spread_buy); m_sell.set(spread_sell);
    m_time = s_time;

    //判断价差是否改变
    if( m_last_buy.equal(m_buy) && 
        m_last_sell.equal(m_sell))
        return -1;//价格没有变化

    //保存上次价格
    m_last_buy = m_buy; m_last_sell = m_sell;

    return on_spread_change();
}


int ctp_strategy_range::get_current_buy()
{
    return m_buy.i;
}

int ctp_strategy_range::get_current_sell()
{
    return m_sell.i;
}

int ctp_strategy_range::get_price_det()
{
    return m_range.m_price_det.i;
}

std::string ctp_range::to_string() const
{
    std::ostringstream ss;

    ss <<"high = "     <<m_range_high.i<<" ,";
    ss <<"low = "      <<m_range_low.i <<" ,";
    ss <<"price_det = "<<m_price_det.i <<" ,";
    ss <<"profit = "   <<m_profit.i;
    return ss.str();
}


int ctp_strategy_range::calc_profit( const ctp_strategy_position& p )
{
    assert(p.open);
    if(!p.open)return 0;

    if(p.buy)//持有买单
    {
        //现在卖的话,利润多少
        return m_buy.i-p.price ;
    }

    //持有的是卖单,现在买的话,利润多少
    return p.price-m_sell.i;
}

bool ctp_strategy_range::price_gt_high()
{
    return m_buy.valid && m_buy.i >= m_range.m_range_high.i;
}

bool ctp_strategy_range::price_lt_low()
{
    return m_sell.valid && m_sell.i <= m_range.m_range_low.i;
}

int ctp_strategy_range::do_buy_open()
{
    assert(false);
    return -1;
}

int ctp_strategy_range::do_sell_open()
{
    assert(false);
    return -1;
}


int ctp_strategy_range::do_close( const ctp_strategy_position& p )
{
    assert(false);
    return -1;
}

int ctp_strategy_range::on_spread_change()
{
    //////////////////////////////////////////////////////////////////////////
    auto ret = is_trade_time(m_time.c_str())?0:-1; //是否是交易时间
    CTP_PRINT("[%s]on_spread_change: time = %s, buy = %d, sell = %d, ret=%d \n", 
        m_id.c_str(),
        m_time.c_str(),  get_current_buy(), get_current_sell(), ret);

    if(ret)return ret;//非交易时间, 不要下单


    return ret;
}

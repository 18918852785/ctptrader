// ctp-strategy_range.cpp : 定义 DLL 应用程序的导出函数。
//

#include "ctp-strategy-range2.h"

#include "../../include/string/utf8_string.h"
#include "../../include/config/bs_config.h"

#include <assert.h>
#include "../gtest/flags.h"


int ctp_strategy_range2::start()
{
    //调用基类的start
    auto ret = ctp_strategy_range::start();
    if(ret) return  ret;

    //启动 买 策略
    ret = m_buy.start();
    if(ret) return  ret;

    //启动 卖 策略
    ret = m_sell.start();
    if(ret) return  ret;

    return ret;
}

int ctp_strategy_range2::on_spread_change( const char* instrumentID, int spread_buy, int spread_sell,const char* s_time )
{
    int ret = ctp_strategy_range::on_spread_change(instrumentID,spread_buy,spread_sell,s_time);
    if(ret != 0) return ret;

    ret = m_buy.on_spread_change(instrumentID, spread_buy,spread_sell,s_time);
    if(ret != 0) return ret;

    ret = m_sell.on_spread_change(instrumentID, spread_buy,spread_sell,s_time);
    if(ret != 0) return ret;

    return 0;
}

int ctp_strategy_range2::on_notify(const ctp_trader_callback_data* d)
{
    auto ret = ctp_strategy_range::on_notify(d);
    if(ret)return ret;

    m_buy.on_notify(d);
    m_sell.on_notify(d);
    return ret;
}

int ctp_strategy_range2::do_buy_open()
{
    return -1;
}

int ctp_strategy_range2::do_sell_open()
{
    return -1;
}


int ctp_strategy_range2::do_close( const ctp_strategy_position& p )
{
    return -1;
}

int ctp_strategy_range2::on_spread_change()
{
    int ret = ctp_strategy_range::on_spread_change();
    if(ret != 0) return ret;

    return 0;
}

// ctp-market.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "ctp-order-book.h"
#include "ctp-trader-global.h"

#include "../../include/string/utf8_string.h"
#include "../ctp-log/ctp-log.h"

ctp_order_book::ctp_order_book()
{
}



bool ctp_trader_callback_OnRtnOrder::is_final() const
{

    //AllTraded = 48,                                          // 全部成交。                                                                                  --->最终状态
    //Canceled = 53,                                           // 报单被取消                                                                                 --->最终状态
    //NoTradeNotQueueing = 52,                     // 没有交易 且不在队列中，说明：报单被CTP拒绝。                       --->最终状态
    //PartTradedNotQueueing = 50,                // 部分成交，不在队列中，说明：部成部撤。                                    --->最终状态。
    return (OrderStatus == THOST_FTDC_OST_PartTradedNotQueueing) ||
        (OrderStatus == THOST_FTDC_OST_NoTradeNotQueueing) ||
        (OrderStatus == THOST_FTDC_OST_Canceled) ||
        (OrderStatus != THOST_FTDC_OST_AllTraded);
}


// ctp-market.cpp : ���� DLL Ӧ�ó���ĵ���������
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

    //AllTraded = 48,                                          // ȫ���ɽ���                                                                                  --->����״̬
    //Canceled = 53,                                           // ������ȡ��                                                                                 --->����״̬
    //NoTradeNotQueueing = 52,                     // û�н��� �Ҳ��ڶ����У�˵����������CTP�ܾ���                       --->����״̬
    //PartTradedNotQueueing = 50,                // ���ֳɽ������ڶ����У�˵�������ɲ�����                                    --->����״̬��
    return (OrderStatus == THOST_FTDC_OST_PartTradedNotQueueing) ||
        (OrderStatus == THOST_FTDC_OST_NoTradeNotQueueing) ||
        (OrderStatus == THOST_FTDC_OST_Canceled) ||
        (OrderStatus != THOST_FTDC_OST_AllTraded);
}


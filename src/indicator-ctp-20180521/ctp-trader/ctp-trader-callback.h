#pragma once

#include "ctp-trader-export.h"

#include <memory>
#include <functional>

//////////////////////////////////////////////////////////////////////////
enum ctp_trader_event_id{
    TE_UNKNOWN=0,
    event_OnRspUserLogin,
    event_OnRspSettlementInfoConfirm,
    event_OnRspQryTradingAccount,
    event_OnRspOrderAction,

    event_OnRspOrderInsert,//下单
    event_OnRtnOrder,//交易所应答
    event_OnRtnTrade,//交易成功
};

//////////////////////////////////////////////////////////////////////////
class ctp_trader_global;
typedef std::shared_ptr<ctp_trader_global> ctp_trader_global_ptr;

//////////////////////////////////////////////////////////////////////////
class ctp_trader_callback_OnRspOrderInsert;
class ctp_trader_callback_OnRtnOrder;
class ctp_trader_callback_OnRtnTrade;

class CTPMARKET_API ctp_trader_callback_data{
public:
    ctp_trader_callback_data()
        :InstrumentID(""),
        UpdateTime(""),
        UpdateMillisec(0),
        Volume(0),
        BuyPrice1(0),
        SellPrice1(0),
        event_id(ctp_trader_event_id::TE_UNKNOWN)
    {}
    virtual ~ctp_trader_callback_data(){}

public:
    //////////////////////////////////////////////////////////////////////////
    virtual const ctp_trader_callback_OnRspOrderInsert* to_OnRspOrderInsert()const{return nullptr;}
    virtual const ctp_trader_callback_OnRtnOrder* to_OnRtnOrder()const{return nullptr;}
    virtual const ctp_trader_callback_OnRtnTrade* to_OnRtnTrade()const{return nullptr;}
    //////////////////////////////////////////////////////////////////////////
public:
    char * InstrumentID;
    char * UpdateTime;
    int UpdateMillisec;
    int Volume;
    float BuyPrice1,SellPrice1;

    std::string trading_day;
    std::string session_id;
    ctp_trader_event_id event_id;
};


//柜台校验出错返回
class CTPMARKET_API ctp_trader_callback_OnRspOrderInsert: public ctp_trader_callback_data{
public:
    ctp_trader_callback_OnRspOrderInsert():ErrorID(0)
    {event_id = event_OnRspOrderInsert;}

    virtual const ctp_trader_callback_OnRspOrderInsert* to_OnRspOrderInsert()const{return this;}

    std::string OrderRef;

    ///错误代码
    int	ErrorID;
    ///错误信息
    std::string ErrorMsg;
};

///报单进入交易所后的通知
class CTPMARKET_API ctp_trader_callback_OnRtnOrder: public ctp_trader_callback_data{
public:
    ctp_trader_callback_OnRtnOrder():OrderStatus(0)
    {event_id = event_OnRtnOrder;}
    virtual const  ctp_trader_callback_OnRtnOrder* to_OnRtnOrder()const{return this;}

    char OrderStatus;
    std::string OrderRef,OrderSysID;
    bool is_final()const;
};

///成交通知
class CTPMARKET_API ctp_trader_callback_OnRtnTrade: public ctp_trader_callback_data{
public:
    ctp_trader_callback_OnRtnTrade()
    {event_id = event_OnRtnTrade;}

    virtual const ctp_trader_callback_OnRtnTrade* to_OnRtnTrade()const{return this;}

    std::string OrderRef,OrderSysID;
};

typedef std::function<void(const ctp_trader_callback_data*)> ctp_trader_callback;

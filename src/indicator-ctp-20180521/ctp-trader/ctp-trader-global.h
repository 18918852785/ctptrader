#pragma once
#include "ctp-trader-export.h"
#include "ctp-trader-callback.h"
#include "../ctp-market/ctp-int.h"
#include <string>
#include <vector>

class CTPMARKET_API ctp_ReqOrderInsert{

public:
    ctp_ReqOrderInsert()
        :InstrumentID("")
        ,buy(false)
        ,open(false)
        ,price(0)
    {
    }
    std::string order_ref;
    ctp_int request_id;

    const char* InstrumentID;
    bool buy;
    bool open;

    double price;

    std::string time;

    bool is_sp()const;
};

class CTraderSpi;
class CThostFtdcTraderApi;
class ctp_order_book;
class ctp_criticalsection;

typedef std::shared_ptr<ctp_order_book> ctp_order_book_ptr;
typedef std::shared_ptr<ctp_criticalsection> ctp_criticalsection_ptr;
namespace thread
{
    struct ithread;
    typedef std::shared_ptr<ithread> ithread_ptr;
}
class CTPMARKET_API ctp_trader_global{
public:
    ctp_trader_global();
    ~ctp_trader_global();
    int init();


    CThostFtdcTraderApi* g_pTraderApi;
    CTraderSpi* g_pTraderSpi;

    std::string g_strTraderFront;
    std::string g_strMdFront;
    std::string g_brokerId ;
    std::string g_investorId ;
    std::string g_passWord ;

    ctp_codes g_instrumentId;
    std::vector<std::string> g_instrumentId_data;

    char g_direction;
    ctp_int g_iRequestID;

    ctp_int g_frontId;
    ctp_int g_sessionId;
    std::string g_orderRef;
    std::string g_trading_day;

    ::thread::ithread_ptr g_trader_thread;

    ctp_trader_callback on_trader_data;
    ctp_order_book_ptr book;
    ctp_criticalsection_ptr locker;

    void IncOrderRef();
    void IncRequestID();

    void notify(const ctp_trader_callback_data& d);
};
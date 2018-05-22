#pragma once

#include "ctp-market-export.h"

#include <memory>
#include <functional>

class ctp_market_global;
typedef std::shared_ptr<ctp_market_global> ctp_market_global_ptr;

struct CThostFtdcDepthMarketDataField;

//行情价格: 回调数据
class CTPMARKET_API ctp_market_callback_data{
public:
    ctp_market_callback_data():InstrumentID(""),
        UpdateTime(""),
        UpdateMillisec(0),
        Volume(0),
        BuyPrice1(0),
        SellPrice1(0){}

    char * InstrumentID;//期货代码: M1901
    char * UpdateTime;  //时间
    int UpdateMillisec; //毫秒
    int Volume;         //成交量
    float BuyPrice1,SellPrice1;//买一价,卖一价

    CThostFtdcDepthMarketDataField* ctp_p;//ctp内部数据结构
};
typedef std::function<void(ctp_market_callback_data*)> ctp_market_callback;

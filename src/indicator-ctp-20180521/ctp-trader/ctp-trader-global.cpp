// ctp-market.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "ctp-trader-global.h"
#include "../ctp-log/ctp-log.h"
#include "../ctp-market/ctp-criticalsection.h"
#include "ctp-order-book.h"

#include "../gtest/flags.h"

DEFINE_FLAG( int, simnow, 1,"simnow模拟");

//2018.4.21
//从徽商期货下载快期交易软件
//%user_home%\AppData\Roaming\Q73_hsqh_3_standard\20180421143557\broker.xml
//BrokerID="8888" BrokerName="徽商期货"
/*
<Servers>
<Server>
<Name>上海电信一</Name>
<Trading>
<item>118.242.3.178:41205</item>
<item>118.242.3.179:41205</item>
</Trading>
<MarketData>
<item>118.242.3.178:41173</item>
<item>118.242.3.179:41173</item>
</MarketData>
</Server>
<Server>
<Name>上海电信二</Name>
<Trading>
<item>49.239.1.90:41205</item>
<item>49.239.1.91:41205</item>
</Trading>
<MarketData>
<item>49.239.1.90:41173</item>
<item>49.239.1.91:41173</item>
</MarketData>
</Server>

<Server>
<Name>上海联通</Name>
<Trading>
<item>27.115.57.196:41205</item>
<item>27.115.57.197:41205</item>
</Trading>
<MarketData>
<item>27.115.57.196:41173</item>
<item>27.115.57.197:41173</item>
</MarketData>
</Server>

<Server>
<Name>北京站点</Name>
<Trading>
<item>119.254.89.50:41205</item>
</Trading>
<MarketData>
<item>119.254.89.50:41173</item>
</MarketData>
</Server>
</Servers>
*/

ctp_trader_global::ctp_trader_global()
    :locker (new ctp_criticalsection())
{
    init();
}

ctp_trader_global::~ctp_trader_global()
{

}

int ctp_trader_global::init()
{
    dpu::Flags::Init();
    CTP_LOG_INIT("ctp-trader");

    g_pTraderApi = nullptr;
    //CThostFtdcTraderApi* g_pTraderApi2;
    switch(FLAG_simnow){
    default:
    case 1:{
        CTP_PRINT(" ****** Simnow **************************************************\n");
        //模拟不间断交易,周末也交易:
        g_strTraderFront = "tcp://180.168.146.187:10030";
        g_strMdFront = "tcp://180.168.146.187:10031";
        g_brokerId = "9999";
        g_investorId = "068854";
        g_passWord = "123456";
           }
           break;
    case 2:{
        CTP_PRINT(" ****** Simnow **************************************************\n");
        //模拟
        g_strTraderFront = "tcp://180.168.146.187:10000";
        g_strMdFront = "tcp://180.168.146.187:10010";
        g_brokerId = "9999";
        g_investorId = "068854";
        g_passWord = "123456";
           }
           break;
    case 0:{
        // ****************************************************************
        CTP_PRINT(" ****** 徽商期货 交易**************************************************\n");
        g_strTraderFront = "tcp://118.242.3.178:41205";
        g_strMdFront = "tcp://118.242.3.178:41173";
        g_brokerId = "8888";
        g_investorId = "用户id";
        g_passWord = "密码";
        // ****************************************************************
        }
           break;
    }
 
    g_instrumentId_data.push_back("SP m1809&m1901");
    g_instrumentId.init(g_instrumentId_data);

    //g_instrumentId1 = "m1809";//"rb1901";//rb1810
    //g_instrumentId2 = "m1901";//"rb1901";//rb1810

    g_direction = THOST_FTDC_D_Buy;
    g_iRequestID.i = 0;

    //////////////////////////////////////////////////////////////////////////
    book = std::make_shared<ctp_order_book>();
    return 0;
}

void ctp_trader_global::IncOrderRef()
{
    auto i= atoi(g_orderRef.c_str());
    i++;
    char s[128];
    itoa(i+1,s,10);
    g_orderRef = s;
}

void ctp_trader_global::IncRequestID()
{
    g_iRequestID.i++;
}

void ctp_trader_global::notify(const ctp_trader_callback_data& d)
{
    ctp_locker l(locker);
    on_trader_data(&d);
}

bool ctp_ReqOrderInsert::is_sp() const
{
    std::string s(InstrumentID);
    return s.find('&') != -1;
}

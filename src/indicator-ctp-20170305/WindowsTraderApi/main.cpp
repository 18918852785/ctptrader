#include "TraderSpi.h"


CThostFtdcTraderApi* g_pTraderApi = nullptr;
// ****** Simnow **************************************************
char* g_strTraderFront = "tcp://180.168.146.187:10000";
TThostFtdcBrokerIDType g_brokerId = "9999";
TThostFtdcInvestorIDType g_investorId = "068854";
TThostFtdcPasswordType  g_passWord = "123456";

TThostFtdcInstrumentIDType g_instrumentId = "m1809";//买卖的合约
TThostFtdcPriceType g_limitPrice = 3069; //买卖的价格
TThostFtdcDirectionType g_direction = THOST_FTDC_D_Buy; //买
int iRequestID = 0; //请求id

void main()
{
    g_pTraderApi  = CThostFtdcTraderApi::CreateFtdcTraderApi();
    CThostFtdcTraderSpi* pUserSpi = new CTraderSpi();

    g_pTraderApi->RegisterSpi(pUserSpi);
    g_pTraderApi->SubscribePublicTopic(THOST_TERT_QUICK);
    g_pTraderApi->SubscribePrivateTopic(THOST_TERT_QUICK);
    g_pTraderApi->RegisterFront(g_strTraderFront);
    g_pTraderApi->Init();
    g_pTraderApi->Join();

}
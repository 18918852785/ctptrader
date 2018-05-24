#include "TraderSpi.h"


CThostFtdcTraderApi* g_pTraderApi;
CThostFtdcTraderApi* g_pTraderApi2;
char* g_strTraderFront = "tcp://222.66.235.70:21205";
TThostFtdcBrokerIDType g_brokerId = "66666";
TThostFtdcInvestorIDType g_investorId = "8001034695";
TThostFtdcPasswordType  g_passWord = "123456";
TThostFtdcInstrumentIDType g_instrumentId = "IF1608";
TThostFtdcPriceType g_limitPrice = 3069;
TThostFtdcDirectionType g_direction = THOST_FTDC_D_Buy;
int iRequestID = 0;

void main(void)
{
    g_pTraderApi  = CThostFtdcTraderApi::CreateFtdcTraderApi();
    g_pTraderApi2 = CThostFtdcTraderApi::CreateFtdcTraderApi();
    CThostFtdcTraderSpi* pUserSpi = new CTraderSpi();

    g_pTraderApi->RegisterSpi(pUserSpi);
    g_pTraderApi->SubscribePublicTopic(THOST_TERT_QUICK);
    g_pTraderApi->SubscribePrivateTopic(THOST_TERT_QUICK);
    g_pTraderApi->RegisterFront(g_strTraderFront);
    g_pTraderApi->Init();
    g_pTraderApi->Join();

    g_pTraderApi2->RegisterSpi(pUserSpi);
    g_pTraderApi2->SubscribePublicTopic(THOST_TERT_QUICK);
    g_pTraderApi2->SubscribePrivateTopic(THOST_TERT_QUICK);
    g_pTraderApi2->RegisterFront(g_strTraderFront);
    g_pTraderApi2->Init();
    g_pTraderApi2->Join();
}
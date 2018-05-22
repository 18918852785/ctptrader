// ctp-market.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "ctp-market-global.h"

#include "../gtest/flags.h"
#include "../ctp-log/ctp-log.h"

DEFINE_FLAG( int, simnow, 1,"simnowģ��");

ctp_market_global::ctp_market_global()
{

}

ctp_market_global::~ctp_market_global()
{

}

int ctp_market_global::init()
{
    dpu::Flags::Init();
    CTP_LOG_INIT("ctp-market");

    g_pMdApi = nullptr;
    switch(FLAG_simnow){
    default:
    case 1:{
        // ****** Simnow **************************************************
        g_strMdFront = "tcp://180.168.146.187:10031";//10010";
        g_brokerId = "9999";
        g_investorId = "068854";
        g_passWord = "123456";
        // ****************************************************************
           }
           break;
    case 2:{
        // ****** Simnow **************************************************
        g_strMdFront = "tcp://180.168.146.187:10010";
        g_brokerId = "9999";
        g_investorId = "068854";
        g_passWord = "123456";
        // ****************************************************************
           }
           break;
    case 0:
        {
        CTP_PRINT(" ****** �����ڻ� ����**************************************************\n");
        //g_strTraderFront = "tcp://118.242.3.178:41205";
        g_strMdFront = "tcp://118.242.3.178:41173";
        g_brokerId = "8888";
        g_investorId = "�û�id";
        g_passWord = "******";
        // ****************************************************************
        }
        break;
    }
    g_instrumentId_data.clear();
    g_instrumentId.clear();
    g_iRequestID.i = 0;

    return 0;
}

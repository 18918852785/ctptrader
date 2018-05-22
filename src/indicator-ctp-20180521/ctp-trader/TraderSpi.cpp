#include "StdAfx.h"
#include <windows.h>

#include "../ctp-log/ctp-log.h"
#include "../ctp-market/ctp-criticalsection.h"
#include "../../include/string/utf8_string.h"

#pragma warning(disable : 4996)
using namespace std;


// �����ж�
static bool IsFlowControl(int iResult)
{
    return ((iResult == -2) || (iResult == -3));
}

void CTraderSpi::OnFrontConnected()
{
    CTP_LOG_INFO( "--->>> " << "OnFrontConnected" << endl);
    ///�û���¼����
    ReqUserLogin();
}

void CTraderSpi::ReqUserLogin()
{
    CThostFtdcReqUserLoginField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, g->g_brokerId.c_str());
    strcpy(req.UserID, g->g_investorId.c_str());
    strcpy(req.Password, g->g_passWord.c_str());

    g->IncRequestID();
    int iResult = g->g_pTraderApi->ReqUserLogin(&req, g->g_iRequestID.i);
    CTP_LOG_INFO( "--->>> �����û���¼����: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl);
}

void CTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    std::ostringstream msg;

    msg<< "--->>> " << "OnRspUserLogin" << endl;
    if (bIsLast && !IsErrorRspInfo(pRspInfo))
    {
        // ����Ự����
        g->g_frontId.i = pRspUserLogin->FrontID;
        g->g_sessionId.i = pRspUserLogin->SessionID;
        int iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
        iNextOrderRef++;
        char s_orderRef[128];
        sprintf(s_orderRef, "%d", iNextOrderRef);
        g->g_orderRef = s_orderRef;
        
        ///��ȡ��ǰ������
        g->g_trading_day = g->g_pTraderApi->GetTradingDay();
        msg<< "--->>> ��ȡ��ǰ������ = " << g->g_trading_day << endl;

        ctp_trader_callback_data d;
        d.event_id = event_OnRspUserLogin;
        d.trading_day = g->g_trading_day;
        d.session_id = string_format("%X",g->g_sessionId.i);
        notify(d);

        ///Ͷ���߽�����ȷ��
        ReqSettlementInfoConfirm();
    }
    CTP_LOG_INFO(msg.str());
}

void CTraderSpi::ReqSettlementInfoConfirm()
{
    CThostFtdcSettlementInfoConfirmField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, g->g_brokerId.c_str());
    strcpy(req.InvestorID, g->g_investorId.c_str());

    g->IncRequestID();
    int iResult = g->g_pTraderApi->ReqSettlementInfoConfirm(&req, g->g_iRequestID.i);

    CTP_LOG_INFO( "--->>> Ͷ���߽�����ȷ��: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl);
}

void CTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CTP_LOG_INFO( "--->>> " << "OnRspSettlementInfoConfirm" << endl);

    if (bIsLast && !IsErrorRspInfo(pRspInfo))
    {
        ctp_trader_callback_data d;
        d.event_id = event_OnRspSettlementInfoConfirm;
        d.trading_day = pSettlementInfoConfirm->ConfirmDate;
        notify(d);
    }
}


void CTraderSpi::ReqQryInstrument()
{
    ReqQryInstrument(g->g_instrumentId.data[0]);
    //ReqQryInstrument(g_instrumentId2);
}

void CTraderSpi::ReqQryInstrument(const char* instrumentID)
{
    CThostFtdcQryInstrumentField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.InstrumentID, instrumentID);
    int i=0;
    while (i++<3)
    {
        g->IncRequestID();
        int iResult = g->g_pTraderApi->ReqQryInstrument(&req, g->g_iRequestID.i);
        if (!IsFlowControl(iResult))
        {
            CTP_LOG_INFO( "--->>> �����ѯ��Լ: "  << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl);
            break;
        }
        else
        {
            CTP_LOG_INFO( "--->>> �����ѯ��Լ: "  << iResult << ", �ܵ�����" << endl);
            Sleep(1000);
        }
    }
}

void CTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    std::ostringstream msg;
    msg<< "--->>> " << "OnRspQryInstrument: " ;
    if(pInstrument){
        msg<<  pInstrument->ExchangeID<<"."<< pInstrument->InstrumentID <<  endl;
    }
    if (bIsLast && !IsErrorRspInfo(pRspInfo))
    {
        ///�����ѯ�˻��ʽ�
        ReqQryTradingAccount();
    }
    CTP_LOG_INFO(msg.str());
}

void CTraderSpi::ReqQryTradingAccount()
{
    CThostFtdcQryTradingAccountField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, g->g_brokerId.c_str());
    strcpy(req.InvestorID, g->g_investorId.c_str());
    int i=0;
    while (i++<3)
    {
        g->IncRequestID();
        int iResult = g->g_pTraderApi->ReqQryTradingAccount(&req, g->g_iRequestID.i);
        if (!IsFlowControl(iResult))
        {
            CTP_LOG_INFO( "--->>> �����ѯ�ʽ��˻�: "  << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl);
            break;
        }
        else
        {
            CTP_LOG_INFO( "--->>> �����ѯ�ʽ��˻�: "  << iResult << ", �ܵ�����" << endl);
            Sleep(1000);
        }
    } // while
}

void CTraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    std::ostringstream msg;

    msg<< "--->>> " << "OnRspQryTradingAccount: ";
    if(pTradingAccount){
        msg<<"Available = "<< pTradingAccount->Available<< endl;
    }
    if (bIsLast && !IsErrorRspInfo(pRspInfo))
    {
        ///�����ѯͶ���ֲ߳�
        //ReqQryInvestorPosition();

        notify(event_OnRspQryTradingAccount);
    }
    CTP_LOG_INFO(msg.str());
}

void CTraderSpi::ReqQryInvestorPosition()
{
    ReqQryInvestorPosition(g->g_instrumentId.data[0]);
    //ReqQryInvestorPosition(g_instrumentId2);
}

void CTraderSpi::ReqQryInvestorPosition(const char* instrumentID)
{
    CThostFtdcQryInvestorPositionField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, g->g_brokerId.c_str());
    strcpy(req.InvestorID, g->g_investorId.c_str());
    strcpy(req.InstrumentID, g->g_instrumentId.data[0]);
    int i=0;
    while (i++<3)
    {
        g->IncRequestID();
        int iResult = g->g_pTraderApi->ReqQryInvestorPosition(&req, g->g_iRequestID.i);
        if (!IsFlowControl(iResult))
        {
            CTP_LOG_INFO( "--->>> �����ѯͶ���ֲ߳�: "  << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl);
            break;
        }
        else
        {
            CTP_LOG_INFO( "--->>> �����ѯͶ���ֲ߳�: "  << iResult << ", �ܵ�����" << endl);
            Sleep(1000);
        }
    } // while
}

void CTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    std::ostringstream msg;

    msg<< "--->>> " << "OnRspQryInvestorPosition: " ;
    if(pInvestorPosition){
        msg<< pInvestorPosition->InstrumentID<<", "<< pInvestorPosition->PosiDirection<<", "<< pInvestorPosition->Position<<endl;
    }else{
        msg<<" (NULL) "<<endl;
    }
    if (bIsLast && !IsErrorRspInfo(pRspInfo))
    {
        ///�µ�������¼������
        //ʵ���µ�: ReqOrderInsert();
    }

    CTP_LOG_INFO(msg.str());
}

///�µ�������¼������
int CTraderSpi::ReqOrderInsert(const ctp_ReqOrderInsert* r)
{
    ctp_locker l(g->locker);

    CThostFtdcInputOrderField req;
    memset(&req, 0, sizeof(req));
    ///���͹�˾����
    strcpy(req.BrokerID, g->g_brokerId.c_str());
    ///Ͷ���ߴ���
    strcpy(req.InvestorID, g->g_investorId.c_str());
    ///��Լ����
    strcpy(req.InstrumentID, r->InstrumentID);

    ///��������
    strcpy(req.OrderRef, r->order_ref.c_str());
    ///�û�����
    // TThostFtdcUserIDType UserID;
    ///�����۸�����: �޼�
    req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
    ///��������: 
    req.Direction = r->buy?THOST_FTDC_D_Buy:THOST_FTDC_D_Sell;//  g->g_direction;
    ///��Ͽ�ƽ��־: ����
    req.CombOffsetFlag[0] = r->open? THOST_FTDC_OF_Open : THOST_FTDC_OF_Close;
    //if(r->is_sp())req.CombOffsetFlag[1]=req.CombOffsetFlag[0];
    ///���Ͷ���ױ���־
    req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
    //if(r->is_sp())req.CombHedgeFlag[1]=req.CombHedgeFlag[0];

    ///�۸�
    req.LimitPrice = r->price;
    ///����: 1
    req.VolumeTotalOriginal = 1;
    ///��Ч������: ������Ч
	//IOC��THOST_FTDC_TC_IOC�������ɽ��򳷵�
	req.TimeCondition = THOST_FTDC_TC_GFD;
    ///GTD����
    // TThostFtdcDateType GTDDate;
    ///�ɽ�������: �κ�����
    req.VolumeCondition = THOST_FTDC_VC_AV;
    ///��С�ɽ���: 1
    req.MinVolume = 1;
    ///��������: ����
    req.ContingentCondition = THOST_FTDC_CC_Immediately;
    ///ֹ���
    // TThostFtdcPriceType StopPrice;
    ///ǿƽԭ��: ��ǿƽ
    req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
    ///�Զ������־: ��
    req.IsAutoSuspend = 0;
    ///ҵ��Ԫ
    // TThostFtdcBusinessUnitType BusinessUnit;
    ///������
    // TThostFtdcRequestIDType RequestID;
    ///�û�ǿ����־: ��
    req.UserForceClose = 0;

    int iResult = g->g_pTraderApi->ReqOrderInsert(&req, r->request_id.i);

    auto s_open=r->open?"open,":"close,";

    CTP_LOG_INFO( "--->>> ����¼������: " 
        << "instrumentID="<<r->InstrumentID
        << "," << s_open
        << ",price="<<r->price
        << ",OrderRef="<<r->order_ref.c_str()
        << ",RequestID="<<r->request_id.i
        << ",result = "<< iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") 
        << endl);

    return iResult;
}

void CTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//��̨У�������

    //if(IsErrorRspInfo(pRspInfo))
    ctp_trader_callback_OnRspOrderInsert d;
    d.OrderRef = (pInputOrder?pInputOrder->OrderRef:"");
    d.ErrorID = (pRspInfo?pRspInfo->ErrorID:0);
    d.ErrorMsg = (pRspInfo?pRspInfo->ErrorMsg:"");


    CTP_LOG_INFO( "--->>> OnRspOrderInsert:" 
        << " RequestID = "<< nRequestID
        << ",RequestID = "<< (pInputOrder?pInputOrder->RequestID: 0)
        << ",OrderRef  = "<< (pInputOrder?pInputOrder->OrderRef:"")
        << ",ErrorID = "<< d.ErrorID
        << ",ErrorMsg  = "<< d.ErrorMsg
        << endl);

    notify(d);
}

//���������ݹҵ��� order ref + front id + session id ������
void CTraderSpi::ReqOrderAction(CThostFtdcOrderField *pOrder)
{
    //static bool ORDER_ACTION_SENT = false;  //�Ƿ����˱���
    //if (ORDER_ACTION_SENT)
    //    return;

    CThostFtdcInputOrderActionField req;
    memset(&req, 0, sizeof(req));
    ///���͹�˾����
    strcpy(req.BrokerID, pOrder->BrokerID);
    ///Ͷ���ߴ���
    strcpy(req.InvestorID, pOrder->InvestorID);
    ///������������
    // TThostFtdcOrderActionRefType OrderActionRef;
    ///��������
    strcpy(req.OrderRef, pOrder->OrderRef);
    ///������
    // TThostFtdcRequestIDType RequestID;
    ///ǰ�ñ��
    req.FrontID = g->g_frontId.i;
    ///�Ự���
    req.SessionID = g->g_sessionId.i;
    ///����������
    // TThostFtdcExchangeIDType ExchangeID;
    ///�������
    // TThostFtdcOrderSysIDType OrderSysID;
    ///������־
    req.ActionFlag = THOST_FTDC_AF_Delete;
    ///�۸�
    // TThostFtdcPriceType LimitPrice;
    ///�����仯
    // TThostFtdcVolumeType VolumeChange;
    ///�û�����
    // TThostFtdcUserIDType UserID;
    ///��Լ����
    strcpy(req.InstrumentID, pOrder->InstrumentID);

    g->IncRequestID();
    int iResult = g->g_pTraderApi->ReqOrderAction(&req, g->g_iRequestID.i);
    CTP_LOG_INFO( "--->>> ������������: "  <<
        "order ref="<<pOrder->OrderRef<<", " 
        << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl);
    //ORDER_ACTION_SENT = true;
}

//�������������֪ͨ�����磬����������˲�䣬�����Ѿ�ȫ���ɽ�
void CTraderSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CTP_LOG_INFO( "--->>> " << "OnRspOrderAction" << endl);
    if(IsErrorRspInfo(pRspInfo))
        notify(event_OnRspOrderAction);
}

///�������뽻�������֪ͨ
void CTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
    std::ostringstream msg;

    msg<< "--->>> " << "OnRtnOrder ";
    if (IsMyOrder(pOrder))
    {
        msg  <<" InstrumentID="  << pOrder->InstrumentID 
             <<",RequestID = "   << pOrder->RequestID 
             <<",OrderRef = "    << pOrder->OrderRef     
             <<", OrderLocalID= " <<pOrder->OrderLocalID
             <<",OrderSysID = "  << pOrder->OrderSysID
             <<",OrderStatus = " << pOrder->OrderStatus;


        //�Ƿ����ڽ��׵ı���
        if (IsTradingOrder(pOrder))//�ҵ���δ�ɽ�״̬
        {
            msg<<",IsTradingOrder == true, "<<endl;
            //ReqOrderAction(pOrder);//����
        }
        else if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled)
            msg<< ",--->>> �����ɹ�"<<endl;

        CTP_LOG_INFO(msg.str());

    }else{
        CTP_LOG_INFO(msg.str());
    }


    ctp_trader_callback_OnRtnOrder d;
    d.OrderRef = string_trim(pOrder?pOrder->OrderRef:"");
    d.OrderStatus = ( pOrder?pOrder->OrderStatus: 0);
    d.OrderSysID= string_trim(pOrder?pOrder->OrderSysID:"");
    notify(d);
}

///�ɽ�֪ͨ
void CTraderSpi::OnRtnTrade(CThostFtdcTradeField *pOrder)
{

    std::ostringstream msg;

    msg<< "--->>> OnRtnTrade:";
    if (IsMyOrder(pOrder))
    {
        msg << " InstrumentID = "<<pOrder->InstrumentID
            <<", OrderRef = " <<pOrder->OrderRef
            <<", OrderSysID = " <<pOrder->OrderSysID
            <<", OrderLocalID= " <<pOrder->OrderLocalID
            <<", price = " << pOrder->Price
            <<", �ɽ�"
            <<endl;

        CTP_LOG_INFO(msg.str());

    }else{
        CTP_LOG_INFO(msg.str());
    }

    ctp_trader_callback_OnRtnTrade d;
    d.OrderRef = string_trim(pOrder?pOrder->OrderRef:"");
    d.OrderSysID= string_trim(pOrder?pOrder->OrderSysID:"");
    notify(d);
}

void CTraderSpi:: OnFrontDisconnected(int nReason)
{
    CTP_LOG_INFO( "--->>> " << "OnFrontDisconnected" << endl
        << "--->>> Reason = " << nReason << endl);
}

void CTraderSpi::OnHeartBeatWarning(int nTimeLapse)
{
    CTP_LOG_INFO( "--->>> " << "OnHeartBeatWarning" << endl
        <<"--->>> nTimerLapse = " << nTimeLapse << endl);
}

void CTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CTP_LOG_INFO( "--->>> " << "OnRspError" << endl);
    IsErrorRspInfo(pRspInfo);
}

bool CTraderSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
    // ���ErrorID != 0, ˵���յ��˴������Ӧ
    bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
    if (bResult)
        CTP_LOG_INFO( "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl);
    return bResult;
}

bool CTraderSpi::IsMyOrder(CThostFtdcOrderField *pOrder)
{
    return ((pOrder->FrontID == g->g_frontId.i) &&
        (pOrder->SessionID == g->g_sessionId.i) &&
        (strcmp(pOrder->OrderRef, g->g_orderRef.c_str()) == 0));
}


bool CTraderSpi::IsMyOrder(CThostFtdcTradeField* pOrder)
{
    return true;
    return (
        //(pOrder->FrontID == g->g_frontId.i) &&
        //(pOrder->SessionID == g->g_sessionId.i) &&
        (strcmp(pOrder->OrderRef, g->g_orderRef.c_str()) == 0));
}

bool CTraderSpi::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
    return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
        (pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
        (pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}

int CTraderSpi::notify( ctp_trader_event_id eid )
{
    ctp_trader_callback_data d;
    d.event_id = eid;
    return notify(d);
}

int CTraderSpi::notify( const ctp_trader_callback_data& td )
{
    g->notify(td);
    return 0;
}

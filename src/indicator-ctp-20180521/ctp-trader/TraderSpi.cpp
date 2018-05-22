#include "StdAfx.h"
#include <windows.h>

#include "../ctp-log/ctp-log.h"
#include "../ctp-market/ctp-criticalsection.h"
#include "../../include/string/utf8_string.h"

#pragma warning(disable : 4996)
using namespace std;


// 流控判断
static bool IsFlowControl(int iResult)
{
    return ((iResult == -2) || (iResult == -3));
}

void CTraderSpi::OnFrontConnected()
{
    CTP_LOG_INFO( "--->>> " << "OnFrontConnected" << endl);
    ///用户登录请求
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
    CTP_LOG_INFO( "--->>> 发送用户登录请求: " << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl);
}

void CTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    std::ostringstream msg;

    msg<< "--->>> " << "OnRspUserLogin" << endl;
    if (bIsLast && !IsErrorRspInfo(pRspInfo))
    {
        // 保存会话参数
        g->g_frontId.i = pRspUserLogin->FrontID;
        g->g_sessionId.i = pRspUserLogin->SessionID;
        int iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
        iNextOrderRef++;
        char s_orderRef[128];
        sprintf(s_orderRef, "%d", iNextOrderRef);
        g->g_orderRef = s_orderRef;
        
        ///获取当前交易日
        g->g_trading_day = g->g_pTraderApi->GetTradingDay();
        msg<< "--->>> 获取当前交易日 = " << g->g_trading_day << endl;

        ctp_trader_callback_data d;
        d.event_id = event_OnRspUserLogin;
        d.trading_day = g->g_trading_day;
        d.session_id = string_format("%X",g->g_sessionId.i);
        notify(d);

        ///投资者结算结果确认
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

    CTP_LOG_INFO( "--->>> 投资者结算结果确认: " << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl);
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
            CTP_LOG_INFO( "--->>> 请求查询合约: "  << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl);
            break;
        }
        else
        {
            CTP_LOG_INFO( "--->>> 请求查询合约: "  << iResult << ", 受到流控" << endl);
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
        ///请求查询账户资金
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
            CTP_LOG_INFO( "--->>> 请求查询资金账户: "  << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl);
            break;
        }
        else
        {
            CTP_LOG_INFO( "--->>> 请求查询资金账户: "  << iResult << ", 受到流控" << endl);
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
        ///请求查询投资者持仓
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
            CTP_LOG_INFO( "--->>> 请求查询投资者持仓: "  << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl);
            break;
        }
        else
        {
            CTP_LOG_INFO( "--->>> 请求查询投资者持仓: "  << iResult << ", 受到流控" << endl);
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
        ///下单：报单录入请求
        //实盘下单: ReqOrderInsert();
    }

    CTP_LOG_INFO(msg.str());
}

///下单：报单录入请求
int CTraderSpi::ReqOrderInsert(const ctp_ReqOrderInsert* r)
{
    ctp_locker l(g->locker);

    CThostFtdcInputOrderField req;
    memset(&req, 0, sizeof(req));
    ///经纪公司代码
    strcpy(req.BrokerID, g->g_brokerId.c_str());
    ///投资者代码
    strcpy(req.InvestorID, g->g_investorId.c_str());
    ///合约代码
    strcpy(req.InstrumentID, r->InstrumentID);

    ///报单引用
    strcpy(req.OrderRef, r->order_ref.c_str());
    ///用户代码
    // TThostFtdcUserIDType UserID;
    ///报单价格条件: 限价
    req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
    ///买卖方向: 
    req.Direction = r->buy?THOST_FTDC_D_Buy:THOST_FTDC_D_Sell;//  g->g_direction;
    ///组合开平标志: 开仓
    req.CombOffsetFlag[0] = r->open? THOST_FTDC_OF_Open : THOST_FTDC_OF_Close;
    //if(r->is_sp())req.CombOffsetFlag[1]=req.CombOffsetFlag[0];
    ///组合投机套保标志
    req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
    //if(r->is_sp())req.CombHedgeFlag[1]=req.CombHedgeFlag[0];

    ///价格
    req.LimitPrice = r->price;
    ///数量: 1
    req.VolumeTotalOriginal = 1;
    ///有效期类型: 当日有效
	//IOC：THOST_FTDC_TC_IOC，立即成交或撤单
	req.TimeCondition = THOST_FTDC_TC_GFD;
    ///GTD日期
    // TThostFtdcDateType GTDDate;
    ///成交量类型: 任何数量
    req.VolumeCondition = THOST_FTDC_VC_AV;
    ///最小成交量: 1
    req.MinVolume = 1;
    ///触发条件: 立即
    req.ContingentCondition = THOST_FTDC_CC_Immediately;
    ///止损价
    // TThostFtdcPriceType StopPrice;
    ///强平原因: 非强平
    req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
    ///自动挂起标志: 否
    req.IsAutoSuspend = 0;
    ///业务单元
    // TThostFtdcBusinessUnitType BusinessUnit;
    ///请求编号
    // TThostFtdcRequestIDType RequestID;
    ///用户强评标志: 否
    req.UserForceClose = 0;

    int iResult = g->g_pTraderApi->ReqOrderInsert(&req, r->request_id.i);

    auto s_open=r->open?"open,":"close,";

    CTP_LOG_INFO( "--->>> 报单录入请求: " 
        << "instrumentID="<<r->InstrumentID
        << "," << s_open
        << ",price="<<r->price
        << ",OrderRef="<<r->order_ref.c_str()
        << ",RequestID="<<r->request_id.i
        << ",result = "<< iResult << ((iResult == 0) ? ", 成功" : ", 失败") 
        << endl);

    return iResult;
}

void CTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//柜台校验出错返回

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

//撤单，根据挂单的 order ref + front id + session id 来撤单
void CTraderSpi::ReqOrderAction(CThostFtdcOrderField *pOrder)
{
    //static bool ORDER_ACTION_SENT = false;  //是否发送了报单
    //if (ORDER_ACTION_SENT)
    //    return;

    CThostFtdcInputOrderActionField req;
    memset(&req, 0, sizeof(req));
    ///经纪公司代码
    strcpy(req.BrokerID, pOrder->BrokerID);
    ///投资者代码
    strcpy(req.InvestorID, pOrder->InvestorID);
    ///报单操作引用
    // TThostFtdcOrderActionRefType OrderActionRef;
    ///报单引用
    strcpy(req.OrderRef, pOrder->OrderRef);
    ///请求编号
    // TThostFtdcRequestIDType RequestID;
    ///前置编号
    req.FrontID = g->g_frontId.i;
    ///会话编号
    req.SessionID = g->g_sessionId.i;
    ///交易所代码
    // TThostFtdcExchangeIDType ExchangeID;
    ///报单编号
    // TThostFtdcOrderSysIDType OrderSysID;
    ///操作标志
    req.ActionFlag = THOST_FTDC_AF_Delete;
    ///价格
    // TThostFtdcPriceType LimitPrice;
    ///数量变化
    // TThostFtdcVolumeType VolumeChange;
    ///用户代码
    // TThostFtdcUserIDType UserID;
    ///合约代码
    strcpy(req.InstrumentID, pOrder->InstrumentID);

    g->IncRequestID();
    int iResult = g->g_pTraderApi->ReqOrderAction(&req, g->g_iRequestID.i);
    CTP_LOG_INFO( "--->>> 撤单操作请求: "  <<
        "order ref="<<pOrder->OrderRef<<", " 
        << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl);
    //ORDER_ACTION_SENT = true;
}

//撤单发生错误的通知：比如，发出撤单的瞬间，报单已经全部成交
void CTraderSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CTP_LOG_INFO( "--->>> " << "OnRspOrderAction" << endl);
    if(IsErrorRspInfo(pRspInfo))
        notify(event_OnRspOrderAction);
}

///报单进入交易所后的通知
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


        //是否正在交易的报单
        if (IsTradingOrder(pOrder))//挂单，未成交状态
        {
            msg<<",IsTradingOrder == true, "<<endl;
            //ReqOrderAction(pOrder);//撤单
        }
        else if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled)
            msg<< ",--->>> 撤单成功"<<endl;

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

///成交通知
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
            <<", 成交"
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
    // 如果ErrorID != 0, 说明收到了错误的响应
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

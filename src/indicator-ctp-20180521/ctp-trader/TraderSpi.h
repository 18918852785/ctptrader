#pragma once
#include "..\CTP636Common\ThostFtdcTraderApi.h"

#include "ctp-trader-global.h"

class CTraderSpi : public CThostFtdcTraderSpi
{
public:
    CTraderSpi(std::shared_ptr<ctp_trader_global> a):g(a){}
    ///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
    virtual void OnFrontConnected();

    ///��¼������Ӧ
    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///Ͷ���߽�����ȷ����Ӧ
    virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///�����ѯ��Լ��Ӧ
    virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///�����ѯ�ʽ��˻���Ӧ
    virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///�����ѯͶ���ֲ߳���Ӧ
    virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///����¼��������Ӧ
    virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///��������������Ӧ
    virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///����Ӧ��
    virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
    virtual void OnFrontDisconnected(int nReason);

    ///������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
    virtual void OnHeartBeatWarning(int nTimeLapse);

    ///����֪ͨ
    virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

    ///�ɽ�֪ͨ
    virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

public:
    int notify(ctp_trader_event_id eid);
    int notify(const ctp_trader_callback_data& td);
//private:
    ///�û���¼����
    void ReqUserLogin();
    ///Ͷ���߽�����ȷ��
    void ReqSettlementInfoConfirm();
    ///�����ѯ��Լ
    void ReqQryInstrument();
    void ReqQryInstrument(const char* instrumentID);
    ///�����ѯ�ʽ��˻�
    void ReqQryTradingAccount();
    ///�����ѯͶ���ֲ߳�
    void ReqQryInvestorPosition();
    void ReqQryInvestorPosition(const char* instrumentID);
    ///����¼������
    int ReqOrderInsert(const ctp_ReqOrderInsert* r);
    ///������������
    void ReqOrderAction(CThostFtdcOrderField *pOrder);

    // �Ƿ��յ��ɹ�����Ӧ
    bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
    // �Ƿ��ҵı����ر�
    bool IsMyOrder(CThostFtdcOrderField *pOrder);
    bool IsMyOrder(CThostFtdcTradeField *p);

    // �Ƿ����ڽ��׵ı���
    bool IsTradingOrder(CThostFtdcOrderField *pOrder);

    std::shared_ptr<ctp_trader_global> g;
};
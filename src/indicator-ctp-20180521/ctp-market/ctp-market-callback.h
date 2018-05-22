#pragma once

#include "ctp-market-export.h"

#include <memory>
#include <functional>

class ctp_market_global;
typedef std::shared_ptr<ctp_market_global> ctp_market_global_ptr;

struct CThostFtdcDepthMarketDataField;

//����۸�: �ص�����
class CTPMARKET_API ctp_market_callback_data{
public:
    ctp_market_callback_data():InstrumentID(""),
        UpdateTime(""),
        UpdateMillisec(0),
        Volume(0),
        BuyPrice1(0),
        SellPrice1(0){}

    char * InstrumentID;//�ڻ�����: M1901
    char * UpdateTime;  //ʱ��
    int UpdateMillisec; //����
    int Volume;         //�ɽ���
    float BuyPrice1,SellPrice1;//��һ��,��һ��

    CThostFtdcDepthMarketDataField* ctp_p;//ctp�ڲ����ݽṹ
};
typedef std::function<void(ctp_market_callback_data*)> ctp_market_callback;

#pragma once
#include "ctp-trader-export.h"
#include "ctp-trader-callback.h"

class ctp_ReqOrderInsert;

// �����Ǵ� ctp-market.dll ������
class CTPMARKET_API ctp_trader {
public:
	ctp_trader(void);


    int set_codes(const char* code);
    int set_codes(int count, const char** codes);

    void set_trader_callback(ctp_trader_callback c);

    int start();
    ctp_trader_global_ptr g;


    ///�����ѯ��Լ
    int ReqQryInstrument();
    int ReqOrderInsert(const ctp_ReqOrderInsert* r);//�µ�
};


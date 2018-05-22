#pragma once
#include "ctp-market-export.h"
#include "ctp-market-callback.h"


// �����Ǵ� ctp-market.dll ������
class CTPMARKET_API ctp_market {
public:
	ctp_market(void);



    int set_codes(const char* code);
    int set_codes(int count, const char** codes);

    void set_market_callback( ctp_market_callback c);

	int set_price(const char* code, double buy, double sell);
	int get_price(const char* code, double& buy, double &sell);
	int get_spread(double & spread1, double& spread2);

	int start();
    ctp_market_global_ptr g;
};


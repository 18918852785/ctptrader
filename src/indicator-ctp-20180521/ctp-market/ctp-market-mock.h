#pragma once
#include "ctp-market-export.h"
#include "ctp-market-callback.h"
#include "ctp-int.h"
#include <unordered_map>

namespace thread
{
    struct ithread;
    typedef std::shared_ptr<ithread> ithread_ptr;
}
// 此类是从 ctp-market.dll 导出的
class CTPMARKET_API ctp_market_mock {
public:
	ctp_market_mock(void);



    int set_codes(const char* code);
    int set_codes(int count, const char** codes);

    void set_market_callback( ctp_market_callback c);

	int set_price(const char* code, double buy, double sell);
	int get_price(const char* code, double& buy, double &sell);
	int get_spread(double & spread1, double& spread2);

	int start();

    ::thread::ithread_ptr g_market_thread;

    ctp_market_callback on_market_data ;
    std::unordered_map<std::string, ctp_price> g_prices;

    int parse_line(const std::string& line);
};


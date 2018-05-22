#pragma once
#include "ctp-trader-export.h"


// 此类是从 ctp-market.dll 导出的
class CTPMARKET_API ctp_order_book{
public:
	ctp_order_book(void);
    virtual ~ctp_order_book(){}

public:
    //static int set_order(const std::string& order_ref,)
};


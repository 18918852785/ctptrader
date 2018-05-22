// ctp-market.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "ctp-market-mock.h"

#include "../../include/string/utf8_string.h"
#include "../../include/config/bs_config.h"
#include "../gtest/flags.h"

#include <assert.h>

DEFINE_FLAG(charp, mock_path, "s:\\pricediff_m1901_m1809\\0420.csv", "mock数据文件路径");

ctp_market_mock::ctp_market_mock()
{
}


int ctp_market_mock::set_codes( const char* code )
{
    return 0;
}

int ctp_market_mock::set_codes( int count, const char** codes )
{
    return 0;
}

int ctp_market_mock::start()
{
	dpu::Flags::Init();

    auto action = [&](){
        //s:\pricediff_m1901_m1809\0420.csv
        auto path = FLAG_mock_path;
        auto f = fopen(path,"rt");
        assert(f);
        if(f){
            std::string line;
            cosmos::bs_config::read_file_line(f,line);//第一行是标题
            while( 0 == cosmos::bs_config::read_file_line(f,line))
            {
                parse_line(line);//解析文本
                line.clear();
            }
            
            fclose(f);
        }
    };

    g_market_thread.reset(new ::thread::task_thread());
    g_market_thread->start();
    g_market_thread->add_task(action);
    //action();
    return 0;
}

static bool check_price(int i){
    return i>2000 & i <4000;
}

int ctp_market_mock::parse_line( const std::string& line )
{
    //20180419 21:00:00.004266,20180420,21:00:00,861,3238,286,3238,114,3239,1,3212,8856,3211,388,3212,14

    auto fields = string_split(line,",");
    if(fields.size()<16)return -1;
    auto m1901_bid = atoi(fields[6].c_str());
    auto m1901_ask = atoi(fields[8].c_str());
    auto m1809_bid = atoi(fields[12].c_str());
    auto m1809_ask = atoi(fields[14].c_str());
    if( ! check_price(m1901_bid) ||
        ! check_price(m1901_ask) ||
        ! check_price(m1809_bid) ||
        ! check_price(m1809_ask) ){
            assert(false);
            return -1;
    }

    {
        ctp_market_callback_data d;
        d.InstrumentID = "m1901";
        d.UpdateTime = (char*)fields[0].c_str();
        d.BuyPrice1 = m1901_bid;
        d.SellPrice1 = m1901_ask;
        on_market_data(&d);//数据改变通知
    }
    {
        ctp_market_callback_data d;
        d.InstrumentID = "m1809";
        d.UpdateTime = (char*)fields[0].c_str();
        d.BuyPrice1 = m1809_bid;
        d.SellPrice1 = m1809_ask;
        on_market_data(&d);//数据改变通知
    }
    return 0;
}

void ctp_market_mock::set_market_callback( ctp_market_callback c )
{
    on_market_data = c;
}

int ctp_market_mock::set_price(const char* code, double buy, double sell)
{
	ctp_price p(buy,sell);
	g_prices[code]=p;
	return 0;
}

int ctp_market_mock::get_price(const char* code, double& buy, double & sel)
{
	buy=sel=0;
	auto i = g_prices.find(code);
	if(i==g_prices.end())
		return -1;

	buy = i->second.buy;
	sel = i->second.sell;
	if(buy<2 || sel <2)
		return -1;
	return 0;
}

int ctp_market_mock::get_spread(double & spread1, double &spread2)
{
	spread1 = spread2=0;
	double buy1, sell1, buy2, sell2;
	auto ret = get_price( "m1809",buy1,sell1);
	if(ret!=0) return ret;
	
	ret = get_price( "m1901",buy2,sell2);
	if(ret!=0) return ret;

	spread1 = -sell2+buy1;
	spread2 = -buy2 + sell1;
	return ret;
}

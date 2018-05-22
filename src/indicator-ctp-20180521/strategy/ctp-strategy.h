#pragma once
#include <functional>

class ctp_ReqOrderInsert;
#define  ctp_order ctp_ReqOrderInsert
//class ctp_order{
//public:
//    ctp_order():code(""),buy(false),open(false),price(0),s_time("")
//    {
//
//    }
//    const char* code;
//    bool buy;
//    bool open;
//    int price;
//    const char* s_time;
//};

typedef     std::function<int(ctp_order* o)>  ctp_order_func;


#include <string>
#include <vector>


///仓位
class ctp_strategy_position{
public:
    ctp_strategy_position():buy(false),open(false),price(0),
        changes(0),
        order_error_id(0),
        status(-1),traded(0){}
    bool buy;
    bool open;
    int price;
    
    int changes;//改变了几次
    std::string order_ref;
    int order_error_id;
    char status;
    std::string order_sys_id;
    int traded;//成交

    std::string trading_day;
    std::string session_id;
    std::string time;
    


    static std::shared_ptr<ctp_strategy_position> parse(const std::string& s);

    std::string to_string()const;
};


///仓位
class ctp_strategy_positions{
public:
    std::vector<ctp_strategy_position> m_positions;
};

class ctp_trader_callback_data;
class ctp_strategy {

public:
    static bool is_trade_time(const char* s_time);//交易时间段

public:
	ctp_strategy(void);
    virtual ~ctp_strategy();

    virtual void set_id(const std::string& i){m_id=i;}
    virtual void set_trading_day(const std::string& i,const std::string& sid){m_trading_day=i;m_session_id=sid;}
    virtual void set_trade_code(const std::string& i){m_trade_code=i;}
    virtual void set_market_codes(const std::string& i);
    virtual void set_on_order(ctp_order_func f){ on_order = f;}

    virtual int start();

    virtual int on_spread_change(const char* instrumentID, int spread_buy, int spread_sell);
    virtual int on_notify(const ctp_trader_callback_data* d);

    ctp_strategy_positions m_positions;

    int get_last_position(ctp_strategy_position* p, bool* has_postion);


    int do_order(ctp_order* o);
    int do_order(const char* code,bool buy, bool open, int price,const char*s_time);

    static std::string get_time();

protected:
    std::string get_trade_code()const {return m_trade_code;}
    bool is_my_market_code(const std::string& c)const;

    int save_position();

    std::string m_id;
    std::string m_trading_day;
    std::string m_session_id;
private:
    std::string get_position_path();

    int load_position();

    ctp_order_func on_order;
    std::vector<std::string> m_market_codes;
    std::string m_trade_code;
};


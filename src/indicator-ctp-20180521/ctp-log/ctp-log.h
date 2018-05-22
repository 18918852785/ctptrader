#pragma once
#include <sstream>

// 此类是从 ctp-market.dll 导出的
class ctp_log {
public:
	ctp_log(void);

    static void init(const char* module_name);
    
    static int log_vprint(char* str, size_t size, const char* format, va_list args);

    static int log_vprint2(char* str, size_t size, const char* format, ...);
    
    static int log_print(const char* format, ...);
    
    static int log_print(int level, const char* format, ...);

    static std::shared_ptr<ctp_log> g_log;
};

#define CTP_LOG_INIT(n)  ctp_log::init(n)

#define CTP_PRINT      ctp_log::log_print


//////////////////////////////////////////////////////////////////////////
#define CTP_LOG_DEBUG(_items)	\
{								\
    std::ostringstream os;		\
    os << _items;				\
    std::string str = os.str();	\
    CTP_PRINT(4/*indicator_log_debug*/,str.c_str());\
}
#define CTP_LOG_INFO(_items)	\
{								\
    std::ostringstream os;		\
    os << _items;				\
    std::string str = os.str();	\
    CTP_PRINT(1/*indicator_log_always*/,str.c_str());\
}

#define CTP_LOG_ERROR(_items)	\
{								\
    std::ostringstream os;		\
    os << _items;				\
    std::string str = os.str();	\
    CTP_PRINT(2/*indicator_log_error*/,str.c_str());\
}

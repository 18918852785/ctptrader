// ctp-market.cpp : 定义 DLL 应用程序的导出函数。
//

#include <windows.h>
#include <shlwapi.h>
#include <assert.h>

#include "../../include/string/utf8_string.h"
#include "ctp-log.h"

#include "../../include/dll_loader/dll_loader.h"

//#include "../../indicator-config/header/indicator-config.h"

//#include "../../indicator-log/header/indicator-log.h"

#define va_copy(dst, src) (memmove(&(dst), &(src), sizeof(dst)))
#define TO_CONSOLE 1
std::shared_ptr<ctp_log> ctp_log::g_log;
static void* module_id=nullptr;

ctp_log::ctp_log()
{
}

void ctp_log::init( const char* module_name )
{
    if(g_log)return;

    g_log = std::make_shared<ctp_log>();
    //INDICATOR_LOG_INIT();
    module_id = 0;
    //if(logger)//load nanomsg.dll failed
    //    module_id = logger->create_logger(module_name);
}

int ctp_log::log_vprint( char* str, size_t size, const char* format, va_list args )
{
    if (str == NULL || size == 0) {
        int retval = _vscprintf(format, args);
        return retval;
    }
    va_list args_copy;
    va_copy(args_copy, args);
    int written = _vsnprintf(str, size, format, args_copy);
    va_end(args_copy);
    if (written < 0) {
        // _vsnprintf returns -1 if the number of characters to be written is
        // larger than 'size', so we call _vscprintf which returns the number
        // of characters that would have been written.
        va_list args_retry;
        va_copy(args_retry, args);
        written = _vscprintf(format, args_retry);
        va_end(args_retry);
    }
    // Make sure to zero-terminate the string if the output was
    // truncated or if there was an error.
    // The static cast is safe here as we have already determined that 'written'
    // is >= 0.
    if (static_cast<size_t>(written) >= size) {
        str[size - 1] = '\0';
    }
    return written;
}

int ctp_log::log_vprint2( char* str, size_t size, const char* format, ... )
{
    va_list args;
    va_start(args, format);
    int retval = log_vprint(str, size, format, args);
    va_end(args);
    return retval;
}

int ctp_log::log_print( const char* format, ... )
{
    char buf[2048]={0};

    va_list args;
    va_start(args, format);
    auto retval = log_vprint(buf,sizeof(buf), format, args);
    va_end(args);
    //
    if(!g_log){ assert(false);return -1;}
    CTP_LOG_DEBUG(buf);
    return retval;
}

static void remove_crln(char* msg){
    auto i = strlen(msg);
    while(i >=1 ){
        if( msg[i-1] == '\n' || 
            msg[i-1] == '\r')
            msg[i-1] = 0;
        else
            break;
        i--;
    }
}

int ctp_log::log_print( int level, const char* format, ... )
{
    char buf[2048]={0};

    va_list args;
    va_start(args, format);
    auto retval = log_vprint(buf,sizeof(buf), format, args);
    va_end(args);


    if(!g_log){ assert(false);return -1;}


    //控制台打印
    if(TO_CONSOLE)printf(buf);

    //删除日志后面多余的回车
    remove_crln(buf);

    //写入日志文件
    //INDICATOR_LOG(module_id, (indicator_log_level)level,buf);
    return 0;
}


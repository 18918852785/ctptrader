#include "print.h"

#include "../string/utf8_string.h"

#if CONSOLE_PRINTF == 1
#include "bs_log.h"
#include "bs_config.h"
#endif

#ifdef _WIN32
#include <windows.h>

namespace cosmos{

#if CONSOLE_PRINTF == 1
	bs_config::log_config      console_config;
	bool console_config_inited = false;
#endif

	void my_printf(const char*fmt,...)
	{
#if CONSOLE_PRINTF == 1
		if (!console_config_inited)
			return;
		if( !console_config.m_debug_enabled &&
			!console_config.m_info_enabled &&
			!console_config.m_error_enabled )
		{
			return;
		}
#endif

		va_list arg = (va_list)((char*)(&fmt) + sizeof(const char*)); 
		//多线程调用 printf,会崩溃
		vprintf(fmt, arg);
	}

#define fg_blue FOREGROUND_BLUE
#define fg_red FOREGROUND_RED
#define fg_green FOREGROUND_GREEN
#define fg_intensity FOREGROUND_INTENSITY


	void my_printf_set_color(int color) {
		color |= FOREGROUND_INTENSITY;
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);;
		SetConsoleTextAttribute(hConsole, color);
	}

	void my_printf_set_color_red() {
		my_printf_set_color(fg_red);
	}

	void my_printf_set_color_black() {
		my_printf_set_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // default color
	}

#define printfc(color,m,...); \
    my_printf_set_color(color); \
    printf(m, __VA_ARGS__); \
    set_std_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // default color

}

#else
#include <stdio.h> 
#include <stdarg.h> 

namespace cosmos {

	void my_printf(const char*fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		vfprintf(stderr, fmt, args);
		va_end(args);
	}
}


#endif

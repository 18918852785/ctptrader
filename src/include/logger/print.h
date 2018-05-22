#pragma once

#define PRINTF cosmos::my_printf

namespace cosmos{
	void my_printf(const char*fmt,...);
	void my_printf_set_color_red();
	void my_printf_set_color_black();
}


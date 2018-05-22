#pragma once

#ifdef CTPMARKET_EXPORTS
#define CTPMARKET_API __declspec(dllexport)
#else
#define CTPMARKET_API __declspec(dllimport)
#endif

#pragma once

#ifdef _WIN32
	#if !defined (_CHAR16T)
	#define _CHAR16T
#if _MSC_VER < 1900 
	typedef wchar_t char16_t;
#endif
#if _MSC_VER < 1900 
	typedef unsigned int char32_t;
#endif
	#endif
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#define _T16(x)	 L ## x
	#define CP_936  936
#else//if defined(XP_MACOSX)
	#include <string>
	#include <string.h>
	#include <iconv.h>
	#define STRUTILS_MAX_BUFFER_SIZE    2*1024
	#define _T16(x)	u ## x
#endif

namespace cosmos {

	class string_converter
	{
	private:
		string_converter();
		~string_converter();

	public:
		//
		// SUCCESS: return 0, FAILURE: return !0
		//
		static inline int utf8_to_utf16(const char* from_u8, char16_t** to_u16)
		{
			if (!from_u8 || !to_u16)
				return -1;

			int to_u16_len = 0;
#ifdef _WIN32
			to_u16_len = MultiByteToWideChar(CP_UTF8, 0, from_u8, strlen(from_u8), NULL, 0);
			if (0 == to_u16_len || nullptr == (*to_u16 = new char16_t[to_u16_len + 1]))
				return -1;

			if (0 == MultiByteToWideChar(CP_UTF8, 0, from_u8, -1, (LPWSTR)*to_u16, to_u16_len + 1))
			{
				delete *to_u16;
				*to_u16 = nullptr;
				return -1;
			}
			(*to_u16)[to_u16_len] = 0;
#else
			*to_u16 = nullptr;
			char16_t tmp_buf_out16[STRUTILS_MAX_BUFFER_SIZE] = { 0 };
			char16_t* out16 = tmp_buf_out16;
			size_t out16_len = STRUTILS_MAX_BUFFER_SIZE * sizeof(char16_t);
			size_t in_len = strlen(from_u8);

			iconv_t cd;
			cd = iconv_open("UCS-2", "UTF-8");
			if ((iconv_t)-1 == cd)
			{
				return -1;
			}

			size_t ret_size = iconv(cd, (char**)&from_u8, (size_t*)&in_len, (char**)&out16, (size_t*)&out16_len);
			iconv_close(cd);

			if ((size_t)(-1) == ret_size)
			{
				return -1;
			}

			to_u16_len = STRUTILS_MAX_BUFFER_SIZE - out16_len / sizeof(char16_t);
			*to_u16 = new char16_t[to_u16_len + 1];
			if (nullptr == *to_u16)	return -1;
			memcpy(*to_u16, tmp_buf_out16, to_u16_len * sizeof(char16_t));
			(*to_u16)[to_u16_len] = 0;
#endif
			return to_u16_len;
		}

		static int utf16_to_utf8(const char16_t* from_u16, char** to_u8)
		{
			if (!from_u16 || !to_u8)
				return -1;

			int to_u8_len = 0;
#ifdef _WIN32
			to_u8_len = WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)from_u16, wcslen((wchar_t*)from_u16), nullptr, 0, nullptr, nullptr);
			if (0 == to_u8_len || nullptr == (*to_u8 = new char[to_u8_len + 1]))
				return -1;

			if (0 == WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)from_u16, -1, *to_u8, to_u8_len + 1, nullptr, nullptr))
			{
				int err = GetLastError();
				delete *to_u8;
				*to_u8 = nullptr;
				return -1;
			}
			(*to_u8)[to_u8_len] = 0;
#else
			*to_u8 = nullptr;
			char tmp_buf_out8[STRUTILS_MAX_BUFFER_SIZE] = { 0 };
			char* out8 = tmp_buf_out8;
			size_t out_len = STRUTILS_MAX_BUFFER_SIZE * sizeof(char);
			size_t in_len = u16strlen(from_u16) * sizeof(char16_t);

			iconv_t cd;
			cd = iconv_open("UTF-8", "UCS-2");
			if ((iconv_t)-1 == cd)
			{
				return -1;
			}

			size_t ret_size = iconv(cd, (char**)&from_u16, (size_t*)&in_len, (char**)&out8, (size_t*)&out_len);
			iconv_close(cd);

			if ((size_t)(-1) == ret_size)
			{
				return -1;
			}

			to_u8_len = STRUTILS_MAX_BUFFER_SIZE - out_len / sizeof(char);
			*to_u8 = new char[to_u8_len + 1];
			if (nullptr == *to_u8)	return -1;
			memcpy(*to_u8, tmp_buf_out8, to_u8_len * sizeof(char));
			(*to_u8)[to_u8_len] = 0;
#endif
			return to_u8_len;
		}

		static inline int utf8_to_wchar(const char* from_u8, wchar_t** to_u16)
		{
			if (!from_u8 || !to_u16)
				return -1;

			int to_u16_len = 0;
#ifdef _WIN32
			to_u16_len = MultiByteToWideChar(CP_UTF8, 0, from_u8, strlen(from_u8), NULL, 0);
			if (0 == to_u16_len || nullptr == (*to_u16 = new wchar_t[to_u16_len + 1]))
				return -1;

			if (0 == MultiByteToWideChar(CP_UTF8, 0, from_u8, -1, *to_u16, to_u16_len + 1))
			{
				delete *to_u16;
				*to_u16 = nullptr;
				return -1;
			}
			(*to_u16)[to_u16_len] = 0;
#else
			*to_u16 = nullptr;
			wchar_t tmp_buf_out16[STRUTILS_MAX_BUFFER_SIZE] = { 0 };
			wchar_t* out16 = tmp_buf_out16;
			size_t out16_len = STRUTILS_MAX_BUFFER_SIZE * sizeof(wchar_t);
			size_t in_len = strlen(from_u8);

			iconv_t cd;
			cd = iconv_open("UCS-4", "UTF-8");
			if ((iconv_t)-1 == cd)
			{
				return -1;
			}

			size_t ret_size = iconv(cd, (char**)&from_u8, (size_t*)&in_len, (char**)&out16, (size_t*)&out16_len);
			iconv_close(cd);

			if ((size_t)(-1) == ret_size)
			{
				return -1;
			}

			to_u16_len = STRUTILS_MAX_BUFFER_SIZE - out16_len / sizeof(wchar_t);
			*to_u16 = new wchar_t[to_u16_len + 1];
			if (nullptr == *to_u16)	return -1;
			memcpy(*to_u16, tmp_buf_out16, to_u16_len * sizeof(wchar_t));
			(*to_u16)[to_u16_len] = 0;
#endif
			return to_u16_len;
		}

		static int wchar_to_utf8(const wchar_t* from_u16, char** to_u8)
		{
			if (!from_u16 || !to_u8)
				return -1;

			int to_u8_len = 0;
#ifdef _WIN32
			to_u8_len = WideCharToMultiByte(CP_UTF8, 0, from_u16, wcslen(from_u16), nullptr, 0, nullptr, nullptr);
			if (0 == to_u8_len || nullptr == (*to_u8 = new char[to_u8_len + 1]))
				return -1;

			if (0 == WideCharToMultiByte(CP_UTF8, 0, from_u16, -1, *to_u8, to_u8_len + 1, nullptr, nullptr))
			{
				int err = GetLastError();
				delete *to_u8;
				*to_u8 = nullptr;
				return -1;
			}
			(*to_u8)[to_u8_len] = 0;
#else
			*to_u8 = nullptr;
			char tmp_buf_out8[STRUTILS_MAX_BUFFER_SIZE] = { 0 };
			char* out8 = tmp_buf_out8;
			size_t out_len = STRUTILS_MAX_BUFFER_SIZE * sizeof(char);
			size_t in_len = wcslen(from_u16) * sizeof(wchar_t);

			iconv_t cd;
			cd = iconv_open("UTF-8", "UCS-4");
			if ((iconv_t)-1 == cd)
			{
				return -1;
			}

			size_t ret_size = iconv(cd, (char**)&from_u16, (size_t*)&in_len, (char**)&out8, (size_t*)&out_len);
			iconv_close(cd);

			if ((size_t)(-1) == ret_size)
			{
				return -1;
			}

			to_u8_len = STRUTILS_MAX_BUFFER_SIZE - out_len / sizeof(char);
			*to_u8 = new char[to_u8_len + 1];
			if (nullptr == *to_u8)	return -1;
			memcpy(*to_u8, tmp_buf_out8, to_u8_len * sizeof(char));
			(*to_u8)[to_u8_len] = 0;
#endif
			return to_u8_len;
		}

#ifndef _WIN32
		static int wchar_to_utf16(const wchar_t* from_wchar, char16_t** to_u16)
		{
			if (!from_wchar || !to_u16)
				return -1;

			*to_u16 = nullptr;
			char16_t tmp_buf_out16[STRUTILS_MAX_BUFFER_SIZE] = { 0 };
			char16_t* out16 = tmp_buf_out16;
			size_t out_len = STRUTILS_MAX_BUFFER_SIZE * sizeof(char16_t);
			size_t in_len = wcslen(from_wchar) * sizeof(wchar_t);

			iconv_t cd;
			cd = iconv_open("UCS-2", "UCS-4");
			if ((iconv_t)-1 == cd)
			{
				return -1;
			}

			size_t ret_size = iconv(cd, (char**)&from_wchar, (size_t*)&in_len, (char**)&out16, (size_t*)&out_len);
			iconv_close(cd);

			if ((size_t)(-1) == ret_size)
			{
				return -1;
			}

			size_t to_u16_len = STRUTILS_MAX_BUFFER_SIZE - out_len / sizeof(char16_t);
			*to_u16 = new char16_t[to_u16_len + 1];
			if (nullptr == *to_u16)	return -1;
			memcpy(*to_u16, tmp_buf_out16, to_u16_len * sizeof(char16_t));
			(*to_u16)[to_u16_len] = 0;
			return to_u16_len;
		}

		static int utf16_to_wchar(const char16_t* from_u16, wchar_t** to_wchar)
		{
			if (!from_u16 || !to_wchar)
				return -1;

			*to_wchar = nullptr;
			wchar_t tmp_buf_out32[STRUTILS_MAX_BUFFER_SIZE] = { 0 };
			wchar_t* out32 = tmp_buf_out32;
			size_t out_len = STRUTILS_MAX_BUFFER_SIZE * sizeof(wchar_t);
			size_t in_len = u16strlen(from_u16) * sizeof(char16_t);

			iconv_t cd;
			cd = iconv_open("UCS-4", "UCS-2");
			if ((iconv_t)-1 == cd)
			{
				return -1;
			}

			size_t ret_size = iconv(cd, (char**)&from_u16, (size_t*)&in_len, (char**)&out32, (size_t*)&out_len);
			iconv_close(cd);

			if ((size_t)(-1) == ret_size)
			{
				return -1;
			}

			size_t to_u32_len = STRUTILS_MAX_BUFFER_SIZE - out_len / sizeof(wchar_t);
			*to_wchar = new wchar_t[to_u32_len + 1];
			if (nullptr == *to_wchar)	return -1;
			memcpy(*to_wchar, tmp_buf_out32, to_u32_len * sizeof(wchar_t));
			(*to_wchar)[to_u32_len] = 0;
			return to_u32_len;
		}
#endif

		static int gbk_to_utf16(const char* from_gbk, char16_t** to_u16)
		{
			if (!from_gbk || !to_u16)
				return -1;

			int to_u16_len = 0;
#ifdef _WIN32
			to_u16_len = MultiByteToWideChar(CP_936, 0, from_gbk, strlen(from_gbk), NULL, 0);
			if (0 == to_u16_len || nullptr == (*to_u16 = (char16_t*)new wchar_t[to_u16_len + 1]))
				return -1;

			if (0 == MultiByteToWideChar(CP_936, 0, from_gbk, -1, (LPWSTR)*to_u16, to_u16_len + 1))
			{
				delete *to_u16;
				*to_u16 = nullptr;
				return -1;
			}
			(*to_u16)[to_u16_len] = 0;
#else
			*to_u16 = nullptr;
			char16_t tmp_buf_out16[STRUTILS_MAX_BUFFER_SIZE] = { 0 };
			char16_t* out16 = tmp_buf_out16;
			size_t out16_len = STRUTILS_MAX_BUFFER_SIZE * sizeof(char16_t);
			size_t in_len = strlen(from_gbk);

			iconv_t cd;
			cd = iconv_open("UCS-2", "GBK");
			if ((iconv_t)-1 == cd)
			{
				return -1;
			}

			size_t ret_size = iconv(cd, (char**)&from_gbk, (size_t*)&in_len, (char**)&out16, (size_t*)&out16_len);
			iconv_close(cd);

			if ((size_t)(-1) == ret_size)
			{
				return -1;
			}

			to_u16_len = STRUTILS_MAX_BUFFER_SIZE - out16_len / sizeof(char16_t);
			*to_u16 = new char16_t[to_u16_len + 1];
			if (nullptr == *to_u16)	return -1;
			memcpy(*to_u16, tmp_buf_out16, to_u16_len * sizeof(char16_t));
			(*to_u16)[to_u16_len] = 0;
#endif
			return to_u16_len;
		}

		static int utf16_to_gbk(const char16_t* from_u16, char** to_gbk)
		{
			if (!from_u16 || !to_gbk)
				return -1;

			int to_gbk_len = 0;
#ifdef _WIN32
			to_gbk_len = WideCharToMultiByte(CP_936, 0, (wchar_t*)from_u16, wcslen((wchar_t*)from_u16), nullptr, 0, nullptr, nullptr);
			if (0 == to_gbk_len || nullptr == (*to_gbk = new char[to_gbk_len + 1]))
				return -1;

			if (0 == WideCharToMultiByte(CP_936, 0, (wchar_t*)from_u16, -1, *to_gbk, to_gbk_len + 1, nullptr, nullptr))
			{
				delete *to_gbk;
				*to_gbk = nullptr;
				return -1;
			}
			(*to_gbk)[to_gbk_len] = 0;
#else
			*to_gbk = nullptr;
			char tmp_buf_outgbk[STRUTILS_MAX_BUFFER_SIZE] = { 0 };
			char* outgbk = tmp_buf_outgbk;
			size_t out_len = STRUTILS_MAX_BUFFER_SIZE * sizeof(char);
			size_t in_len = u16strlen(from_u16) * sizeof(char16_t);

			iconv_t cd;
			cd = iconv_open("GBK", "UCS-2");
			if ((iconv_t)-1 == cd)
			{
				return -1;
			}

			size_t ret_size = iconv(cd, (char**)&from_u16, (size_t*)&in_len, (char**)&outgbk, (size_t*)&out_len);
			iconv_close(cd);

			if ((size_t)(-1) == ret_size)
			{
				return -1;
			}

			to_gbk_len = STRUTILS_MAX_BUFFER_SIZE - out_len / sizeof(char);
			*to_gbk = new char[to_gbk_len + 1];
			if (nullptr == *to_gbk)	return -1;
			memcpy(*to_gbk, tmp_buf_outgbk, to_gbk_len * sizeof(char));
			(*to_gbk)[to_gbk_len] = 0;
#endif
			return to_gbk_len;
		}

		static int gbk_to_utf8(const char* from_gbk, char** to_u8)
		{
			if (!from_gbk || !to_u8)
				return -1;
			int to_u8_len = 0;
#ifdef _WIN32
			char16_t* tmp_to_u16;
			to_u8_len = gbk_to_utf16(from_gbk, &tmp_to_u16);
			if (to_u8_len <= 0)
				return -1;
			to_u8_len = utf16_to_utf8(tmp_to_u16, to_u8);
			if (to_u8_len <= 0)
			{
				free_buf(tmp_to_u16);
				return -1;
			}
			free_buf(tmp_to_u16);
#else
			*to_u8 = nullptr;
			char tmp_buf_out8[STRUTILS_MAX_BUFFER_SIZE] = { 0 };
			char* out8 = tmp_buf_out8;
			size_t out8_len = STRUTILS_MAX_BUFFER_SIZE * sizeof(char);
			size_t in_len = strlen(from_gbk);

			iconv_t cd;
			cd = iconv_open("UTF-8", "GBK");
			if ((iconv_t)-1 == cd)
			{
				return -1;
			}

			size_t ret_size = iconv(cd, (char**)&from_gbk, (size_t*)&in_len, (char**)&out8, (size_t*)&out8_len);
			iconv_close(cd);

			if ((size_t)(-1) == ret_size)
			{
				return -1;
			}

			to_u8_len = STRUTILS_MAX_BUFFER_SIZE - out8_len / sizeof(char);
			*to_u8 = new char[to_u8_len + 1];
			if (nullptr == *to_u8)	return -1;
			memcpy(*to_u8, tmp_buf_out8, to_u8_len * sizeof(char));
			(*to_u8)[to_u8_len] = 0;
#endif
			return to_u8_len;
		}

		static int utf8_to_gbk(const char* from_u8, char** to_gbk)
		{
			if (!from_u8 || !to_gbk)
				return -1;
			int to_gbk_len = 0;
#ifdef _WIN32
			char16_t* tmp_to_u16;
			to_gbk_len = utf8_to_utf16(from_u8, &tmp_to_u16);
			if (to_gbk_len <= 0)
				return -1;
			to_gbk_len = utf16_to_gbk(tmp_to_u16, to_gbk);
			if (to_gbk_len <= 0)
			{
				free_buf(tmp_to_u16);
				return -1;
			}
			free_buf(tmp_to_u16);
#else
			*to_gbk = nullptr;
			char tmp_buf_outgbk[STRUTILS_MAX_BUFFER_SIZE] = { 0 };
			char* outgbk = tmp_buf_outgbk;
			size_t outgbk_len = STRUTILS_MAX_BUFFER_SIZE * sizeof(char);
			size_t in_len = strlen(from_u8);

			iconv_t cd;
			cd = iconv_open("GBK", "UTF-8");
			if ((iconv_t)-1 == cd)
			{
				return -1;
			}

			size_t ret_size = iconv(cd, (char**)&from_u8, (size_t*)&in_len, (char**)&outgbk, (size_t*)&outgbk_len);
			iconv_close(cd);

			if ((size_t)(-1) == ret_size)
			{
				return -1;
			}

			to_gbk_len = STRUTILS_MAX_BUFFER_SIZE - outgbk_len / sizeof(char);
			*to_gbk = new char[to_gbk_len + 1];
			if (nullptr == *to_gbk)	return -1;
			memcpy(*to_gbk, tmp_buf_outgbk, to_gbk_len * sizeof(char));
			(*to_gbk)[to_gbk_len] = 0;
#endif
			return to_gbk_len;
		}

		static inline int gbk_to_wchar(const char* from_gbk, wchar_t** to_u16)
		{
			if (!from_gbk || !to_u16)
				return -1;

			int to_u16_len = 0;
#ifdef _WIN32
			to_u16_len = MultiByteToWideChar(CP_936, 0, from_gbk, strlen(from_gbk), NULL, 0);
			if (0 == to_u16_len || nullptr == (*to_u16 = new wchar_t[to_u16_len + 1]))
				return -1;

			if (0 == MultiByteToWideChar(CP_936, 0, from_gbk, -1, *to_u16, to_u16_len + 1))
			{
				delete *to_u16;
				*to_u16 = nullptr;
				return -1;
			}
			(*to_u16)[to_u16_len] = 0;
#else
			*to_u16 = nullptr;
			wchar_t tmp_buf_out16[STRUTILS_MAX_BUFFER_SIZE] = { 0 };
			wchar_t* out16 = tmp_buf_out16;
			size_t out16_len = STRUTILS_MAX_BUFFER_SIZE * sizeof(wchar_t);
			size_t in_len = strlen(from_gbk);

			iconv_t cd;
			cd = iconv_open("UCS-4", "GBK");
			if ((iconv_t)-1 == cd)
			{
				return -1;
			}

			size_t ret_size = iconv(cd, (char**)&from_gbk, (size_t*)&in_len, (char**)&out16, (size_t*)&out16_len);
			iconv_close(cd);

			if ((size_t)(-1) == ret_size)
			{
				return -1;
			}

			to_u16_len = STRUTILS_MAX_BUFFER_SIZE - out16_len / sizeof(wchar_t);
			*to_u16 = new wchar_t[to_u16_len + 1];
			if (nullptr == *to_u16)	return -1;
			memcpy(*to_u16, tmp_buf_out16, to_u16_len * sizeof(wchar_t));
			(*to_u16)[to_u16_len] = 0;
#endif
			return to_u16_len;
		}

		static int wchar_to_gbk(const wchar_t* from_u16, char** to_gbk)
		{
			if (!from_u16 || !to_gbk)
				return -1;

			int to_gbk_len = 0;
#ifdef _WIN32
			to_gbk_len = WideCharToMultiByte(CP_936, 0, from_u16, wcslen(from_u16), nullptr, 0, nullptr, nullptr);
			if (0 == to_gbk_len || nullptr == (*to_gbk = new char[to_gbk_len + 1]))
				return -1;

			if (0 == WideCharToMultiByte(CP_936, 0, from_u16, -1, *to_gbk, to_gbk_len + 1, nullptr, nullptr))
			{
				int err = GetLastError();
				delete *to_gbk;
				*to_gbk = nullptr;
				return -1;
			}
			(*to_gbk)[to_gbk_len] = 0;
#else
			*to_gbk = nullptr;
			char tmp_buf_out8[STRUTILS_MAX_BUFFER_SIZE] = { 0 };
			char* out8 = tmp_buf_out8;
			size_t out_len = STRUTILS_MAX_BUFFER_SIZE * sizeof(char);
			size_t in_len = wcslen(from_u16) * sizeof(wchar_t);

			iconv_t cd;
			cd = iconv_open("GBK", "UCS-4");
			if ((iconv_t)-1 == cd)
			{
				return -1;
			}

			size_t ret_size = iconv(cd, (char**)&from_u16, (size_t*)&in_len, (char**)&out8, (size_t*)&out_len);
			iconv_close(cd);

			if ((size_t)(-1) == ret_size)
			{
				return -1;
			}

			to_gbk_len = STRUTILS_MAX_BUFFER_SIZE - out_len / sizeof(char);
			*to_gbk = new char[to_gbk_len + 1];
			if (nullptr == *to_gbk)	return -1;
			memcpy(*to_gbk, tmp_buf_out8, to_gbk_len * sizeof(char));
			(*to_gbk)[to_gbk_len] = 0;
#endif
			return to_gbk_len;
		}

		static int ansi_to_utf8(char* from_gbk, char** to_u8);

		static void free_buf(char* buf8)
		{
			if (buf8)
			{
				delete[] buf8;
			}
		}

		static void free_buf(char16_t* buf16)
		{
			if (buf16)
			{
				delete[] buf16;
			}
		}

#ifndef _WIN32
		static void free_buf(wchar_t* buf)
		{
			if (buf)
			{
				delete[] buf;
			}
		}
#endif

	private:
		static size_t u16strlen(const char16_t* in_buf)
		{
			if (!in_buf)
			{
				return 0;
			}

			const char16_t* eos = in_buf;

			while (*(eos++));

			return (eos - in_buf - 1);
		}
	};

}//namespace cosmos

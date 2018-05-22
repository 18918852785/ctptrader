#pragma once

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <malloc.h>
#define _T16(x)	 L ## x
#else//if defined(XP_MACOSX)
#include "string_define.h"
#include <string>
#include <iconv.h>
#include <alloca.h>
#include <errno.h>
#define _T16(x)	u ## x
#endif

#include <assert.h>
#include "string_alloc.h"

#pragma pack(push, 8)

#ifndef CP_ACP
#define CP_ACP  0
#endif

#ifndef CP_UTF8
#define CP_UTF8 65001
#endif

#ifndef CP_936
#define CP_936  936
#endif

#ifndef INT_MAX
#define INT_MAX 2147483647
#endif

#ifndef ALLOC_STACK_MAX
#define ALLOC_STACK_MAX 2048
#endif

#ifndef DEBUG
#ifndef _WIN32
#define XP_USES_CONVERSION int _convert; (_convert); UINT _acp = 0; (_acp); LPCWSTR _lpw; (_lpw); LPCSTR _lpa; (_lpa); char16_t* _lpa16; (_lpa16)
#define XP_USES_CONVERSION_EX int _convert; (_convert); UINT _acp = 0; (_acp); LPCWSTR _lpw; (_lpw); LPCSTR _lpa; (_lpa); char16_t* _lpa16; (_lpa16); USES_HEAP_ALLOCA
#else
#define XP_USES_CONVERSION int _convert; (_convert); UINT _acp = 0; (_acp); LPCWSTR _lpw; (_lpw); LPCSTR _lpa; (_lpa)
#define XP_USES_CONVERSION_EX int _convert; (_convert); UINT _acp = 0; (_acp); LPCWSTR _lpw; (_lpw); LPCSTR _lpa; (_lpa); USES_HEAP_ALLOCA
#endif
#else
#ifndef _WIN32
#define XP_USES_CONVERSION int _convert = 0; (_convert); UINT _acp = 0; (_acp); LPCWSTR _lpw = NULL; (_lpw); LPCSTR _lpa = NULL; (_lpa); char16_t* _lpa16 = NULL; (_lpa16)
#define XP_USES_CONVERSION_EX int _convert = 0; (_convert); UINT _acp = 0; (_acp); LPCWSTR _lpw = NULL; (_lpw); LPCSTR _lpa = NULL; (_lpa); char16_t* _lpa16 = NULL; (_lpa16); USES_HEAP_ALLOCA
#else
#define XP_USES_CONVERSION int _convert = 0; (_convert); UINT _acp = 0; (_acp); LPCWSTR _lpw = NULL; (_lpw); LPCSTR _lpa = NULL; (_lpa); char16_t* _lpa16 = NULL; (_lpa16)
#define XP_USES_CONVERSION_EX int _convert = 0; (_convert); UINT _acp = 0; (_acp); LPCWSTR _lpw = NULL; (_lpw); LPCSTR _lpa = NULL; (_lpa); char16_t* _lpa16 = NULL; (_lpa16); USES_HEAP_ALLOCA
#endif
#endif

#ifndef _WIN32
#define C8_TO_U16_CP(lpa, cp) (\
	((_lpa = lpa) == NULL) ? NULL : (\
	_convert = (strlen(_lpa)+1),\
	(INT_MAX/2<_convert) ? NULL : \
	_mbs2wcs((char16_t*) alloca(_convert*sizeof(WCHAR)), _lpa, _convert, (cp))))

#define C8_TO_U16_CP_EX(lpa, cp) (\
	((_lpa = lpa) == nullptr) ? nullptr : (\
	_convert = (strlen(_lpa)+1),\
	_mbs2wcs((ALLOC_STACK_MAX<_convert) ? (char16_t*)_xp_safe_alloc_mgr.alloca(_convert*sizeof(WCHAR)) : (char16_t*)alloca(_convert*sizeof(WCHAR)), _lpa, _convert, (cp))))

#define U16_TO_C8_CP(lpa16, cp) (\
	((_lpa16 = lpa16) == NULL) ? NULL : (\
	_convert = (ucslen(_lpa16)+1) * sizeof(char16_t) * 2,\
	(INT_MAX/2<_convert) ? NULL : \
	_wcs2mbs((LPSTR) alloca(_convert), _lpa16, _convert, (cp))))

#define U16_TO_C8_CP_EX(lpa16, cp) (\
	((_lpa16 = lpa16) == NULL) ? NULL : (\
	_convert = (ucslen(_lpa16)+1) * sizeof(char16_t) * 2,\
	_wcs2mbs((ALLOC_STACK_MAX<_convert) ? (LPSTR)_xp_safe_alloc_mgr.alloca(_convert) : (LPSTR)alloca(_convert), _lpa16, _convert, (cp))))

#define U16_TO_U32(lpa16) (\
	((_lpa16 = lpa16) == NULL) ? NULL : (\
	_convert = (ucslen(_lpa16)+1),\
	(INT_MAX/2<_convert) ? NULL : \
	_wcs2wcs((LPWSTR) alloca(_convert*sizeof(WCHAR)), _lpa16, _convert*sizeof(char16_t))))

#define U16_TO_U32_EX(lpa16) (\
	((_lpa16 = lpa16) == NULL) ? NULL : (\
	_convert = (ucslen(_lpa16)+1)*sizeof(WCHAR),\
	_wcs2wcs((ALLOC_STACK_MAX<_convert) ? (LPWSTR)_xp_safe_alloc_mgr.alloca(_convert) : (LPWSTR)alloca(_convert), _lpa16, _convert)))

#define U32_TO_U16(lpw) (\
	((_lpw = lpw) == NULL) ? NULL : (\
	_convert = (wcslen(lpw)+1),\
	(INT_MAX/2<_convert) ? NULL : \
	_wcs2wcs((char16_t*) alloca(_convert*sizeof(WCHAR)), _lpw, _convert*sizeof(WCHAR))))

#define U32_TO_U16_EX(lpw) (\
	((_lpw = lpw) == NULL) ? NULL : (\
	_convert = (wcslen(lpw)+1)*sizeof(WCHAR),\
	_wcs2wcs((ALLOC_STACK_MAX<_convert) ? (char16_t*)_xp_safe_alloc_mgr.alloca(_convert) : (char16_t*)alloca(_convert), _lpw, _convert)))
#endif

#define C8_TO_W_CP(lpa, cp) (\
	((_lpa = lpa) == NULL) ? NULL : (\
	_convert = (strlen(_lpa)+1),\
	(INT_MAX/2<_convert) ? NULL : \
	_mbs2wcs((LPWSTR) alloca(_convert*sizeof(WCHAR)), _lpa, _convert, (cp))))

#define C8_TO_W_CP_EX(lpa, cp) (\
	((_lpa = lpa) == NULL) ? NULL : (\
	_convert = (strlen(_lpa)+1),\
	_mbs2wcs((ALLOC_STACK_MAX<_convert) ? (LPWSTR)_xp_safe_alloc_mgr.alloca(_convert*sizeof(WCHAR)) : (LPWSTR)alloca(_convert*sizeof(WCHAR)), _lpa, _convert, (cp))))

#define W_TO_C8_CP(lpw, cp) (\
	((_lpw = lpw) == NULL) ? NULL : (\
	_convert = (wcslen(_lpw)+1) * sizeof(WCHAR) * 2,\
	(INT_MAX/2<_convert) ? NULL : \
	_wcs2mbs((LPSTR) alloca(_convert), _lpw, _convert, (cp))))

#define W_TO_C8_CP_EX(lpw, cp) (\
	((_lpw = lpw) == NULL) ? NULL : (\
	_convert = (wcslen(_lpw)+1) * sizeof(WCHAR) * 2,\
	_wcs2mbs((ALLOC_STACK_MAX<_convert) ? (LPSTR)_xp_safe_alloc_mgr.alloca(_convert) : (LPSTR)alloca(_convert), _lpw, _convert, (cp))))

//--------------------------
#define UTF8_TO_WCHAR(lpa) (\
	C8_TO_W_CP(lpa, CP_UTF8))

#define UTF8_TO_WCHAR_EX(lpa) (\
	C8_TO_W_CP_EX(lpa, CP_UTF8))

#define WCHAR_TO_UTF8(lpw) (\
	W_TO_C8_CP(lpw, CP_UTF8))

#define WCHAR_TO_UTF8_EX(lpw) (\
	W_TO_C8_CP_EX(lpw, CP_UTF8))

#define GBK_TO_WCHAR(lpa) (\
	C8_TO_W_CP(lpa, CP_936))

#define GBK_TO_WCHAR_EX(lpa) (\
	C8_TO_W_CP_EX(lpa, CP_936))

#define WCHAR_TO_GBK(lpw) (\
	W_TO_C8_CP(lpw, CP_936))

#define WCHAR_TO_GBK_EX(lpw) (\
	W_TO_C8_CP_EX(lpw, CP_936))

#define ANSI_TO_WCHAR(lpa) (\
	C8_TO_W_CP(lpa, CP_ACP))

#define ANSI_TO_WCHAR_EX(lpa) (\
	C8_TO_W_CP_EX(lpa, CP_ACP))

#define WCHAR_TO_ANSI(lpw) (\
	W_TO_C8_CP(lpw, CP_ACP))

#define WCHAR_TO_ANSI_EX(lpw) (\
	W_TO_C8_CP_EX(lpw, CP_ACP))

#ifndef _WIN32
#define GBK_TO_CHAR16(lpa) (\
	C8_TO_U16_CP(lpa, CP_936))

#define GBK_TO_CHAR16_EX(lpa) (\
	C8_TO_U16_CP_EX(lpa, CP_936))

#define CHAR16_TO_GBK(lpa16) (\
	U16_TO_C8_CP(lpa16, CP_936))

#define CHAR16_TO_GBK_EX(lpa16) (\
	U16_TO_C8_CP_EX(lpa16, CP_936))

#define UTF8_TO_CHAR16(lpa) (\
	C8_TO_U16_CP(lpa, CP_UTF8))

#define UTF8_TO_CHAR16_EX(lpa) (\
	C8_TO_U16_CP_EX(lpa, CP_UTF8))

#define CHAR16_TO_UTF8(lpa16) (\
	U16_TO_C8_CP(lpa16, CP_UTF8))

#define CHAR16_TO_UTF8_EX(lpa16) (\
	U16_TO_C8_CP_EX(lpa16, CP_UTF8))

#define WCHAR_TO_CHAR16(lpw) (\
	U32_TO_U16(lpw))

#define WCHAR_TO_CHAR16_EX(lpw) (\
	U32_TO_U16_EX(lpw))

#define CHAR16_TO_WCHAR(lpa16) (\
	U16_TO_U32(lpa16))

#define CHAR16_TO_WCHAR_EX(lpa16) (\
	U16_TO_U32_EX(lpa16))
#endif

#define GBK_TO_UTF8(lpa) (\
	((_lpw = GBK_TO_WCHAR(lpa)) == NULL) ? NULL : (\
	WCHAR_TO_UTF8(_lpw)))

#define GBK_TO_UTF8_EX(lpa) (\
	((_lpw = GBK_TO_WCHAR_EX(lpa)) == NULL) ? NULL : (\
	WCHAR_TO_UTF8_EX(_lpw)))

#define UTF8_TO_GBK(lpa) (\
	((_lpw = UTF8_TO_WCHAR(lpa)) == NULL) ? NULL : (\
	WCHAR_TO_GBK(_lpw)))

#define UTF8_TO_GBK_EX(lpa) (\
	((_lpw = UTF8_TO_WCHAR_EX(lpa)) == NULL) ? NULL : (\
	WCHAR_TO_GBK_EX(_lpw)))

#define ANSI_TO_UTF8(lpa) (\
	((_lpw = ANSI_TO_WCHAR(lpa)) == NULL) ? NULL : (\
	WCHAR_TO_UTF8(_lpw)))

#define ANSI_TO_UTF8_EX(lpa) (\
	((_lpw = ANSI_TO_WCHAR_EX(lpa)) == NULL) ? NULL : (\
	WCHAR_TO_UTF8_EX(_lpw)))

#define UTF8_TO_ANSI(lpa) (\
	((_lpw = UTF8_TO_WCHAR(lpa)) == NULL) ? NULL : (\
	WCHAR_TO_ANSI(_lpw)))

#define UTF8_TO_ANSI_EX(lpa) (\
	((_lpw = UTF8_TO_WCHAR_EX(lpa)) == NULL) ? NULL : (\
	WCHAR_TO_ANSI_EX(_lpw)))

//
// SUCCESS: return 0, FAILURE: return !0
//
inline static LPSTR _wcs2mbs(LPSTR lpa, LPCWSTR lpw, int nChars, UINT acp)
{
	assert(lpa != nullptr);
	assert(lpw != nullptr);
	if ( lpw == nullptr || lpa == nullptr )
		return nullptr;

	*lpa = '\0';
#ifdef _WIN32
	if ( 0 == WideCharToMultiByte( acp, 0, lpw, -1, lpa, nChars, nullptr, nullptr) )
		return nullptr;
#else
	char* out8 = lpa;
	size_t out_len = nChars;
	size_t in_len = nChars / 2;

	iconv_t cd;
	const char* target_encoding;
	switch (acp)
	{
	case CP_ACP:
		target_encoding = "ASCII";
		break;
	case CP_UTF8:
		target_encoding = "UTF-8";
		break;
	case CP_936:
		target_encoding = "GBK";
		break;
	default:
		target_encoding = "UTF-8";
		break;
	}

	cd = iconv_open(target_encoding, "UCS-4-INTERNAL");
	if ( (iconv_t)-1 == cd )
		return nullptr;

	size_t ret_size = iconv(cd, (char**)&lpw, (size_t*)&in_len, (char**)&out8, (size_t*)&out_len);
	iconv_close(cd);
	if ( (size_t)(-1) == ret_size )
		return nullptr;
#endif
	return lpa;
}

#ifndef _WIN32
inline static LPSTR _wcs2mbs(LPSTR lpa, const char16_t* lpw, int nChars, UINT acp)
{
	assert(lpa != nullptr);
	assert(lpw != NULL);
	if ( lpw == nullptr || lpa == nullptr )
		return nullptr;

	*lpa = '\0';

	char* out8 = lpa;
	size_t out_len = nChars;
	size_t in_len = nChars / 2;

	iconv_t cd;
	const char* target_encoding;
	switch (acp)
	{
	case CP_ACP:
		target_encoding = "ASCII";
		break;
	case CP_UTF8:
		target_encoding = "UTF-8";
		break;
	case CP_936:
		target_encoding = "GBK";
		break;
	default:
		target_encoding = "UTF-8";
		break;
	}

	cd = iconv_open(target_encoding, "UCS-2-INTERNAL");
	if ( (iconv_t)-1 == cd )
		return nullptr;

	size_t ret_size = iconv(cd, (char**)&lpw, (size_t*)&in_len, (char**)&out8, (size_t*)&out_len);
	iconv_close(cd);

	if ( (size_t)(-1) == ret_size )
		return nullptr;

	return lpa;
}

static inline char16_t* _mbs2wcs(char16_t* lpw, LPCSTR lpa, int nChars, UINT acp)
{
	assert(lpa != nullptr);
	assert(lpw != nullptr);
	if ( lpw == nullptr || lpa == nullptr )
		return nullptr;

	*lpw = '\0';

	char16_t* out16 = lpw;
	size_t out16_len = nChars * sizeof(WCHAR);
	size_t in_len = nChars;

	iconv_t cd;
	const char* source_encoding;
	switch (acp)
	{
	case CP_ACP:
		source_encoding = "ASCII";
		break;
	case CP_UTF8:
		source_encoding = "UTF-8";
		break;
	case CP_936:
		source_encoding = "GBK";
		break;
	default:
		source_encoding = "UTF-8";
		break;
	}

	cd = iconv_open("UCS-2-INTERNAL", source_encoding);
	if ( (iconv_t)-1 == cd )
		return nullptr;

	size_t ret_size = iconv(cd, (char**)&lpa, (size_t*)&in_len, (char**)&out16, (size_t*)&out16_len);
	iconv_close(cd);

	if ( (size_t)(-1) == ret_size )
	{
		return nullptr;
	}

	return lpw;
}
#endif

#ifndef _WIN32
inline static char16_t* _wcs2wcs(char16_t* lpw16, const wchar_t* lpw32, int nChars)
{
	assert(lpw16 != nullptr);
	assert(lpw32 != nullptr);
	if ( lpw32 == nullptr || lpw16 == nullptr )
		return nullptr;

	*lpw16 = '\0';
	char16_t* out16 = lpw16;
	size_t out_len = nChars;
	size_t in_len = nChars;

	iconv_t cd;
	cd = iconv_open("UCS-2-INTERNAL", "UCS-4-INTERNAL");
	if ( (iconv_t)-1 == cd )
		return nullptr;

	size_t ret_size = iconv(cd, (char**)&lpw32, (size_t*)&in_len, (char**)&out16, (size_t*)&out_len);
	iconv_close(cd);

	if ( (size_t)(-1) == ret_size )
		return nullptr;

	return lpw16;
}

inline static wchar_t* _wcs2wcs(wchar_t* lpw32, const char16_t* lpw16, int nChars)
{
	assert(lpw16 != nullptr);
	assert(lpw32 != nullptr);
	if ( lpw32 == nullptr || lpw16 == nullptr )
		return nullptr;

	*lpw32 = '\0';
	wchar_t* out32 = lpw32;
	size_t out_len = nChars * 2;
	size_t in_len = nChars;

	iconv_t cd;
	cd = iconv_open("UCS-4-INTERNAL", "UCS-2-INTERNAL");
	if ( (iconv_t)-1 == cd )
		return nullptr;

	size_t ret_size = iconv(cd, (char**)&lpw16, (size_t*)&in_len, (char**)&lpw32, (size_t*)&out_len);
	iconv_close(cd);

	if ( (size_t)(-1) == ret_size )
	{
		int err = errno;
		return nullptr;
	}

	return lpw32;
}
#endif

static inline LPWSTR _mbs2wcs(LPWSTR lpw, LPCSTR lpa, int nChars, UINT acp)
{
	assert(lpa != nullptr);
	assert(lpw != nullptr);
	if ( lpw == nullptr || lpa == nullptr )
		return nullptr;

	*lpw = '\0';
#ifdef _WIN32
	if ( 0 == MultiByteToWideChar(acp, 0, lpa, -1, lpw, nChars) )
		return nullptr;
#else
	wchar_t* out32 = lpw;
	size_t out32_len = nChars * sizeof(WCHAR);
	size_t in_len = nChars;

	iconv_t cd;
	const char* source_encoding;
	switch (acp)
	{
	case CP_ACP:
		source_encoding = "ASCII";
		break;
	case CP_UTF8:
		source_encoding = "UTF-8";
		break;
	case CP_936:
		source_encoding = "GBK";
		break;
	default:
		source_encoding = "UTF-8";
		break;
	}

	cd = iconv_open("UCS-4-INTERNAL", source_encoding);
	if ( (iconv_t)-1 == cd )
		return nullptr;

	size_t ret_size = iconv(cd, (char**)&lpa, (size_t*)&in_len, (char**)&out32, (size_t*)&out32_len);
	iconv_close(cd);

	if ( (size_t)(-1) == ret_size )
	{
		return nullptr;
	}
#endif
	return lpw;
}

#ifndef _WIN32 
inline static size_t ucslen(const char16_t* in_buf)
{
	if ( !in_buf )
		return 0;

	const char16_t* eos = in_buf;
	while ( *(eos++) );

	int len = (--eos - in_buf);
	return len;
}
#endif

#pragma pack(pop)


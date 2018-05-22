#ifdef _WIN32

#define _ATL_ATTRIBUTES
#define _ATL_APARTMENT_THREADED
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <atlbase.h>
#include <atlcom.h>
using namespace ATL;
typedef LONG HRESULT;

#include <atlstr.h>
#include <comutil.h>

#include "util.h"

namespace signalrcpp{
	//////////////////////////////////////////////////////////////////////////
	//util
	byte hexchar2num(char c)
	{
		if( c >= '0' && c <= '9')
			return c - '0';
		return c - 'a' + 10;
	}

	std::string util::variant2utfstring(const VARIANT& v)
	{
		std::string ret;
		if( v.vt == VT_EMPTY || v.vt == VT_NULL )
			return ret;
		if( v.vt == VT_BSTR )
			return bstr2utfstring(v.bstrVal);
		char buf[128] = {0};
		sprintf_s(buf, sizeof(buf)-1, "vt=%d", v.vt);
		ret = buf;
		return ret;
	}


	// Convert a wide Unicode string to an UTF8 string
	std::string utf8_encode(const std::wstring &wstr)
	{
		if( wstr.empty() ) return std::string();
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string strTo( size_needed, 0 );
		WideCharToMultiByte                  (CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}

	// Convert an UTF8 string to a wide Unicode String
	std::wstring utf8_decode(const std::string &str)
	{
		if( str.empty() ) return std::wstring();
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo( size_needed, 0 );
		MultiByteToWideChar                  (CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}

	std::string wstrtostr(const std::wstring &wstr)
	{
		// Convert a Unicode string to an ASCII string
		if( wstr.empty() ) return std::string();
		int size_needed = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string strTo( size_needed, 0 );
		WideCharToMultiByte                  (CP_ACP, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}

	std::wstring strtowstr(const std::string &str)
	{
		// Convert an ASCII string to a Unicode String
		std::wstring wstrTo;
		wchar_t *wszTo = new wchar_t[str.length() + 1];
		wszTo[str.size()] = L'\0';
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wszTo, (int)str.length());
		wstrTo = wszTo;
		delete[] wszTo;
		return wstrTo;
	}

	std::string util::bstr2utfstring(const BSTR b)
	{
		if(!b)
			return std::string();
		auto len = SysStringLen(b);
		if( ! len )
			return std::string();

		int size_needed = WideCharToMultiByte(CP_UTF8, 0, b, (int)len, NULL, 0, NULL, NULL);
		std::string strTo( size_needed, 0 );
		WideCharToMultiByte                  (CP_UTF8, 0, b, (int)len, &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}

	std::vector<char> util::variant2bytearray( const VARIANT& v )
	{
		std::vector<char> ret;
		if( v.vt == VT_EMPTY || v.vt == VT_NULL )
			return ret;
		if( v.vt == VT_BSTR )
		{
			auto s_pchar( signalrcpp::util::bstr2utfstring(v.bstrVal));
			if( s_pchar.size() == 0 )
				return ret;
			ret.resize( s_pchar.size() );
			memcpy(&ret[0], &s_pchar[0], s_pchar.size());
			return ret;
		}
		if( v.vt == (VT_ARRAY | VT_UI1 ))
		{
			auto psa = v.parray;
			void* pba = 0;
			LONG len = 0;
			auto hr = SafeArrayGetUBound(psa, 1, &len);
			if( hr )
				return ret;//failed
			len ++;

			auto result=SafeArrayAccessData(psa, &pba);
			if( result || ! pba )
			{
				return ret;
			}
			// Copy the bytes to the safe array.
			ret.resize(len);
			memcpy( &ret[0], pba, len );

			// Unaccess the data
			SafeArrayUnaccessData(psa);
		}
		return ret;
	}

	long util::bytearray2variant( const void* data, int data_len, CComVariant& ret )
	{
		ret.Clear();
		if( ! data || data_len == 0 )
			return 0;
		SAFEARRAYBOUND  sab={0};
		sab.lLbound = 0;
		sab.cElements = data_len;
		auto psa = SafeArrayCreate(VT_UI1, 1, &sab);
		if( ! psa )
			return E_FAIL;
		void* pba = 0;
		auto result=SafeArrayAccessData(psa, &pba);
		if( result || ! pba )
		{
			SafeArrayDestroy(psa);
			return E_FAIL;
		}
		// Copy the bytes to the safe array.
		memcpy( pba, data, data_len );

		 // Unaccess the data
		SafeArrayUnaccessData(psa);

		ret.vt = VT_ARRAY | VT_UI1;
		ret.parray = psa;
		return 0;
	}

	long util::hex2safearray( const char* hex, int hex_len, CComVariant& v_ret )
	{
		v_ret.Clear();
		if( ! hex || ! hex_len )
			return 0;
		std::vector<unsigned char> byte_arr;
		auto ret = hex2bytearray(hex, hex_len, byte_arr);
		if( ret )
			return ret;//failed
		if( byte_arr.size() == 0 )
			return 0;
		return bytearray2variant(&byte_arr[0], (int) byte_arr.size(), v_ret);
	}

	long util::hex2bytearray( const char* hex, int hex_len, std::vector<unsigned char>& ret )
	{
		ret.clear();
		if( ! hex || ! hex_len )
			return 0;
		//字符串必须是 'hex:'开头
		if( hex_len < (4+2) || strncmp(hex,"hex:",4) != 0 )
		{
			ret.resize(hex_len);
			memcpy(&ret[0], hex, hex_len);
			return 0;
		}
		int len = (hex_len-4)/2;
		hex += 4;//header
		hex_len -= 4;
		ret.resize(len);
		for(int i=0; i<len; i++)
		{
			hex_len -= 2;
			if(hex_len <0 )//越界
			{
				//LOG_FUNC(module_id,xp_log_error,"hex string format error");
				break;
			}
			ret[i] = hexchar2num( *hex ) * 16 + hexchar2num( *(hex+1) );
			hex += 2;
		}
		return 0;
	}

	char hex_char []= 
	{
		'0',
		'1',
		'2',
		'3',
		'4',
		'5',
		'6',
		'7',
		'8',
		'9',
		'a',
		'b',
		'c',
		'd',
		'e',
		'f',
		0  ,
	};

	long util::bytearray2hex( const void* data, int data_len, std::string& ret )
	{

		ret.clear();
		if( ! data || ! data_len )
			return 0;
		
		char* pdata = (char*)data;

		ret.resize( 4 + data_len*2, 0);
		memcpy(&ret[0], "hex:", 4);//header

		for(int i=0; i<data_len;i++)
		{
			ret[4+i*2]   = hex_char[ (unsigned char) *pdata / 16 ];
			ret[4+i*2+1] = hex_char[(unsigned char)*pdata % 16];
			pdata ++;
		}
		return 0;
	}

	std::string util::variant2hexstring( const VARIANT& v )
	{
		std::string ret;

		if( v.vt == VT_BSTR )
		{
			auto s_pchar( signalrcpp::util::bstr2utfstring(v.bstrVal));
			if( s_pchar.size() == 0 )
				return ret;
			auto hr = bytearray2hex(&s_pchar[0], (int)s_pchar.size(), ret);
			if( hr )//fail
				return ret;
			return ret;
		}
		if( v.vt != (VT_ARRAY|VT_UI1))
			return ret;//failed

		auto arr = variant2bytearray(v);
		if( arr.size() == 0 )
			return ret;
		auto hr = bytearray2hex(&arr[0], (int)arr.size(), ret);
		if( hr )//fail
			return ret;
		return ret;
	}

	std::string util::utf2stdstring( const char* utf8, int len )
	{
		std::string ret;
		if( ! utf8 || ! len )
			return ret;
		std::string s_utf8;
		s_utf8.resize(len);
		memcpy(&s_utf8[0], utf8, len);
		auto w = utf8_decode(s_utf8);
		return wstrtostr(w);
	}

	std::string util::utf2stdstring_null_terminate( const char* utf8, int len )
	{
		auto s = utf2stdstring(utf8, len);
		if( s.size() == 0 )
			return s;
		std::string ret( s.size()+1, 0 );
		memcpy(&ret[0], &s[0], s.size());
		return ret;
	}

	long util::git_create(IGlobalInterfaceTable** ppRet)
	{
		long hr = 0;
		CComPtr<IGlobalInterfaceTable> p;

		hr = p.CoCreateInstance(CLSID_StdGlobalInterfaceTable);
		if( hr != S_OK || ! p )
		{
			//LOG_FUNC(module_id, xp_log_error, "CoCreateInstance(CLSID_StdGlobalInterfaceTable) failed");
		}else
		{
			*ppRet = p.Detach();//save
		}
		return hr;
	}

	//////////////////////////////////////////////////////////////////////////
	//unit test
	int signalrcpp_util_unittest ( int argc, _TCHAR* argv[])
	{
		std::string ret;
		signalrcpp::util::bytearray2hex("abc123", 6, ret);
		CComVariant v_ret;
		signalrcpp::util::hex2safearray( &ret[0], (int)ret.size(), v_ret);
		return 0;
	}

}

#endif//win32

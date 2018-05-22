#pragma once

#include <string>
#include <vector>
#include <functional>
#include <algorithm>


namespace signalrcpp
{
	class util
	{
	public:
		static long              bytearray2hex(const void* data, int data_len, std::string& ret);
		static long              hex2bytearray(const char* hex, int hex_len, std::vector<unsigned char>& ret);

		static std::string       utf2stdstring(const char* utf8, int len);
		static std::string       utf2stdstring_null_terminate(const char* utf8, int len);
#ifdef _WIN32
	public://string , byte array, hex string
		static std::string       variant2utfstring(const VARIANT& v);
		static std::string       variant2hexstring(const VARIANT& v);

		static std::string       bstr2utfstring(const BSTR v);
		static std::vector<char> variant2bytearray(const VARIANT& v);
		static long              bytearray2variant(const void* data, int data_len, CComVariant& ret);
		static long              hex2safearray(const char* hex, int hex_len, CComVariant& ret);

	public://git
		static long              git_create(IGlobalInterfaceTable** ppRet);
#endif
	};

}

#pragma once
#include <memory>
#include <string>
#include <string.h>

namespace signalrcpp
{
	struct struct_string//简单字符串
	{
		const char* ptr;
		int len;
	};
	typedef std::shared_ptr<struct_string> string_ptr;

	//////////////////////////////////////////////////////////////////////////
	struct hub_response//状态变更通知
	{
		long       event_id;
		string_ptr classs_name, object_name, data, raw;
	};
	typedef std::shared_ptr<hub_response> hub_response_ptr;

	//////////////////////////////////////////////////////////////////////////
	//
	struct struct_string_impl: struct_string
	{
		std::string m_str;
		static string_ptr create(const std::string& s)
		{
			return create(s.c_str(), s.size());
		}
		static string_ptr create(const char*msg)
		{
			auto len = 0;
			if(msg)
				len = strlen(msg);
			return create(msg, len);
		}
		static string_ptr create(const char*msg, int len)
		{
			auto p = new struct_string_impl();
			string_ptr ret( p);
			p->m_str.resize(len,0);
			p->len = len;
			if(len)
			{
				memcpy(&p->m_str[0], msg, len);
			}
			p->ptr = p->m_str.c_str();
			
			return ret;
		}
	};
}
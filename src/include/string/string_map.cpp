#define STRING_MAP_TEST 1

#include "string_map.h"
#include "string_util.h"
#include <assert.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <sstream>

#include "string_map_impl.h"
#include "string_map_serializer.h"

#if STRING_MAP_TEST ==1
#include <time.h>
#endif

namespace cosmos {

	static std_string INT2STR(int a)
	{
		char buf[128];
		itoa(a, buf, 10);
		return buf;
	}
	static std_string INT642STR(int a)
	{
		char buf[128];
		_i64toa(a, buf, 10);
		return buf;
	}

	static string_map_item string_map_item_create(const std::string& v)
	{
		return string_item_create(v.c_str(), (int)v.size());
	}

	//////////////////////////////////////////////////////////////////////////
	void string_unordered_map::create(const char* input_str, int input_str_len, string_unordered_map* pfields)
	{
		pfields->clear();
		if (input_str == nullptr || !input_str_len)
			return;

		std_string input;
		input.resize(input_str_len);
		memcpy(&input[0], input_str, input_str_len);

		std_vector<std_string> keys;
		std_vector<std_string> values;
		string_util::SplitKeyValue(input, keys, values, false, ",", "=");

		for (int i = 0; i < (int)keys.size(); i++)
		{
			(*pfields)[keys[i]] = string_map_item_create(values[i]);
		}
	}
#include "string_map_serializer.inc"
#include "string_map_bin.inc"

	std_string string_unordered_map::toString()
	{
		std_string ret;
		for (auto it = this->begin(); it != this->end(); it++)
		{
			ret += it->first;
			ret += "=";
			ret += string_item_impl::toString( it->second);
			ret += ",";
		}
		//
		return ret;
	}

	string_map_item string_unordered_map::get(const std_string& key)
	{
		if (this->count(key) > 0)
		{
			return (*this)[key];
		}
		return string_map_item();
	}

	bool string_unordered_map::get(const std_string& key, std_string& v_ret)
	{
		if (this->count(key) > 0)
		{
			auto v = (*this)[key];
			if (!v)return false;
			v_ret.resize(v->size());
			if (v->size())
			{
				memcpy(&v_ret[0], v->c_str(), v->size());
			}
			return true;
		}
		return false;
	}

	bool string_unordered_map::get(const std_string& key, string_item_ptr& v)
	{
		if (this->count(key) > 0)
		{
			v = (*this)[key];
			return true;
		}
		return false;
	}

	void string_unordered_map::set(const std_string& key, const std_string& v)
	{
		(*this)[key] = string_map_item_create(v);
	}
	void string_unordered_map::set(const std_string& key, string_item_ptr v)
	{
		(*this)[key] = string_map_item(v);
	}

	std_string string_item_impl::toString(const std_shared_ptr<string_item>& value)
	{
		std_string ret;
		if (!value || ! value->size())
			return ret;
		ret.resize(value->size());
		memcpy(&ret[0], value->c_str(), value->size());
		return ret;
	}


	//////////////////////////////////////////////////////////////////////////
	//string_map
	string_item_ptr string_item_create(const char*s)
	{
		return std_make_shared<string_item_impl>(s);
	}

	string_item_ptr string_item_create(const char*s, int size)
	{
		return std_make_shared<string_item_impl>(s, size);
	}
	string_item_ptr string_item_create(int v_i)
	{
		return std_make_shared<string_item_impl>(v_i);
	}
	string_item_ptr string_item_create(__int64 v_i)
	{
		return std_make_shared<string_item_impl>(v_i);
	}

	std_shared_ptr<string_map> string_map_create(const char* input_str)
	{
		return string_map_create(string_item_create(input_str));
	}

	std_shared_ptr<string_map> string_map_create(string_item_ptr input)
	{
		auto p = new string_map_impl();
		std_shared_ptr<string_map> ret(p);

		string_unordered_map::create(input->c_str(), input->size(), p->m_map.get());
		return ret;
	}
	std_shared_ptr<string_map> string_map_create(const char*p, int p_len)
	{
		return string_map_create(string_item_create(p, p_len));
	}

	//////////////////////////////////////////////////////////////////////////
	//string_item

	int string_item_impl::size() const
	{
		return (int)m_obj.size();
	}

	const char* string_item_impl::c_str()const
	{
		return m_obj.c_str();
	}


	string_item_ptr string_map_impl::get(int key)const
	{
		auto p = new string_item_impl();
		string_item_ptr ret(p);
		if (m_map->get(INT2STR(key), p->m_obj))
		{
			return ret;
		}
		return nullptr;
	}


	void string_map_impl::remove(int key)
	{
		m_map->erase(INT2STR(key));
	}

	string_item_ptr string_map_impl::to_string() const
	{
		auto p = new string_item_impl();
		string_item_ptr ret(p);
		p->m_obj = m_map->toString();
		return ret;
	}

	void string_map_impl::clear()
	{
		m_map->clear();
	}


	string_map_impl::string_map_impl()
	{
		m_map.reset(new string_unordered_map());
	}

	void string_map_impl::set(int key, const char* val, int val_len)
	{
		if (val_len <= 0)//字符串
		{
			m_map->set(INT2STR(key), val);
			return;
		}
		//二进制
		std_string s;
		s.resize(val_len, 0);
		memcpy(&s[0], val, val_len);
		m_map->set(INT2STR(key), s);
	}


	void string_map_impl::set(int key, string_item_ptr sip)
	{
		set(INT2STR(key).c_str(), sip);
	}

	void   string_map_impl::set(const char* key, string_item_ptr sip)
	{
		m_map->set(key, sip);
	}

	static std_string g_string_map_to_string_buffer;//全局静态变量，不能多线程调用
	INDICATOR_DATA_EXPORT const char* string_map_to_string(const char* input_str, int input_str_len)
	{
		auto pmap = cosmos::string_map_from_string(input_str, input_str_len);
		if(!pmap)
			return nullptr;
		auto s = pmap->to_string();
		if(!s)
			return nullptr;
		g_string_map_to_string_buffer= cosmos::string_item_impl::toString(s);
		return g_string_map_to_string_buffer.c_str();
	}
	

	INDICATOR_DATA_EXPORT int string_map_test1()
	{
		auto fields = string_map_create(nullptr);
		//fields->set(306,c.c_str());
		fields->set(307,"CN1901");
		char s_price[128]={0};
#if STRING_MAP_TEST==1
		time_t t= 0;
		time(&t);
		sprintf(s_price, "%.01f", t*1.0/1000/10);
#endif
		fields->set(504,s_price);
		fields->set(504,"M5");
		fields->set(999,"MA1");

		std::string s500;
		int test_len = 0x820;
		s500.resize(test_len,0);
		strcpy(&s500[0], "select 1 from abc");
		fields->set(500, s500.c_str(), s500.size());

		//二进制测试
		auto md1 = string_map_to_bin_string(fields.get());

		fields = string_map_from_string(md1->c_str(), md1->size());
		if(fields){
			auto v500 = fields->get(500);
			auto p500 = v500->c_str();
			auto p500_len = v500->size();
			assert(p500_len == test_len);
		}
		return 0;
	}

	INDICATOR_DATA_EXPORT int string_map_test2()
	{
		auto fields = string_map_create(nullptr);
		fields->set(307,"CN1901");
		return 0;
	}

	INDICATOR_DATA_EXPORT int string_map_test()
	{
		int i_ret = string_map_test1();
		if(i_ret)return i_ret;
		i_ret = string_map_test2();
		return i_ret;
	}
}//namespace cosmos
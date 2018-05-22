#pragma once
#include <string.h>

namespace cosmos{
	//////////////////////////////////////////////////////////////////////////
	//string item
	class INDICATOR_DATA_EXPORT string_item_impl : public string_item
	{
	public:
		string_item_impl(int v_i) :m_type(SIT_INT) {
			u.m_value_i = v_i;
		}

		string_item_impl(__int64 v_i) :m_type(SIT_INT64) {
			u.m_value_i64 = v_i;
		}

	public://string
		string_item_impl() :m_type(SIT_STRING) {}
		string_item_impl(const char*p)
			:m_type(SIT_STRING)
			, m_obj(p ? p : "") {}

		string_item_impl(const char*p, int size)
			:m_type(SIT_STRING)
		{
			m_obj.resize(size);
			if (size > 0)
			{
				memcpy(&m_obj[0], p, size);
			}
		}

		int         size()const;
		const char* c_str()const;
		void        set_str(const char* p)
		{
			if (p)
				m_obj = p;
			else
				m_obj.clear();
		}
		void  set_str(const char* p, int size)
		{
			if (size)
			{
				m_obj.resize(size);
				memcpy(&m_obj[0], p, size);
			}
			else
				m_obj.clear();
		}
		string_item_type get_type() { return m_type; }
		virtual void* get_data()
		{
			switch (m_type) {
			case SIT_STRING:
				return (void*)c_str();
			case SIT_INT:
				return (void*)u.m_value_i;
			case SIT_INT64:
				return (void*)u.m_value_i64;
			}
			return (void*)-1;
		}

		static std_string toString(const std_shared_ptr<string_item>& p);
	public:
		std_string m_obj;
		string_item_type m_type;
		union {
			int m_value_i;
			__int64 m_value_i64;
		}u;
	};


	//////////////////////////////////////////////////////////////////////////
	//string map item
#define string_map_item string_item_ptr

	class INDICATOR_DATA_EXPORT string_unordered_map : public std_unordered_map<std_string, string_map_item>
	{
	public:
		static void     create(const char* input_str, int input_str_len, string_unordered_map* pfields);

		string_map_item get(const std_string& key);
		bool            get(const std_string& key, std_string& v);
		bool            get(const std_string& key, std_shared_ptr<string_item>& v);

		void set(const std_string& key, const std_string& v);
		void set(const std_string& key, std_shared_ptr<string_item> v);

		std_string toString();

	};
	typedef std_shared_ptr<string_unordered_map> string_unordered_map_ptr;

	class string_map_impl : public string_map
	{
	public:
		string_map_impl();
		void            set(int key, const char* val, int val_len = 0);
		void            set(int key, string_item_ptr sip);
		void            set(const char* key, string_item_ptr sip);
		string_item_ptr get(int key)const;

		void            remove(int key);

		string_item_ptr to_string()const;

		void            clear();

	public:
		string_unordered_map_ptr m_map;
	};
}//namespace cosmos

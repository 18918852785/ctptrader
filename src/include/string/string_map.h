#pragma once

#include <memory>
#include <string>

#include "string_define.h"

namespace cosmos {
	//////////////////////////////////////////////////////////////////////////
	//string item type
	enum string_item_type
	{
		SIT_UNKNOWN = 0,
		SIT_STRING,
		SIT_INT,
		SIT_INT64,

		SIT_LIST,
	};

	//////////////////////////////////////////////////////////////////////////
	//string item
	struct string_item
	{
		virtual int         size()const = 0;
		virtual const char* c_str()const = 0;
		virtual void        set_str(const char*) = 0;
		virtual void        set_str(const char*, int len) = 0;

		virtual string_item_type get_type() = 0;
		virtual void* get_data() = 0;
	};

	typedef std_shared_ptr<string_item> string_item_ptr;

	struct item_list
	{
		virtual int             count()const = 0;
		virtual string_item_ptr get(int i) = 0;
		virtual int             insert(int i,string_item_ptr) = 0;
		virtual int             set(int i,string_item_ptr) = 0;
	};
	typedef std_shared_ptr<item_list> item_list_ptr;
	
	struct string_map
	{
		//int key / value
		virtual void            set(int key, const char* val, int val_len = 0) = 0;
		virtual void            set(int key, string_item_ptr sip) = 0;
		virtual void            set(const char* key, string_item_ptr sip) = 0;

		virtual string_item_ptr get(int key)const = 0;
		virtual void            remove(int key) = 0;
		virtual void            clear() = 0;	//clear

		virtual string_item_ptr to_string()const = 0;	//to binary string

	};

	typedef std_shared_ptr<string_map> string_map_ptr;

	//////////////////////////////////////////////////////////////////////////
	//string
	INDICATOR_DATA_EXPORT string_item_ptr string_item_create(const char* s);
	INDICATOR_DATA_EXPORT string_item_ptr string_item_create(const char*s, int size);
	INDICATOR_DATA_EXPORT string_item_ptr string_item_create(int v_i);
	INDICATOR_DATA_EXPORT string_item_ptr string_item_create(__int64 v_i64);

	//数组
	INDICATOR_DATA_EXPORT item_list_ptr item_list_create(string_item_type t);

	//map
	INDICATOR_DATA_EXPORT std_shared_ptr<string_map> string_map_create(const char* input_str);
	INDICATOR_DATA_EXPORT std_shared_ptr<string_map> string_map_create(const char* input_str, int input_str_len);
	INDICATOR_DATA_EXPORT std_shared_ptr<string_map> string_map_create(string_item_ptr input);
	INDICATOR_DATA_EXPORT string_item_ptr            string_map_to_bin_string(string_map* m);
	INDICATOR_DATA_EXPORT const char*				 string_map_to_string(const char* input_str, int input_str_len);

	INDICATOR_DATA_EXPORT std_shared_ptr<string_map> string_map_from_string(const char*p, int p_len);//自动判断是文本还是二进制

	INDICATOR_DATA_EXPORT int                        string_map_test();
}//namespace cosmos

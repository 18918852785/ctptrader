#pragma once

//////////////////////////////////////////////////////////////////////////
//
//#define REQUEST_DATA_VERSION 0x00316477 //wd1	
namespace cosmos {

	class string_map_serializer
	{
		int m_is_text;
		unsigned int m_version;//版本号
	public:
		string_map_serializer(const char*buf, int buf_len);

		const char* m_data;
		int m_data_len;

		string_map_ptr fields;

		const char* get_buffer()const;
		std::string get_buffer_as_string()const;
		int get_buffer_length()const;
		bool parse();
		bool is_text(); //是不是明文?
		static bool read_uint(const char*p, int& s, int len, unsigned int &v);
		static bool read_int(const char*p, int& s, int len, int &v);
		static bool read_int64(const char*p, int& s, int len, __int64 &v);
		static bool read_bin(const char*p, int& s, int len, int bin_len, string_item_ptr& s_bin);
		static bool read_bin(const char*p, int& s, int len, int bin_len, const char* bin);

		static bool read_string(const char*p, int& s, int len, std::string& s_ret);

		std::string to_string();
		string_map_ptr parse_binary();
		std::string to_binary_string();

		static int parse_binary(string_map* m, const char*p, int p_len);

		static int read_value(const char* p, int& p_pos, int p_len, string_item_ptr& v_str);
		static std::string to_binary_string(const string_map* m);

	public:

		static void write_string(std::ostringstream& ss, const char* str_p, int str_len);
		static void write_value(std::ostringstream& ss, string_item_ptr s);
		static void write_string(std::ostringstream& ss, const std::string& s);
		static void write_bin(std::ostringstream& ss, const char* p, int p_len);
		static void write_uint(std::ostringstream& ss, unsigned int v);
		static void write_int(std::ostringstream& ss, int v);
		static void write_int64(std::ostringstream& ss, __int64 v);
	};
}//namespace cosmos 

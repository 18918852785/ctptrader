//////////////////////////////////////////////////////////////////////////
//
static unsigned int get_version()
{
	char *p = "wd1";
	return *((unsigned int*)p);
}

static void find_version(const char*& buf, int & buf_len, bool& version_found)
{
	version_found = false;
	auto i_ver = get_version();
	int valid_len = buf_len - (int) sizeof(i_ver);
	for(int i=0;i< valid_len;i++)
	{
		auto p = buf + i;
		if( memcmp(p, &i_ver, sizeof(i_ver) ) == 0 )
		{
			version_found = true;
			buf += i;
			buf_len -= i;
			return;
		}
		return;// 不要搜索，只从第一个字符比较版本
	}
}

string_map_serializer::string_map_serializer(const char*buf, int buf_len)
	:m_is_text(-1)
	,m_data(buf)
	,m_data_len(buf_len)
	,m_version( get_version() )
{}

const char* string_map_serializer::get_buffer()const
{
	return m_data;
}

std::string string_map_serializer::get_buffer_as_string() const
{
	std::string s;
	if(get_buffer()&& get_buffer_length())
	{
		s.resize(get_buffer_length());
		memcpy(&s[0], get_buffer(), get_buffer_length());
	}
	return s;
}

int string_map_serializer::get_buffer_length()const
{
	return m_data_len;
}

bool string_map_serializer::parse()
{

	if(is_text())//判断是不是ansi字符串报文
		fields = string_map_create(get_buffer(),get_buffer_length());
	else//二进制报文
		fields = parse_binary();
	return true;
}

bool string_map_serializer::is_text() //是不是明文?
{
	if(m_is_text!=-1)
		return m_is_text==1;
	m_is_text= 0;

	auto p = get_buffer();
	for(int i=0;i<get_buffer_length();i++)
	{

		if( ::isdigit(p[i]) )
			continue;
		//应该是 '='
		if(p[i] == '=')
		{
			m_is_text=1;
			return true;
		}
		return false;//二进制报文
	}

	return false;
}

bool string_map_serializer::read_uint(const char*p, int& s, int len, unsigned int &v)
{
	v=0;
	if( len <= s) return false;
	if(len-s < sizeof(unsigned int))
		return false;
	v = *((unsigned int*)(p+s));
	s += sizeof(unsigned int);
	return true;
}
bool string_map_serializer::read_int(const char*p, int& s, int len, int &v)
{
	v=0;
	if( len <= s) return false;
	if(len-s < sizeof(int))
		return false;
	v = *((int*)(p+s));
	s += sizeof(int);
	return true;
}
bool string_map_serializer::read_int64(const char*p, int& s, int len, __int64 &v)
{
	v=0;
	if( len <= s) return false;
	if(len-s < sizeof(__int64))
		return false;
	v = *((__int64*)(p+s));
	s += sizeof(__int64);
	return true;
}

bool string_map_serializer::read_bin(const char*p, int& s, int len, int bin_len, string_item_ptr& s_bin)
{

//	s_bin = nullptr;
	if( len <= s) return false;
	if(len-s < bin_len)
		return false;
	//if(bin_len>0): 0 字节的字符串，也要创建字符串对象，否则数据类型（字符串）会丢失
	{
		s_bin = string_item_create(p+s,bin_len);
	}
	s += bin_len;
	return true;
}

bool string_map_serializer::read_bin(const char*p, int& s, int len, int bin_len, const char* bin)
{

	//	s_bin = nullptr;
	if( len <= s) return false;
	if(len-s < bin_len)
		return false;
	if(bin_len>0)
	{
		memcpy((void*)bin, p+s, bin_len);
	}
	s += bin_len;
	return true;
}


std::string string_map_serializer::to_string()
{
	if(is_text())//判断是不是ansi字符串报文
	{
		auto a = fields->to_string();
		std::string ret;
		ret.resize(a->size());
		if(ret.size())
		{
			memcpy(&ret[0], a->c_str(),a->size());
		}
		return ret;
	}
	//二进制
	return to_binary_string();
}

string_map_ptr string_map_serializer::parse_binary()
{
	if(fields==nullptr)
	{
		fields = string_map_create(0);
		//二进制格式: command_id,数据类型,数据长度,数据
		auto p = get_buffer();
		auto p_len = get_buffer_length();
		if( 0 != parse_binary(fields.get(), p, p_len))
			return nullptr;
	}
	return fields;
}

int string_map_serializer::read_value( const char* p, int& p_pos1, int p_len,string_item_ptr & v_str)
{
	int p_pos = p_pos1;
	//数据类型,目前都是字符串
	unsigned int value_type = 0;
	if(!read_uint(p,p_pos, p_len, value_type))
		return -1;//失败
	int v_i=0;
	__int64 v_i64=0;
	switch(value_type)
	{
	case SIT_STRING:
		{
			int str_len=0;//字符串长度
			if(!read_int(p,p_pos, p_len, str_len))
				return -1;//失败
			//读取字符串数据
			if(!read_bin(p,p_pos,p_len, str_len, v_str))
				return -1;
			break;
		}
	case SIT_INT:
		{
			if(!read_int(p,p_pos, p_len, v_i))
				return -1;//失败
			v_str = string_item_create(v_i);
		}
		break;
	case SIT_INT64:
		{
			if(!read_int64(p,p_pos, p_len, v_i64))
				return -1;//失败
			v_str = string_item_create(v_i64);
		}
		break;
	default:
		return -1;//不支持的协议
	}
	p_pos1 = p_pos;
	return 0;
}

std::string string_map_serializer::to_binary_string()
{
	return to_binary_string(fields.get());
}

void string_map_serializer::write_uint(std::ostringstream& ss, unsigned int v)
{
	ss.write((const char*)&v, sizeof(v));
}

void string_map_serializer::write_int(std::ostringstream& ss, int v)
{
	ss.write((const char*)&v, sizeof(v));
}

void string_map_serializer::write_int64(std::ostringstream& ss, __int64 v)
{
	ss.write((const char*)&v, sizeof(v));
}

void string_map_serializer::write_bin(std::ostringstream& ss, const char* p, int p_len)
{
	ss.write( p, p_len);
}

bool string_map_serializer::read_string(const char*p, int& s, int len, std::string& s_ret)
{
	unsigned int value_type = SIT_STRING;
	if(! read_uint(p, s, len, value_type))
		return false;
	assert(value_type == SIT_STRING);
	if(value_type != SIT_STRING)
		return false;
	//长度
	int str_len = 0;
	if( ! read_int(p, s, len, str_len))
		return false;
	s_ret.resize(str_len,0);
	if( str_len)
	{
		if( ! read_bin(p,s,len, str_len, s_ret.c_str()) )
			return false;
	}
	return true;
}

void string_map_serializer::write_string(std::ostringstream& ss, const std::string& s)
{
	unsigned int value_type = SIT_STRING;
	write_uint(ss, value_type);
	{
		const char* str_p = nullptr;
		int str_len=0;//字符串长度
		str_len = s.size();
		str_p = s.c_str();
		write_uint(ss, str_len);
		//读取字符串数据
		if(str_len)
			write_bin(ss, str_p, str_len);
	}
}

void string_map_serializer::write_value(std::ostringstream& ss, string_item_ptr s)
{
	switch(s->get_type())
	{
	case SIT_STRING:
		{
			const char* p = nullptr;
			int p_len = 0;
			if(s)
			{
				p = s->c_str();
				p_len = s->size();
			}
			write_string(ss, p, p_len);
		}
		break;
	case SIT_INT:
		write_uint(ss, SIT_INT);
		write_int(ss, (int)(intptr_t)s->get_data());
		break;
	case SIT_INT64:
		write_uint(ss, SIT_INT64);
		write_int64(ss, (__int64)s->get_data());
		break;
	}
}

void string_map_serializer::write_string(std::ostringstream& ss, const char* str_p, int str_len)
{
	unsigned int value_type = SIT_STRING;
	write_uint(ss, value_type);
	{
		write_uint(ss, str_len);
		//读取字符串数据
		if(str_len)
			write_bin(ss, str_p, str_len);
	}
}

std::string string_map_serializer::to_binary_string(const string_map* m)
{
	std::string ret;
	if(m==nullptr)
		return ret;
	//二进制格式: command_id,数据类型,数据长度,数据
	std::ostringstream ss;
	{
		//版本号
		{
			unsigned int v= get_version();
			write_uint(ss,v);
		}
		string_map_impl& map_i = *(string_map_impl*)m;
		for(auto it=map_i.m_map->begin(); it!=map_i.m_map->end();it++)
		{
			//读取单个key/value
			//key id
			auto key= it->first;
			write_value(ss, string_item_create(key.c_str()));
			write_value(ss, it->second);
		}
	}
	ret = ss.str();
	return ret;
}

int string_map_serializer::parse_binary(string_map* m, const char*p, int p_len)
{
	//跳过垃圾数据，找到报文头: wd1
	{
		bool version_found=false;
		find_version(p, p_len, version_found);
		if(!version_found) 
			return -1;
	}
	{
		auto fields = m;
		//二进制格式: command_id,数据类型,数据长度,数据
		int p_pos = 0;
		{
			{
				//版本号
				unsigned int version=0;
				if(!read_uint(p,p_pos, p_len,version))
					return -1;//失败
			}
			//	return -1;//版本不一致
			//读取所有key/value
			while(true)
			{
				//读取单个key/value
				//key id
				string_item_ptr v_key;

				if( 0 != read_value(p,p_pos, p_len, v_key))
					break;//没有key/value,数据结束

				string_item_ptr v_str;
				auto i_ret = read_value(p, p_pos, p_len, v_str);
				if( i_ret!=0)
					return i_ret;
				switch(v_key->get_type())
				{
				case SIT_STRING:
					fields->set( v_key->c_str(),v_str);//保存到map
					break;
				case SIT_INT:
					fields->set( INT2STR((int)(intptr_t)v_key->get_data()).c_str(), v_str);
					break;
				case SIT_INT64:
					fields->set( INT642STR((int)(intptr_t)v_key->get_data()).c_str(), v_str);
					break;
				}
			}
		}
	}
	return 0;//success
}



string_item_ptr string_map_to_bin_string(string_map* m)
{
	auto p = new string_item_impl();
	string_item_ptr ret( p );
	p->m_obj = string_map_serializer::to_binary_string(m);
	return ret;
}

std::shared_ptr<string_map> string_map_from_string(const char*p, int p_len)//自动判断是文本还是二进制
{
	std::shared_ptr<string_map> ret;
	string_map_serializer d(p,p_len);
	if(d.is_text())
	{
		return ret = string_map_create(p,p_len);
	}

	//二进制
	ret = std::make_shared<string_map_impl>();
	int i_ret = 0;
	i_ret = string_map_serializer::parse_binary( ret.get(),p,p_len);
	if(i_ret==0)
		return ret;
	//assert(false);
	return nullptr;
}
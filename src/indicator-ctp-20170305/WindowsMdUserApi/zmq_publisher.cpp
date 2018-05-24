#include <windows.h>
#include <shlwapi.h>

#include "zmq_publisher.h"

#include "../../include/string/utf8_string.h"
#include "../../include/config/bs_config.h"

std::string zmq_command_argument(const std::string& key);

zmq_publisher::zmq_publisher()
	:m_code_count(0)
{
	memset(&m_codes[0], 0, sizeof(m_codes));
}


zmq_publisher::~zmq_publisher()
{
}

int zmq_publisher::init_codes()
{
	m_code_count = (int)m_list_codes.size();
	for (size_t i = 0; i < m_list_codes.size(); i++)
	{
		m_codes[i] = (char*)m_list_codes[i].c_str();
	}
	return 0;
}


int zmq_publisher::init()
{
	//////////////////////////////////////////////////////////////////////////
	//命令行参数文件
	auto p_name = "ctp_codes";
	auto ini_file = zmq_command_argument(p_name);
	printf("parameters: /%s:%s\n", p_name, ini_file.c_str());
	if (ini_file.size() > 0)
	{
		if (::PathFileExistsA(ini_file.c_str()))
		{
			load_ini(ini_file.c_str());
		}
		else
		{
			std::vector<std::string> dels;
			dels.push_back(";");
			dels.push_back(",");
			m_list_codes = string_split(ini_file, dels);
		}
	}
	else
	{
		m_list_codes.push_back("a1705");
		m_list_codes.push_back("m1705");
		m_list_codes.push_back("i1705");

		m_list_codes.push_back("a1709");
		m_list_codes.push_back("m1709");
		m_list_codes.push_back("i1709");

		m_list_codes.push_back("a1801");
		m_list_codes.push_back("m1801");
		m_list_codes.push_back("i1801");
	}

	//////////////////////////////////////////////////////////////////////////
	init_codes();
	return 0;
}

int zmq_publisher::load_ini(const char* ini_path)
{
	printf("load ini from file:%s\n", ini_path);

	if (!PathFileExistsA(ini_path))
		return -1;
	//read codes
	std::vector<std::string> lines;
	auto ret = cosmos::bs_config::read_file_lines(ini_path, lines);
	if (ret)
		return ret;
	m_list_codes = lines;
	init_codes();
	return 0;
}

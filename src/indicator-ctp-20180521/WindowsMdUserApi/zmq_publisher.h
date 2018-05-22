#pragma once
#include <string>
#include <vector>

class zmq_publisher
{
public:
	zmq_publisher();
	~zmq_publisher();

	int init();

	int load_ini(const char* init_path);

	char*  m_codes[1024];
	int    m_code_count;

protected:
	std::vector<std::string> m_list_codes;

	int init_codes();
};


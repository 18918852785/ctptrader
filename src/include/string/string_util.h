#pragma once

#include <string>
#include <vector>

namespace cosmos{

class string_util
{
public:
	string_util(void);
	~string_util(void);

	static int SplitString(
		const std::string& input, 
		const std::string& delimiter, 
		std::vector<std::string>& results, 
		bool includeEmpties
		);


	static int SplitKeyValue(
		const std::string& input, 
		std::vector<std::string>& keys, 
		std::vector<std::string>& values, 
		bool includeEmpties,
		std::string field_split_char,//字段之间的分割符: ,
		std::string value_split_char //值的分割符      : =
		);

	static bool isEqual(const std::string& a, const std::string& b);
};

}//namespace cosmos
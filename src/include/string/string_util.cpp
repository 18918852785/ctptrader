#include "string_util.h"

#include "string_convert.h"

using namespace std;

namespace cosmos{

string_util::string_util(void)
{
}


string_util::~string_util(void)
{
}

//http://www.codeproject.com/Articles/1114/STL-Split-String
//STL split string
int string_util::SplitString(
	const string& input, 
	const string& delimiter, 
	vector<string>& results, 
	bool includeEmpties)
{
	int iPos = 0;
	int newPos = -1;
	int sizeS2 = (int)delimiter.size();
	int isize = (int)input.size();

	if( 
		( isize == 0 )
		||
		( sizeS2 == 0 )
		)
	{
		return 0;
	}

	vector<int> positions;

	newPos = (int)input.find (delimiter, 0);

	if( newPos < 0 )
	{ 
		if( input.length() > 0 )
		{
			results.push_back(input);
		} 
		return 0; 
	}

	int numFound = 0;

	while( newPos >= iPos )
	{
		numFound++;
		positions.push_back(newPos);
		iPos = newPos;
		newPos = (int)input.find (delimiter, iPos+sizeS2);
	}

	if( numFound == 0 )
	{
		return 0;
	}

	for( size_t i=0; i <= (int)positions.size(); ++i )
	{
		string s("");
		if( i == 0 ) 
		{ 
			s = input.substr( i, positions[i] ); 
		}
		if(i>=1 && i<=positions.size()){
			int offset = positions[i-1] + sizeS2;
			if( offset < isize )
			{
				if( i == positions.size() )
				{
					s = input.substr(offset);
				}
				else if( i > 0 )
				{
					s = input.substr( positions[i-1] + sizeS2, 
						positions[i] - positions[i-1] - sizeS2 );
				}
			}
		}
		if( includeEmpties || ( s.size() > 0 ) )
		{
			results.push_back(s);
		}
	}
	return numFound;
}

bool string_util::isEqual(const std::string& a, const std::string& b)
{
	return ( _stricmp(a.c_str(), b.c_str()) == 0);
}

int string_util::SplitKeyValue(
	const std::string& input, 
	std::vector<std::string>& keys, 
	std::vector<std::string>& values, 
	bool includeEmpties,
	std::string field_split_char,//字段之间的分割符: ,
	std::string value_split_char //值的分割符      : =		
	)
{
	vector<string> fields;
	SplitString(input, field_split_char/*","*/, fields, includeEmpties);
	for(size_t i = 0;i<fields.size();i++)
	{
		string field = fields[i];
		vector<string> f;
		SplitString( field,value_split_char/*"="*/,f,true);

		keys.insert(keys.end(), f[0]);
		string v;
		if(f.size()>=2)
		{
			v = f[1];
		}
		values.insert(values.end(), v);
	}
	return (int)fields.size();
}


}//namespace cosmos
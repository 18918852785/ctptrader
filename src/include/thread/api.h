#pragma once
#ifdef _WIN32
#else
#include <unistd.h>
#include <string.h>
#endif

//////////////////////////////////////////////////////////////////////////
inline bool api_get_exe_fullpath_(char * exeName)
{
	bool ret = false;
	char path[_max_path] = { 0 };
#ifdef _WIN32
	ret = ::GetModuleFileNameA(NULL, exeName, _max_path) != 0;
	return ret;
#else
	//http://blog.csdn.net/ljx0305/article/details/9152523
	//从/proc/self/exe获取
#define MAXBUFSIZE _MAX_PATH
	unsigned size = _MAX_PATH;
	char buf[_max_path] = { 0 };
	int count = 0;
	count = readlink("/proc/self/exe", buf, MAXBUFSIZE);
	if (count < 0 || count >= MAXBUFSIZE) {
		return false;
	}

	buf[count] = '\0';
	//PRINTF("exeName:%p, exe path:%s\n", exeName, buf);
	strcpy(exeName, buf);
	return(true);
#endif
}

inline char* api_PathFindFileNameA_(char* pszPath)
{
	if (!pszPath)
		return pszPath;
	//找到最后一个 '/'
	auto plast = strrchr(pszPath, PATH_SPLIT_CHAR);
	if (plast)
		return plast+1;
	return pszPath;
}


inline bool api_get_executable_path_(unsigned int size, char * exepath)
{
	if (! api_get_exe_fullpath_(exepath))
		return false;
	char* p = strrchr(exepath, PATHSEPCHAR);
	*(p) = 0;

	return true;
}
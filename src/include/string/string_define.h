#pragma once

#ifndef INDICATOR_DATA_EXPORT
#define INDICATOR_DATA_EXPORT
#endif

#include <memory>
#include <string>
#ifdef _WIN32

	#define PATH_SPLIT_CHAR '\\'
	#define PATHSEP			"\\"
	#define PATHSEPCHAR		'\\'
	#define DLLPREFIX		""
	#define DLLSUFFIX		".dll"
	#define EXESUFFIX		".exe"

	#define DLLEXPORT __declspec(dllexport) 
#else//linux
	#define PATH_SPLIT_CHAR '/'
	#define PATHSEP			"/"
	#define PATHSEPCHAR		'/'
	#define DLLPREFIX		"lib"
	#define DLLSUFFIX		".so"
	#define EXESUFFIX		""
	#define PARENTDIR		"../"

	#define _tmain main
	#define TCHAR char
	#define _T(a) a

	#include <stdint.h>

	#define __declspec(EXP)  
	#define DLLEXPORT /*extern*/
	//#define dllimport 
	#define AFX_MANAGE_STATE(MOD)

	class CWinApp {};
#endif

#define _max_path 260
#ifndef _MAX_PATH
#define _MAX_PATH _max_path
#endif
#ifndef MAX_PATH
#define MAX_PATH _max_path
#endif


typedef std::string    std_string;
#define std_shared_ptr std::shared_ptr 
#define std_vector     std::vector
#define std_make_shared std::make_shared
#define std_unordered_map std::unordered_map

#ifdef _WIN32
#else //linux
#define INVALID_HANDLE_VALUE	((void*)-1)
#define TRUE ((BOOL)1)
#define FALSE ((BOOL)0)
#define ERROR_SUCCESS 0
#define WAIT_OBJECT_0	((unsigned int)0)
#define WAIT_FAILED		((unsigned int)-1)
#define WAIT_TIMEOUT	((unsigned int)258)
#define INFINITE		((unsigned int)-1)
#endif

#ifdef _WIN32
	#define api_GetCurrentProcessId GetCurrentProcessId
	#define api_GetTickCount GetTickCount
	#define api_GetCommandLineA ::GetCommandLineA
	#define api_SetCommandLineA(a,b) 

	#define api_beginthreadex _beginthreadex
	#define api_GetCurrentThreadId GetCurrentThreadId
	#define api_ZeroMemory ZeroMemory
	#define api_Sleep Sleep
	#define api_InterlockedIncrement InterlockedIncrement
#define api_CreateMutex CreateMutex
#define api_CloseHandle CloseHandle
#define api_ReleaseMutex ReleaseMutex
#define api_WaitForSingleObject WaitForSingleObject
#define api_WaitForThread WaitForThread
#define api_GetLastError  GetLastError
#define api_SetLastError SetLastError

#define api_LoadLibraryA LoadLibraryA
#define api_GetModuleFileNameA GetModuleFileNameA
#define api_GetModuleHandle GetModuleHandle
#define api_GetProcAddress GetProcAddress

#define api_PathAppendA PathAppendA
#define api_PathRemoveFileSpecA PathRemoveFileSpecA
#define api_PathCombineA PathCombineA
#define api_PathFileExistsA PathFileExistsA

#define api_get_exe_fullpath api::get_exe_fullpath 
#define api_get_executable_path api_get_executable_path_
#define api_CreateDirectoryA CreateDirectoryA
#define api_FileTimeToSystemTime FileTimeToSystemTime

#else//linux
	int _stricmp(const char *s1, const char *s2);

	#define ExitProcess exit
	#define _atoi64(val)     strtoll(val, NULL, 10)
	#define api_GetCurrentProcessId api::GetCurrentProcessId
	#define api_GetTickCount api::GetTickCount
	#define api_GetCommandLineA api::GetCommandLineA
	#define api_SetCommandLineA(a,b) api::SetCommandLineA(a,b)

	#define api_beginthreadex api::_beginthreadex
	#define api_GetCurrentThreadId api::GetCurrentThreadId
	#define api_ZeroMemory api::ZeroMemory
	#define api_Sleep api::Sleep
	#define api_InterlockedIncrement api::InterlockedIncrement
#define api_CreateMutex api::CreateMutex
#define api_CloseHandle api::CloseHandle
#define api_GetLastError() errno
#define api_SetLastError api::SetLastError
#define api_ReleaseMutex api::ReleaseMutex
#define api_WaitForSingleObject api::WaitForSingleObject
#define api_WaitForThread api::WaitForThread

#define api_GetModuleFileNameA api::GetModuleFileNameA
#define api_GetModuleHandle api::GetModuleHandle
#define api_LoadLibraryA api::LoadLibraryA
#define api_GetProcAddress api::GetProcAddress

#define api_PathAppendA api::PathAppendA
#define api_PathRemoveFileSpecA api::PathRemoveFileSpecA
#define api_PathCombineA api::PathCombineA
#define api_PathFileExistsA api::PathFileExistsA
#define api_get_exe_fullpath api::get_exe_fullpath 

#define api_CreateDirectoryA api::CreateDirectoryA
#define api_FileTimeToSystemTime api::FileTimeToSystemTime

#define OutputDebugStringA PRINTF
#define CoInitializeEx(a,b)
#define CoUninitialize() 

	typedef void *HANDLE;
	typedef void *HMODULE;

	typedef int64_t __int64;
	typedef uint64_t uint64;

	typedef int INT_PTR, *PINT_PTR;
	typedef unsigned int UINT_PTR, *PUINT_PTR;

	typedef void* MY_LPSECURITY_ATTRIBUTES;
	typedef const char 		*LPCSTR;
	typedef const char 		*LPCTSTR;
	typedef char			*LPSTR;
	typedef char			*LPTSTR;

	typedef unsigned int	UINT;
	typedef int				INT;
	#define WCHAR wchar_t
	#define LPCWSTR const WCHAR*
	typedef WCHAR *NWPSTR, *LPWSTR, *PWSTR;

	typedef long int intptr_t;
	typedef unsigned long int uintptr_t;

	typedef unsigned long   DWORD, *LPDWORD;
	typedef unsigned short  WORD;
	typedef WORD            LANGID;
	typedef void            VOID;
	typedef void*           PVOID;
	typedef int 			BOOL;

	#define CONST const
	#define LONG long
	typedef int64_t LONGLONG;
	typedef uint64_t ULONGLONG;

	typedef struct FILETIME {
		DWORD dwLowDateTime;	/* low 32 bits  */
		DWORD dwHighDateTime;	/* high 32 bits */
	} FILETIME, *PFILETIME, *LPFILETIME;

	#define _taccess access
	#define __stdcall
	#define _stdcall 
#define CALLBACK
	typedef int (CALLBACK *FARPROC)();


	typedef struct _SYSTEMTIME {
		WORD wYear;
		WORD wMonth;
		WORD wDayOfWeek;
		WORD wDay;
		WORD wHour;
		WORD wMinute;
		WORD wSecond;
		WORD wMilliseconds;
	} SYSTEMTIME, *LPSYSTEMTIME;
#endif

bool api_get_exe_fullpath_(char * exeName);

#ifdef _WIN32
#else
	#if ! defined(int64_t)
	#define int64_t 	__int64
	#endif


	char* _itoa(int a, char* b, int c);
	char* _i64toa(long long i, char* buf, int ord);
	int _stricmp(const char *s1, const char *s2);
	#define _snprintf		snprintf
	#define _snprintf_s		sprintf_s
	#define sprintf_s(buf, size, f, ...) sprintf(buf, f, ##__VA_ARGS__)

	#define itoa _itoa

	class api {
	public:
		static LONG InterlockedIncrement(volatile LONG*);
		static LONG InterlockedDecrement(volatile LONG*);
		static LONG InterlockedExchange(volatile LONG *p, LONG v);
		static LONG InterlockedExchangeAdd(LONG volatile *p, LONG v);

		static HANDLE CreateMutex(MY_LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName);
		static DWORD WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);
		static DWORD WaitForThread(HANDLE hHandle, DWORD dwMilliseconds);
		static BOOL ReleaseMutex(HANDLE hMutex);
		static BOOL CloseHandle(HANDLE h);
		static VOID  SetLastError(DWORD dwErrCode);
		static VOID Sleep(DWORD millSeconds);
		static DWORD GetTickCount();
		static DWORD GetCurrentProcessId();


		static const char* GetCommandLineA();
		static const char* SetCommandLineA(const char* cmd);
		static const char* SetCommandLineA(int argc, char* argv[]);

		static HMODULE LoadLibraryA(LPCSTR lpLibFileName);
		static HMODULE GetModuleHandle(LPCTSTR lpModuleName);
		static FARPROC GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
		static FARPROC GetProcAddress2(HMODULE hModule, LPCSTR lpProcName, std::string &err);
		static DWORD GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize);
		
		static LPSTR PathAppendA(LPSTR dest, LPCSTR append);
		static LPCSTR PathFindFileNameA(LPCSTR pszPath);
		static BOOL PathRemoveFileSpecA(LPSTR pszPath);
		static LPSTR  PathCombineA(LPSTR pszDest, LPCSTR pszDir, LPCSTR pszFile);
		static BOOL  PathFileExistsA(LPCSTR pszPath);

		static BOOL CreateDirectoryA(LPCSTR lpPathName, MY_LPSECURITY_ATTRIBUTES lpSecurityAttributes);

		static char* get_lib_name( const char* path,  const char* libname);
		static void free_lib_name( char*& fullname);
		static HMODULE load_library( const char* fullname);
		static void free_library(HMODULE lib);
		static void* find_symbol(HMODULE lib,  const char* symbol);

		static bool get_exe_fullpath(char* exeName);		//exe文件路径：包括文件名
		static bool get_executable_path(uint32_t size, char* exepath);

		static BOOL FileTimeToSystemTime(CONST FILETIME *lpFileTime, LPSYSTEMTIME lpSystemTime);

#define RC_THREAD_NOT_CREATED ((LPDWORD)(-1))
		static HANDLE _beginthreadex(
			void *security,
			unsigned stack_size,
#ifdef _WIN32
			unsigned(*start_address)(void *),
#else
			void* (*start_address)(void *),
#endif
			void *arglist,
			unsigned initflag,
			unsigned *thrdaddr
		);
		static unsigned long GetCurrentThreadId();
		static void      ZeroMemory(void*, unsigned int);

	};
#endif

namespace cosmos{

}//namespace cosmos

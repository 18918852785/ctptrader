
#pragma region Includes
#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include "filemapping.h"
#include "../logger/print.h"
#pragma endregion

#define _T(a) a
#define TCHAR char

namespace filemapping
{
#ifdef _WIN32
filemapping_client::filemapping_client()
	:hMapFile(0)
	,pBuf(0)
	,pBuf_len(0)
{
	// prefix to explicitly create the object in the global or session 
	// namespace. The remainder of the name can contain any character except 
	// the backslash character (\). For details, please refer to:
	// http://msdn.microsoft.com/en-us/library/aa366537.aspx
	TCHAR szMapFileName[] = BSNEWS_FILEMAPPING_NAME;//_T("Local\\HelloWorld");

	// Open the named file mapping.
	hMapFile = OpenFileMappingA(
		FILE_MAP_ALL_ACCESS,	// Read/write access
		FALSE,					// Do not inherit the name
		szMapFileName			// Name of mapping object 
		);

	if (hMapFile == NULL) 
	{ 
		PRINTF(_T("Unable to open file mapping object w/err 0x%08lx\n"), 
			GetLastError());
		return;
	}
	PRINTF(_T("The file mapping object, %s, is opened.\n"), szMapFileName);


	/////////////////////////////////////////////////////////////////////////
	// Maps the view of the file mapping into the address space of the 
	// current process.
	// 

	// Create file view from the file mapping object.
	pBuf = (LPTSTR) MapViewOfFile(
		hMapFile,				// Handle of the map object
		FILE_MAP_ALL_ACCESS,	// Read/write permission
		0,						// A high-order DWORD of the file offset 
								// where the view begins.
		0,						// A low-order DWORD of the file offset 
								// where the view is to begin. 
		pBuf_len				// The number of bytes of a file mapping 
								// to map to the view.
		);

	if (pBuf == NULL) 
	{ 
		PRINTF(_T("Unable to map view of file w/err 0x%08lx\n"), 
			GetLastError()); 
		CloseHandle(hMapFile);
		return;
	}
	PRINTF(_T("The file view is created.\n"));
}

int filemapping_client::copy()
{
	/////////////////////////////////////////////////////////////////////////
	// Show the message in shared memory.
	// 

	PRINTF(_T("The following message is read from the shared memory:\n") \
		_T("\"%s\"\n"), pBuf);
	return 0;
}

filemapping_client::~filemapping_client()
{
	/////////////////////////////////////////////////////////////////////////
	// Unmap the file view and close the file mapping objects.
	// 

	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);

	return;
}

#endif
}//namespace filemapping

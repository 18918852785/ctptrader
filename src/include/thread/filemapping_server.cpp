
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


filemapping_server::filemapping_server()
	:hMapFile(0)
	,pBuf(0)
	,pBuf_len(0)
{
	pBuf_len = BSNEWS_FILEMAPPING_BUFFER_SIZE;

	/////////////////////////////////////////////////////////////////////////
	// Create a file mapping.
	// 

	// In terminal services: The name can have a "Global\" or "Local\" prefix 
	// to explicitly create the object in the global or session namespace.  
	// The remainder of the name can contain any character except the  
	// backslash character (\). For details, please refer to:
	// http://msdn.microsoft.com/en-us/library/aa366537.aspx
	TCHAR szMapFileName[] = BSNEWS_FILEMAPPING_NAME;//_T("Local\\HelloWorld");

	// Create the file mapping object
	hMapFile = CreateFileMappingA(
		INVALID_HANDLE_VALUE,	// Use paging file instead of existing file.
								// Pass file handle to share in a file.
		NULL,					// Default security 
		PAGE_READWRITE,			// Read/write access
		0,						// Max. object size 
		pBuf_len,			// Buffer size  
		szMapFileName			// Name of mapping object
		);

	if (hMapFile == NULL) 
	{ 
		PRINTF(_T("Unable to create file mapping object w/err 0x%08lx\n"), 
			GetLastError());
		return;
	}
	PRINTF(_T("The file mapping object, %s, is created.\n"), szMapFileName);


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

	//Çå¿ÕÄÚ´æ
	ZeroMemory(pBuf, pBuf_len);
}


int filemapping_server::copy( const char* szMessage)
{
	/////////////////////////////////////////////////////////////////////////
	// Write message to the file view.
	// 

	// Write the message to the file view.
	int cbMessageBytes = (int)(strlen(szMessage) + 1 /* NULL */) * sizeof(TCHAR);
	CopyMemory((PVOID)pBuf, szMessage, cbMessageBytes);
	
	PRINTF(_T("The following message is written to the shared memory:\n") \
		_T("\"%s\"\n"), szMessage);

	return 0;
}

filemapping_server::~filemapping_server()
{
	/////////////////////////////////////////////////////////////////////////
	// Unmap the file view and close the file mapping objects.
	// 

	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
}


}//namespace filemapping


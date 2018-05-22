//http://www.programering.com/a/MTO1EDNwATY.html

#include "file_sem.h"

#ifdef _WIN32
#else
#endif

namespace thread {
#ifdef _WIN32
	file_sem::file_sem(const char* name)
	{
		memset(m_cMutexName, 0, sizeof(m_cMutexName));
		int min = strlen(name)>(sizeof(m_cMutexName) - 1) ? (sizeof(m_cMutexName) - 1) : strlen(name);
		strncpy(m_cMutexName, name, min);
		m_pMutex = CreateMutexA(NULL, false, m_cMutexName);
	}
	file_sem::~file_sem()
	{
		CloseHandle(m_pMutex);
	}
	bool file_sem::lock()
	{
		//Failed to create a mutex lock
		if (NULL == m_pMutex)
		{
			return false;
		}
		DWORD nRet = WaitForSingleObject(m_pMutex, INFINITE);
		if (nRet != WAIT_OBJECT_0)
		{
			return false;
		}
		return true;
	}

	bool file_sem::unlock()
	{
		return ReleaseMutex(m_pMutex);
	}

#else
	file_sem::file_sem(const char* name)
	{
		char path[256];
		strcpy(path, "");
		strcat(path, name);
		m_f = fopen(path,"r+");
	}
	file_sem::~file_sem()
	{
		if(m_f)
			fclose(m_f);
	}
	bool file_sem::lock()
	{
		if (!m_f)
			return false;
		return 0 == lockf(fileno(m_f),F_LOCK,0L);
		//fscanf(in,"t%d"t,&ptemp);
		//temp++;
		//fseek(in,0L,0);
		//fprintf(in,"t%d"t,temp);
		//printf("t\n\tid = %s\tnum = %d"t,argv[1],temp);
	}
	bool file_sem::unlock()
	{
		if (!m_f)
			return false;
		return 0 == lockf(fileno(m_f),F_ULOCK,0L);
	}

#endif

}//namespace thread
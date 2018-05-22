//http://www.programering.com/a/MTO1EDNwATY.html

#include "process_sem.h"

#ifdef _WIN32
#else
#endif

namespace thread {
#ifdef _WIN32
	process_sem::process_sem(const char* name)
	{
		memset(m_cMutexName, 0, sizeof(m_cMutexName));
		int min = strlen(name)>(sizeof(m_cMutexName) - 1) ? (sizeof(m_cMutexName) - 1) : strlen(name);
		strncpy(m_cMutexName, name, min);
		m_pMutex = CreateMutexA(NULL, false, m_cMutexName);
	}
	process_sem::~process_sem()
	{
		CloseHandle(m_pMutex);
	}
	bool process_sem::lock()
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

	bool process_sem::unlock()
	{
		return ReleaseMutex(m_pMutex);
	}

#else
	process_sem::process_sem(const char* name)
	{
		memset(m_cMutexName, 0, sizeof(m_cMutexName));
		int min = strlen(name)>(sizeof(m_cMutexName) - 1) ? (sizeof(m_cMutexName) - 1) : strlen(name);
		strncpy(m_cMutexName, name, min);
		m_pSem = sem_open(name, O_RDWR | O_CREAT, 0644, 1);
	}
	process_sem::~process_sem()
	{
		int ret = sem_close(m_pSem);
		if (0 != ret)
		{
			printf("sem_close error %d\n", ret);
		}
		sem_unlink(m_cMutexName);
	}
	bool process_sem::lock()
	{
		int ret = sem_wait(m_pSem);
		if (ret != 0)
		{
			return false;
		}
		return true;
	}
	bool process_sem::unlock()
	{
		int ret = sem_post(m_pSem);
		if (ret != 0)
		{
			return false;
		}
		return true;
	}

#endif

}//namespace thread
#pragma once

#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <memory.h>
#endif

namespace thread
{
	class file_sem{
	public:
		/* The default create anonymous mutual exclusion */
		file_sem(const char* name = NULL);
		~file_sem();
		bool lock();
		bool unlock();
	private:
#ifdef _WIN32
		char m_cMutexName[32];
		HANDLE m_pMutex;
#else
		FILE* m_f;
#endif
};

}//namespace
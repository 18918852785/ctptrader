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
	class process_sem{
	public:
		/* The default create anonymous mutual exclusion */
		process_sem(const char* name = NULL);
		~process_sem();
		bool lock();
		bool unlock();
	private:
#ifdef _WIN32
		void* m_pMutex;
#else
		sem_t* m_pSem;
#endif
		char m_cMutexName[30];
};

}//namespace
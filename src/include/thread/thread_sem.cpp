///https://linux.die.net/man/3/pthread_mutex_init
///https://linux.die.net/man/3/pthread_mutexattr_init

#include "thread_sem.h"

#ifdef _WIN32
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#endif

namespace thread{
#ifdef _WIN32
#else
	semaphore_t *
		semaphore_create(const char *semaphore_name,int init_count)
	{
		int fd;
		semaphore_t *semap;
		pthread_mutexattr_t psharedm;
		pthread_condattr_t psharedc;

		fd = open(semaphore_name, O_RDWR | O_CREAT | O_EXCL, 0666);
		if (fd < 0)
			return (NULL);
		(void)ftruncate(fd, sizeof(semaphore_t));
		(void)pthread_mutexattr_init(&psharedm);
		(void)pthread_mutexattr_setpshared(&psharedm,
			PTHREAD_PROCESS_SHARED);
		(void)pthread_condattr_init(&psharedc);
		(void)pthread_condattr_setpshared(&psharedc,
			PTHREAD_PROCESS_SHARED);
		semap = (semaphore_t *)mmap(NULL, sizeof(semaphore_t),
			PROT_READ | PROT_WRITE, MAP_SHARED,
			fd, 0);
		close(fd);
		(void)pthread_mutex_init(&semap->lock, &psharedm);
		(void)pthread_cond_init(&semap->nonzero, &psharedc);
		semap->count = init_count;
		return (semap);
	}

	semaphore_t *
		semaphore_open(const char *semaphore_name)
	{
		int fd;
		semaphore_t *semap;

		fd = open(semaphore_name, O_RDWR, 0666);
		if (fd < 0)
			return (NULL);
		semap = (semaphore_t *)mmap(NULL, sizeof(semaphore_t),
			PROT_READ | PROT_WRITE, MAP_SHARED,
			fd, 0);
		close(fd);
		return (semap);
	}

	void
		semaphore_post(semaphore_t *semap)
	{
		pthread_mutex_lock(&semap->lock);
		if (semap->count == 0)
			pthread_cond_signal(&semap->nonzero);
		semap->count++;
		pthread_mutex_unlock(&semap->lock);
	}

	void
		semaphore_wait(semaphore_t *semap)
	{
		pthread_mutex_lock(&semap->lock);
		while (semap->count == 0)
			pthread_cond_wait(&semap->nonzero, &semap->lock);
		semap->count--;
		pthread_mutex_unlock(&semap->lock);
	}

	void
		semaphore_close(semaphore_t *semap)
	{
		munmap((void *)semap, sizeof(semaphore_t));
	}

	thread_sem::thread_sem(const char * name,bool create)
	{
		m_created = 0;
		if (name)
		{
			m_name = "/tmp/";
			m_name += name;
		}
		if (create)//试图创建
			semap = semaphore_create(m_name.c_str(), 0);
		else
			semap = semaphore_open(m_name.c_str());

		if (create && semap)//成功
		{
			m_created = 1;
			return;
		}
	}
	thread_sem::~thread_sem()
	{
		if (semap)
		{
			semaphore_close(semap);
			semap = 0;
		}
	}
	long thread_sem::lock()
	{
		if (!semap)
			return -1;
		semaphore_wait(semap);
		return 0;
	}
	long thread_sem::lock(long millseconds)
	{
		return lock();
	}
	long thread_sem::unlock()
	{
		if (!semap)
			return -1;
		semaphore_post(semap);
		return 0;
	}

#endif

}//namespace thread


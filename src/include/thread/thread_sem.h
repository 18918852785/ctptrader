#pragma once
#include <memory>
#include <unordered_map>
#include <queue>
#include <string>
#include <functional>

namespace thread
{
#ifdef _WIN32
#else
	struct semaphore {
		pthread_mutex_t lock;
		pthread_cond_t nonzero;
		unsigned count;
	};
	typedef struct semaphore semaphore_t;

	///https://linux.die.net/man/3/pthread_mutex_init
	///https://linux.die.net/man/3/pthread_mutexattr_init

	semaphore_t *semaphore_create(char *semaphore_name);
	semaphore_t *semaphore_open(char *semaphore_name);
	void semaphore_post(semaphore_t *semap);
	void semaphore_wait(semaphore_t *semap);
	void semaphore_close(semaphore_t *semap);

	class thread_sem
	{
	public:
		thread_sem(const char* name,bool create);
		~thread_sem();

		long           lock();
		long           lock(long millseconds);
		long           unlock();

		semaphore_t* semap;
	private:
		std::string m_name;
		int m_created;
	};
	typedef std::shared_ptr<thread_sem> thread_sem_ptr;
#endif//linux
}//namespace

#ifndef _HELPER_FASTMUTEX_2020_01_15_12_32_01_H
#define _HELPER_FASTMUTEX_2020_01_15_12_32_01_H

#include "Types.h"

namespace ws {

template <class M>
class ScopedLockT {
	kDisableCopyAssign(ScopedLockT);

public:
	explicit ScopedLockT(M& mutex) : _mutex(mutex) {
		_mutex.lock();
	}

	~ScopedLockT() {
		_mutex.unlock();
	}

private:
	M& _mutex;
};

class FastMutex {
	kDisableCopyAssign(FastMutex);

public:
	typedef ScopedLockT<FastMutex> ScopedLock;

public:
	FastMutex() {
		pthread_mutexattr_t mt;
		pthread_mutexattr_init(&mt);
		pthread_mutexattr_settype(&mt, PTHREAD_MUTEX_RECURSIVE_NP);

		pthread_mutex_init(&_cs, &mt);

		pthread_mutexattr_destroy(&mt);
	}

	~FastMutex() {
		pthread_mutex_destroy(&_cs);
	}

public:
	void lock() {
		pthread_mutex_lock(&_cs);
	}

	bool tryLock() {
		return 0 == pthread_mutex_trylock(&_cs);
	}

	bool tryLock(i32 milliseconds) {
		const i32 t = 20;
		struct timespec tv = { 0, t * 1000 * 1000};

		do {
			if(0 == pthread_mutex_timedlock(&_cs, &tv)) {
				return true;
			}

			usleep(t * 1000);
			milliseconds -= t;
		} while(0 < milliseconds);

		return false;
	}

	void unlock() {
		pthread_mutex_unlock(&_cs);
	}

private:
	pthread_mutex_t _cs;
};

}

#endif //_HELPER_FASTMUTEX_2020_01_15_12_32_01_H
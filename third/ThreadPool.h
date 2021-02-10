#ifndef _LIBCOMMON_THREADPOOL_2020_01_15_12_07_28_H
#define _LIBCOMMON_THREADPOOL_2020_01_15_12_07_28_H

#include <Types.h>
#include <FastMutex.h>
#include <Uncopyable.h>

namespace ws {

class ThreadPool {
	typedef std::function<void()> Job;

	kDisableCopyAssign(ThreadPool);

public:
	explicit ThreadPool(u32 threads = 0);
	~ThreadPool();

public:
	u32 size();
	void stop();
	void clear();
	void start(u32 threads);
	void post(const Job& task);

private:
	void run();
	Job getJob();

private:
	std::mutex _fmJobs;
	std::mutex _fmThreads;
	std::queue<Job> _jobs;
	volatile bool _running;
	std::vector<std::thread*> _threads;
	std::condition_variable  _condition;
};

}

#endif //_LIBCOMMON_THREADPOOL_2020_01_15_12_07_28_H
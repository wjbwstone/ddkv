#include "ThreadPool.h"

namespace ws {

ThreadPool::ThreadPool(u32 threads /* = 0 */) : _running(false) {
	start(threads);
}

ThreadPool::~ThreadPool() {
	stop();
}

void ThreadPool::post(const Job& task) {
	std::unique_lock<std::mutex> lock(_fmJobs);
	if(_running) {
		_jobs.push(task);
		_condition.notify_one();
	}
}

ThreadPool::Job ThreadPool::getJob() {
	std::unique_lock<std::mutex> lock(_fmJobs);
	if(_running) {
		if(_jobs.empty()) {
			_condition.wait(lock);
		}

		if(!_jobs.empty()) {
			Job job(_jobs.front());
			_jobs.pop();
			return job;
		}
	}

	return Job(nullptr);
}

void ThreadPool::stop() {
	{
		std::unique_lock<std::mutex> lock(_fmJobs);
		_running = false;
		std::queue<Job> em;
		_jobs.swap(em);
	}

	_condition.notify_all();

	{
		std::unique_lock<std::mutex> lock(_fmThreads);
		for(auto& it : _threads) {
			it->join();
			delete it;
		}
		_threads.clear();
	}
}

void ThreadPool::clear() {
	std::unique_lock<std::mutex> lock(_fmJobs);
	std::queue<Job> em;
	_jobs.swap(em);
}

u32 ThreadPool::size() {
	std::unique_lock<std::mutex> lock(_fmJobs);
	return (u32)_jobs.size();
}

void ThreadPool::start(u32 threads) {
	if(0 != threads) {
		bool needSetRunning = false; {
			std::unique_lock<std::mutex> lock(_fmThreads);
			if(_threads.empty()) {
				needSetRunning = true;
			}
		}

		if(needSetRunning) {
			std::unique_lock<std::mutex> lock(_fmJobs);
			_running = true;
		}

		if(needSetRunning) {
			std::unique_lock<std::mutex> lock(_fmThreads);
			for(u32 i = 0; i < threads; ++i) {
				auto t = new std::thread(std::bind(&ThreadPool::run, this));
				_threads.push_back(t);
			}
		}
	}
}

void ThreadPool::run() {
	while(true) {
		Job job = getJob();
		if(nullptr != job) {
			job();
		} else {
			return;
		}
	}
}

}
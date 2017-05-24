#include <unistd.h>
#include <string.h>

#include <string>
#include <memory>
#include <algorithm>
#include <sstream>
#include <iostream>

#include "thread.hpp"

using namespace std;

Thread::Thread(const ThreadFunc &func, void *data, const string &name, bool detach) : data_(data), name_(name), 
		detach_(detach)
{
	set_thread_func(func);
}

Thread::Thread(const std::string &name, bool detach): name_(name), detach_(detach)
{
}

void Thread::start(void)
{
	if (!func_) {
		stringstream error;

		error << "Thread(" << name_ << ") doesn't set the thread func" << endl;
		throw runtime_error(error.str());
	}

	ThreadData *thread_data = new ThreadData(func_, data_, name_);

	if (pthread_create(&thread_id_, NULL, Thread::start_thread, thread_data)) {
		delete thread_data;
		cerr << "Fail to create thread %s" << name_ << endl;
	}

	if (name_.size()) {
		if (0 != pthread_setname_np(thread_id_, name_.c_str())) {
			cerr << "Fail to set thread name(" << name_ << ")" << endl;
			cerr << "Error: " << strerror(errno) << endl;
		}
	}

	if (detach_) {
		if (0 != pthread_detach(thread_id_)) {
			cerr << "Fail to detach thread" << endl;
		}
	}
}

void* Thread::start_thread(void* data)
{
	ThreadData *thread_data = static_cast<ThreadData*>(data);

	thread_data->func_(thread_data->data_);

	delete thread_data;

	return NULL;
}

void Thread::wait_stoped(void)
{
	pthread_join(thread_id_, NULL);
}

bool Thread::set_cpu_affinity(size_t cpusetsize, const cpu_set_t * cpuset)
{
	return pthread_setaffinity_np(thread_id_, cpusetsize, cpuset);
}

ThreadPool::ThreadPool(bool set_cpu, uint32_t start_cpu, uint32_t max_cpu): set_cpu_(set_cpu), cur_cpu_(0), start_cpu_(start_cpu)
{
	long cpus = sysconf(_SC_NPROCESSORS_ONLN);
	if (-1 == cpus) {
		cerr << "Fail to get cpus nr" << endl;
	}

	max_cpu_ = min(static_cast<uint32_t>(cpus-1), max_cpu);

	if (start_cpu_ >= max_cpu_) {
		start_cpu_ = max_cpu_-1;
	}
}

void ThreadPool::start_all_threads(void)
{	
	for (auto it = threads_.begin(); it != threads_.end(); ++it) {
		(*it)->start();
		
		if (set_cpu_) {
			cpu_set_t cpuset;
			
			CPU_ZERO(&cpuset);
			CPU_SET(cur_cpu_, &cpuset);
			
			(*it)->set_cpu_affinity(sizeof(cpuset), &cpuset);
			
			cur_cpu_++;
			if (cur_cpu_ >= max_cpu_) {
				cur_cpu_ = start_cpu_;
			}
		}
	}
}

void ThreadPool::wait_all_threads_stoped(void)
{
	for (auto it = threads_.begin(); it != threads_.end(); ++it) {
		(*it)->wait_stoped();
	}
}



#pragma once

#include "C:\Users/Delya/Documents/Visual Studio 2015/Projects/4 MIPT/mipt_concurrency_course/Thread_safe_queue/Thread_safe_queue/thread_safe_queue.h"
#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <functional>
#include <memory>

template <class R>
class thread_pool
{
public:
	thread_pool();

	explicit thread_pool(size_t );

	thread_pool(size_t , size_t );

	std::future<R> submit(std::function<R()> );

	void shutdown();

	~thread_pool();

private:

	void initializer();

	void invoke(size_t );

	unsigned default_num_workers();

	//we keep pairs to know when we should terminate threads
	std::vector<std::pair<bool, std::thread>> workers;

	//we have to keep pointer because of packaged_task doesn't have a copy constructor
	typedef std::shared_ptr<std::packaged_task<R()>> async_task;
	thread_safe_queue<async_task> task_queue;

	std::atomic_bool shut_down;
};

#include "thread_pool.hpp"
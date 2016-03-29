#pragma once

#include "C:\Users\Egdeliya\Documents\Visual Studio 2015\Projects\4 MIPT\mipt_concurrency_course\mipt_concurrency_course\Thread_safe_queue\Thread_safe_queue\thread_safe_queue.h"
#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <functional>

//template <class R, class... A>
template <class R>
class thread_pool
{
public:

	thread_pool() : task_queue(10)
	{
		unsigned workers_number = default_num_workers();
		
		workers.reserve(workers_number);

		//this method initializes all threads
		initializer();

	}

	explicit thread_pool(size_t workers_number, size_t tasks_number)
		: task_queue(tasks_number), workers(workers_number) 
	{
		initializer();
	}

	//std::future<R(A ...)> submit(std::packaged_task<R(A ...)> foo)
	std::future<R> submit(std::packaged_task<R()>&& foo)
	{
		//task_queue.enqueue(std::make_shared<std::packaged_task<R(A ...)>>(foo));
		//std::future<R(A ...)> result = foo.get_future();

		task_queue.enqueue(std::make_shared<std::packaged_task<R()>>(foo));
		std::future<R> result = foo.get_future();
		return result;
	}

	void shutdown()
	{
		for (size_t i = 0; i < workers.size(); i++)
		{
			//to stop workers[i] 
			workers[i].first = false;
		}
		
		for (size_t i = 0; i < workers.size() ; i++)
		{
			workers[i].second.join();
		}
	}

	~thread_pool()
	{
		shutdown();
	}

protected:

	void initializer()
	{
		for (size_t i = 0; i < workers.size(); i++)
		{
			workers[i] = std::make_pair(true, std::thread(&thread_pool<R>::invoke, this, i));
			//workers[i] = std::make_pair(true, std::thread(&thread_pool<R, A>::invoke, this, i));
		}
	}

	void invoke(size_t i)
	{

		async_task current_task;

		while (workers[i].first)
		{
			task_queue.pop(current_task);

			//run our function
			//current_task.operator->(A ...);
			current_task.operator->();
		}
	}

	unsigned default_num_workers()
	{
		unsigned threads_number = std::thread::hardware_concurrency();

		if (threads_number < 0)
		{
			return 4;
		}
		else
		{
			return threads_number;
		}
	}

	//we keep pairs to know when we should terminate threads
	std::vector<std::pair<bool, std::thread>> workers;
	
	//we have to keep pointer because of packeged_task doesn't have a copy constructor
	//typedef std::shared_ptr<std::packaged_task<R(A ...)>> async_task;
	typedef std::shared_ptr<std::packaged_task<R()>> async_task;
	thread_safe_queue<async_task> task_queue;

};
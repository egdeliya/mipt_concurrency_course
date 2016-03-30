#pragma once

#include "C:\Users\Egdeliya\Documents\Visual Studio 2015\Projects\4 MIPT\mipt_concurrency_course\mipt_concurrency_course\Thread_safe_queue\Thread_safe_queue\thread_safe_queue.h"
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
	thread_pool() : task_queue(10)
	{
		unsigned workers_number = default_num_workers();

		workers.resize(workers_number);

		//this method initializes all threads
		initializer();
	}

	thread_pool(size_t workers_number) : thread_pool(workers_number, 10) {};

	thread_pool(size_t workers_number, size_t tasks_number)
		: task_queue(tasks_number), workers(workers_number)
	{
		initializer();
	}

	std::future<R> submit(std::function<R()> foo)
	{
		try
		{
			async_task pointer_to_packaged_task = std::make_shared<std::packaged_task<R()>>(std::packaged_task<R()>(foo));
			task_queue.enqueue(pointer_to_packaged_task);
			return pointer_to_packaged_task->get_future();
		}
		catch (std::exception& e)
		{
			std::cout << "from submit[" << std::this_thread::get_id() << "]: " << e.what() << std::endl;
		}
	}

	void shutdown()
	{
		for (size_t i = 0; i < workers.size(); i++)
		{
			workers[i].first = false;

		}

		for (size_t i = 0; i < workers.size(); i++)
		{
			//we have to do that to thread check workers[i].first
			task_queue.enqueue(std::make_shared<std::packaged_task<R()>>([]() {return 0; }));
		}

		for (size_t i = 0; i < workers.size(); i++)
		{
			workers[i].second.join();
		}
	}

	~thread_pool()
	{
		shutdown();
	}

private:

	void initializer()
	{
		for (size_t i = 0; i < workers.size(); i++)
		{
			workers[i].first = true;
			workers[i].second = std::thread(&thread_pool<R>::invoke, this, i);
		}
	}

	void invoke(size_t thread_index)
	{
		try
		{
			while (workers[thread_index].first)
			{
				async_task current_task = task_queue.pop();
				current_task->operator()();
			}
		}
		catch (std::exception& e)
		{
			std::cout << "from invoke " << e.what() << std::endl;
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
	typedef std::shared_ptr<std::packaged_task<R()>> async_task;
	thread_safe_queue<async_task> task_queue;

};
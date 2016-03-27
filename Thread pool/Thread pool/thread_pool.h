#pragma once

#include "C:\Users\Egdeliya\Documents\Visual Studio 2015\Projects\4 MIPT\mipt_concurrency_course\mipt_concurrency_course\Thread_safe_queue\Thread_safe_queue\thread_safe_queue.h"
#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <functional>

template <class R>
class invoke;

template <class R>
class thread_pool
{
public:

	thread_pool() : task_queue(10)
	{
		unsigned workers_number = default_num_workers();
		
		workers.resize(workers_number);

		//инициализируем все потоки
		initializer();

	}

	explicit thread_pool(size_t workers_number, size_t tasks_number)
		: task_queue(tasks_number), workers(workers_number) 
	{
		initializer();
	}

	std::future<R> submit(std::packaged_task<R()> foo)
	{
		task_queue.enqueue(foo);
		std::future<R()> result = foo.get_future();
		return result;
	}

	void shutdown()
	{
		for (size_t i = 0; i < workers.size(); i++)
		{
			workers[i].first = 0;
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

	//static void invoke(size_t thread_id)
	//{
	//	std::packaged_task<R()> current_task;

	//	while (workers[thread_id].first)
	//	{
	//		task_queue.pop(current_task);
	//		current_task();
	//	}
	//}

	void initializer()
	{
		for (size_t i = 0; i < workers.size(); i++)
		{
			//если workers[i].first == true , то продолжаем работу, иначе завершаемся
			workers[i] = std::make_pair(true , std::thread([this, i]() {
				std::packaged_task<R()> current_task;

				while (workers[i].first)
				{
					task_queue.pop(current_task);
					current_task();
				}
			}));
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

	//кладём в вектор пары, чтобы знать, когда завершать thread
	std::vector<std::pair< bool, std::thread> > workers;
	thread_safe_queue<std::packaged_task<R()>> task_queue;

};
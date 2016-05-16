#pragma once

#include  "C:\Users\Delya\Documents\Visual Studio 2015\Projects\4 MIPT\mipt_concurrency_course\Thread_safe_queue\Thread_safe_queue\Thread safe queue\Thread safe queue\thread_safe_queue.h"
#include <thread>
#include <future>
#include <functional>
#include <atomic>
#include <stdexcept>

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

	void invoke();

	static unsigned default_num_workers();

	std::vector<std::thread> workers;

	thread_safe_queue<std::packaged_task<R()> > task_queue;

	volatile std::atomic_bool shut_down;
};

template <class R>
thread_pool<R>::thread_pool() : task_queue(10), shut_down(false)
{
	auto workers_number = default_num_workers();

	workers.resize(workers_number);

	//this method initializes all threads
	initializer();
}

template <class R>
thread_pool<R>::thread_pool(size_t workers_number) : thread_pool(workers_number, 10) {};

template <class R>
thread_pool<R>::thread_pool(size_t workers_number, size_t tasks_number) :
	workers(workers_number), task_queue(tasks_number), shut_down(false)
{
	initializer();
}

template <class R>
std::future<R> thread_pool<R>::submit(std::function<R()> foo)
{
	if (shut_down)
	{
		throw std::logic_error("Exception from thread_pool::submit because method shut down was invoked");
	}

	auto cur_task = std::packaged_task<R()>(foo);
	std::future<R> result = cur_task.get_future();

	try
	{
		task_queue.enqueue(std::move(cur_task));
	}
	catch (std::logic_error e)
	{
		std::cout << e.what() << std::endl;
	}

	return result;
}

template <class R>
void thread_pool<R>::shutdown()
{
	task_queue.shutdown();
	shut_down = true;

	for (size_t i = 0; i < workers.size(); i++)
	{
		workers[i].join();
	}
}

template <class R>
thread_pool<R>::~thread_pool()
{
	if (!shut_down) shutdown();
}

template <class R>
void thread_pool<R>::initializer()
{
	for (size_t i = 0; i < workers.size(); i++)
	{
		workers[i] = std::thread(&thread_pool<R>::invoke, this);
	}
}

template <class R>
void thread_pool<R>::invoke()
{
	while (true)
	{
		std::packaged_task<R()> cur_task;

		// если вызван метод shut_down на пустой очереди, то она кинет исключение,
		// и мы узнаем, что нужно завершать работу
		try
		{
			task_queue.pop(cur_task);
		}
		catch (std::logic_error e)
		{
			break;
		}

		cur_task();
	}
}

template <class R>
unsigned thread_pool<R>::default_num_workers()
{
	auto threads_number = std::thread::hardware_concurrency();

	if (threads_number <= 0)
	{
		return 4;
	}
	else
	{
		return threads_number;
	}
}
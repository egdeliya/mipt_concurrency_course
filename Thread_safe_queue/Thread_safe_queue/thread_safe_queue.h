#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class thread_safe_queue
{
public:
	explicit thread_safe_queue(size_t capacity) : over_size(capacity) {}

	thread_safe_queue(thread_safe_queue<T> &) = delete;

	thread_safe_queue& operator=(thread_safe_queue<T> &) = delete;

	void enqueue(const T& item)
	{
		//std::cout << "hello from enqueue!! \n";
		std::unique_lock<std::mutex> locker(mut/*, std::defer_lock_t()*/);

		overflow.wait(locker, [this]() {return intern.size() < over_size; });
		//overflow.notify_all();
		//while (intern.size() == over_size) overflow.wait(locker);

		intern.push(item);

		locker.unlock();

		empty.notify_all();

		//locker.unlock();
	}

	void pop(T& item)
	{
		//std::cout << "hello from pop!! \n";
		std::unique_lock<std::mutex> locker(mut);

		empty.wait(locker, [this]() {return intern.size() > 0; });
		//empty.notify_all();

		//std::cout << "I can get my element!!\n";

		item = intern.front();
		intern.pop();

		locker.unlock();

		overflow.notify_all();

		//locker.unlock();
	}

	~thread_safe_queue()
	{
		empty.notify_all();
		overflow.notify_all();
	}

private:
	std::queue <T> intern;
	std::mutex mut;

	std::condition_variable empty;
	std::condition_variable overflow;

	const size_t over_size;
};

//#include "thread_safe_queue.hpp"
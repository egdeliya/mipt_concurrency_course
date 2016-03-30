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
	
	void enqueue(const T&);

	T pop();

	void pop(T&);

	//T&& move_pop();

private:
	std::queue <T> intern;
	std::mutex mut;

	std::condition_variable empty;
	std::condition_variable overflow;

	const size_t over_size;
};

#include "thread_safe_queue.hpp"
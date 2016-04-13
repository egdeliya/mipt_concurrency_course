#pragma once

#include <mutex>
#include <condition_variable>

class barrier
{
public:
	explicit barrier(size_t);
	void enter();
private:
	std::mutex mtx;
	std::condition_variable cv;
	size_t counter;
	size_t threads_number;
	size_t entrance_;
	size_t exit_;
};
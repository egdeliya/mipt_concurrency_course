#pragma once

#include <condition_variable>
#include <mutex>

class semaphore
{
public:
	semaphore();

	explicit semaphore(size_t);

	void signal();

	void wait();
private:
	std::condition_variable cv;
	std::mutex mtx;
	int counter;
};
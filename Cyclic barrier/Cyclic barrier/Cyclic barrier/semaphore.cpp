#include "semaphore.h"

semaphore::semaphore(): counter(0) {}

semaphore::semaphore(size_t new_counter) : counter(new_counter) {}

void semaphore::signal()
{
	std::lock_guard<std::mutex> locker(mtx);

	counter++;

	cv.notify_all();
}

void semaphore::wait()
{
	std::unique_lock<std::mutex> locker(mtx);

	cv.wait(locker, [this]() {return counter > 0; });

	counter--;
}


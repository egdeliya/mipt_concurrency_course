#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>

template <typename T, class Container = std::deque<T> >
class thread_safe_queue
{
public:
	explicit thread_safe_queue(size_t capacity) : over_size(capacity), shut_down(false) {}

	thread_safe_queue(thread_safe_queue<T> &) = delete;

	thread_safe_queue& operator=(thread_safe_queue<T> &) = delete;

	void enqueue(const T& item)
	{
		//std::cout << "I'm in the enqueue!\n";
		if (shut_down)
		{
			throw std::logic_error("Method shut down was invoked");
		}

		std::unique_lock<std::mutex> locker(mut);

		overflow.wait(locker, [this]() {return (intern.size() < over_size) && (!shut_down); });

		intern.push_back(T(item));

		empty.notify_all();

		//locker.unlock();
	}

	void pop(T& item)
	{
		std::unique_lock<std::mutex> locker(mut);

		empty.wait(locker, [this]() {return (intern.size() > 0) && (!shut_down); });

		if (shut_down)
		{
			throw std::logic_error("The queue is empty and method shut down was invoked");
		}

		item = std::move(intern.front());
		intern.pop_front();

		overflow.notify_all();

		//locker.unlock();

	}

	void shutdown()
	{
		shut_down = true;
	}

private:
	Container intern;
	std::mutex mut;

	std::condition_variable empty;
	std::condition_variable overflow;

	const size_t over_size;
	bool shut_down;
};


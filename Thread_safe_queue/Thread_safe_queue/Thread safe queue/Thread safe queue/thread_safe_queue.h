#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <atomic>

template <class T, class Container=std::deque<T> >
class thread_safe_queue
{
public:
	explicit thread_safe_queue(size_t capacity) : over_size(capacity), shut_down(false) {}

	thread_safe_queue(thread_safe_queue<T> &) = delete;

	thread_safe_queue& operator=(thread_safe_queue<T> &) = delete;

	void enqueue(const T& item)
	{
		if (shut_down)
		{
			throw std::logic_error("Method shut down was invoked");
		}

		std::unique_lock<std::mutex> locker(mut);

		// ждем пока кто-нибудь что-нибудь полжит в контейнер или пока будет вызван метод shutdown
		overflow.wait(locker, [this]() {return (intern.size() < over_size) || (shut_down); });

		if (shut_down)
		{
			throw std::logic_error("Method shut down was invoked");
		}

		intern.push_back(std::move(item));

		empty.notify_all();

	}

	// этот метод нужен для объектов без конструктора копирования 
	void enqueue(T&& item)
	{
		if (shut_down)
		{
			throw std::logic_error("Method shut down was invoked");
		}

		std::unique_lock<std::mutex> locker(mut);

		overflow.wait(locker, [this]() {return (intern.size() < over_size) || (shut_down); });

		if (shut_down)
		{
			throw std::logic_error("Method shut down was invoked");
		}

		intern.push_back(std::move(item));

		empty.notify_all();

	}

	void pop(T& item)
	{
		std::unique_lock<std::mutex> locker(mut);

		// ждём пока кто-нибудь что-нибудь  положит в очередь или пока не будет вызван метод shutdown
		empty.wait(locker, [this]() {return (intern.size() > 0) || (shut_down); });

		// если кто-то вызвал shutdown и очередь пуста, то кидаем исключение, иначе извлекаем элемент из очереди
		if (shut_down && (intern.size() == 0))
		{
			throw std::logic_error("The queue is empty and method shut down was invoked");
		}

		item = std::move(intern.front());
		intern.pop_front();

		overflow.notify_all();

	}

	void shutdown()
	{
		// просто ставим флажок того что вызван метод shut_down
		shut_down = true;
	}

private:
	Container intern;
	std::mutex mut;

	std::condition_variable empty;
	std::condition_variable overflow;

	const size_t over_size;
	std::atomic_bool shut_down;
};


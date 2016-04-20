#pragma once

template <typename T>
void thread_safe_queue<T>::enqueue(const T& item)
{
	//std::cout << "I'm in the enqueue!\n";

	std::unique_lock<std::mutex> locker(mut);

	overflow.wait(locker, [this]() {return intern.size() < over_size; });

	intern.push(item);
	
	empty.notify_all();

	//locker.unlock();
}

template <typename T>
void thread_safe_queue<T>::pop(T& item)
{
	std::unique_lock<std::mutex> locker(mut);

	empty.wait(locker, [this]() {return intern.size() > 0; });

	item = intern.front();
	intern.pop();
	
	overflow.notify_all();

	//locker.unlock();

}

template <typename T>
T thread_safe_queue<T>::pop()
{
	//std::cout << "I'm in the pop!\n";

	std::unique_lock<std::mutex> locker(mut);

	empty.wait(locker, [this]() {return intern.size() > 0; });

	T element = intern.front();
	intern.pop();
	
	overflow.notify_all();

	return element;
}

template <typename T>
void thread_safe_queue<T>::shutdown()
{

}


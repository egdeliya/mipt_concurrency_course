#pragma once

template <typename T>
void thread_safe_queue<T>::enqueue(const T& item)
{
	std::cout << "hello from enqueue!! \n";
	std::unique_lock<std::mutex> locker(mut);

	overflow.wait(locker, [this]() {return intern.size() < over_size; });
	overflow.notify_all();
	//while (intern.size() == over_size) overflow.wait(locker);

	intern.push(item);

	locker.unlock();

}

template <typename T>
void thread_safe_queue<T>::pop(T& item)
{
	std::cout << "hello from pop!! \n";
	std::unique_lock<std::mutex> locker(mut);

	empty.wait(locker, [this]() {return intern.size() > 0; });
	empty.notify_all();

	std::cout << "I can get my element!!\n";

	item = intern.front();
	intern.pop();
	locker.unlock();
}

template <typename T>
thread_safe_queue<T>::~thread_safe_queue()
{
	empty.notify_all();
	overflow.notify_all();
}
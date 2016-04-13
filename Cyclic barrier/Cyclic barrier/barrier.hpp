#include "barrier.h"
#include <iostream>
#include <stdexcept>

barrier::barrier(size_t num_threads) : threads_number(num_threads), entrance_(0), exit_(0), counter(0) 
{
	if (num_threads == 0)
	{
		throw std::invalid_argument("You must use cyclic barrier with a non-zero number of arguments");
	}
};

void barrier::enter()
{
	std::unique_lock<std::mutex> locker(mtx);

	// если вход не равен выходу, значит, что кто-то уже зашел но ещё пока не успел выйти
	cv.wait(locker, [this]() {return entrance_ == exit_; });

	++counter;

	// если поток заходит последним, то он дожлен подать сигнал всем остальным, чтобы они проснулись 
	// и начали проходить через барьер, и что потоки начали проходить через барьер но пока не все прошли, 
	// чтобы другие потоки не входили, пока старые не вышли
	if (counter == threads_number)
	{
		entrance_ = exit_ + 1;
		cv.notify_all();
	}
	else
	{
		cv.wait(locker);
	}

	--counter;

	//если поток выходит последним, то он должен открыть проход весм остальным потокам, которые хотят пройти через барьер
	if (counter == 0)
	{
		++exit_;
		cv.notify_all();
	}

}
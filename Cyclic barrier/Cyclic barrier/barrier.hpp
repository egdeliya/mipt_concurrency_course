#include "barrier.h"
#include <iostream>

barrier::barrier(size_t num_threads) : threads_number(num_threads), entrance_(0), exit_(0), counter(0) 
{
	/*threads_number = num_threads;
	entrance__ = 0;
	exit_ = 0;
	counter = 0;*/
};

void barrier::enter()
{
	std::unique_lock<std::mutex> locker(mtx);

	// если вход не равен выходу, значит, что кто-то уже зашел но ещё пока не успел выйти
	cv.wait(locker, [this]() {return entrance_ == exit_; });

	++counter;
	//std::cout << "I enter!!! And now counter is " << counter << std::endl;

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
	//std::cout << "I exit!!! And now counter is " << counter << std::endl;

	//если поток выходит последним, то он должен открыть проход весм остальным потокам, которые хотят пройти через барьер
	if (counter == 0)
	{
		++exit_;
		cv.notify_all();
	}

	//cv.notify_all();

}
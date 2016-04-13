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

	// ���� ���� �� ����� ������, ������, ��� ���-�� ��� ����� �� ��� ���� �� ����� �����
	cv.wait(locker, [this]() {return entrance_ == exit_; });

	++counter;
	//std::cout << "I enter!!! And now counter is " << counter << std::endl;

	// ���� ����� ������� ���������, �� �� ������ ������ ������ ���� ���������, ����� ��� ���������� 
	// � ������ ��������� ����� ������, � ��� ������ ������ ��������� ����� ������ �� ���� �� ��� ������, 
	// ����� ������ ������ �� �������, ���� ������ �� �����
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

	//���� ����� ������� ���������, �� �� ������ ������� ������ ���� ��������� �������, ������� ����� ������ ����� ������
	if (counter == 0)
	{
		++exit_;
		cv.notify_all();
	}

	//cv.notify_all();

}
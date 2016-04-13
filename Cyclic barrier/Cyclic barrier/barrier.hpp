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

	// ���� ���� �� ����� ������, ������, ��� ���-�� ��� ����� �� ��� ���� �� ����� �����
	cv.wait(locker, [this]() {return entrance_ == exit_; });

	++counter;

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

	//���� ����� ������� ���������, �� �� ������ ������� ������ ���� ��������� �������, ������� ����� ������ ����� ������
	if (counter == 0)
	{
		++exit_;
		cv.notify_all();
	}

}
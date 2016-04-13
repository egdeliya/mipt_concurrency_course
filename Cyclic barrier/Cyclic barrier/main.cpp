#include <iostream>
#include "barrier.h"
#include <vector>
#include <chrono>
#include <mutex>
#include <condition_variable>

static const size_t threads_number = 10;
static int counter = 2;

using namespace std;

void main()
{
	barrier bar(threads_number);
	std::vector<std::thread> th(threads_number);
	
	auto func = [&bar](size_t id, size_t counter) {
		while (counter > 0)
		{
			bar.enter();
			std::cout << "My id is " << id << std::endl;
			--counter;
		}
	};

	for (size_t i = 0; i < threads_number; i++)
	{
		//�����, ����� ������ ���� ��� ������ ����� ������
		th[i] = std::thread(func, i, 5);
	}

	int buf_counter = counter;
	for (size_t i = 0; i < buf_counter; i++)
	{
		std::this_thread::sleep_for(100ms);
		//--counter;
	}

	for (size_t i = 0; i < threads_number; i++)
	{
		th[i].join();
	}
	
	system("pause");
	return;
}

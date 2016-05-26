#include "spsc_ring_buffer.h"
#include <iostream>
#include <future>

static size_t len = 1024;
static size_t elements_number = 2;

uint64_t producer_work_loop(spsc_ring_buffer<int>& buf)
{
	uint64_t sum = 0;
	for (size_t i = 0; i < elements_number; i++)
	{
		while (!buf.enqueue(i))
		{
			std::this_thread::yield();
		}
		sum += i;
	}
	return sum;
}

uint64_t consumer_work_loop(spsc_ring_buffer<int>& buf)
{
	uint64_t sum = 0;
	int elem;
	for (size_t i = 0; i < elements_number; i++)
	{
		while (!buf.dequeue(elem))
		{
			std::this_thread::yield();
		}
		sum += elem;
	}
	return sum;
}

void main()
{
	spsc_ring_buffer<int> buf(len);

	std::future<uint64_t> produced_sum = std::async(producer_work_loop, std::ref(buf));

	std::future<uint64_t> consumed_sum = std::async(consumer_work_loop, std::ref(buf));

	uint64_t sum1 = produced_sum.get();
	std::cout << sum1 << std::endl;

	uint64_t sum2 = consumed_sum.get();
	std::cout << sum2 << std::endl;

	system("pause");

}
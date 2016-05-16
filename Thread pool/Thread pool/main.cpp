#include "thread_pool.h"
#include <iostream>
#include <vector>
#include <thread>

const size_t array_size = 1000;

using namespace std;

int func_sum(int start, int step, std::vector<int>& vec)
{
	auto sum = 0;
	if (start < vec.size())
	{
		for (size_t i = start; i < vec.size(); i += step)
		{
			sum += vec[i];
		}
	}
	return sum;
}

void test_func_for_pool()
{
	try
	{
		thread_pool<int> pool;

		std::vector<int> our_array(array_size);
		srand(static_cast<unsigned>(time(nullptr)));
		for (size_t i = 0; i < array_size; i++)
		{
			our_array[i] = rand() % 100;
			srand(i);
		}
		std::cout << std::endl;

		// считаем сумму элементов массива в несколько потоков
		auto sum = 0;
		std::vector<std::future<int>> futures;
		for (size_t i = 0; i < 10; i++)
		{
			futures.emplace_back(pool.submit(std::bind(func_sum, i, 100, our_array)));
		}

		for (size_t i = 0; i < futures.size(); i++)
		{
			sum += futures[i].get();
		}
		std::cout << "The sum is: " << sum << std::endl;

		pool.shutdown();
	}
	catch (std::exception& e)
	{
		std::cout << "from test function " << e.what() << std::endl;
	}
}

long test(long limit)
{
	long long a = 0;
	for (auto i = 0; i < limit; ++i) {
		a++;
		if (i % 10000000000 == 0)
			std::cout << "I'm counting. ID ==" << this_thread::get_id() << std::endl;
	}
	return a;
}

int main()
{
	try
	{
		test_func_for_pool();
	} catch (std::exception& e) {
		cout << "from test fun for pool in main " << e.what() << std::endl;
	}


	//Another test
	thread_pool<long> gogoLovers(10);

	vector<future<long>> futures;

	for (long double i = 1; i < 850345; i *= 1.2) {
		futures.emplace_back(gogoLovers.submit(std::bind(test, i)));
	}

	for (auto& it : futures)
		std::cout << "Got result : " << it.get() << std::endl;
	
	gogoLovers.shutdown();
	
	system("pause");
	return 0;
}
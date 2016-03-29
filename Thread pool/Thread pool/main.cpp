#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>
#include "thread_pool.h"

const int magic_number = 9;

int sum(int step, std::vector<int>& vec)
{
	int sum = 0;
	if (step < vec.size() - 1)
	{
		for (size_t i = step; i < vec.size(); i += step)
		{
			sum += vec[i];
		}
	}
	return sum;
}

void test_func_for_pool()
{
	//thread_pool<int, int, std::vector<int>&> pool;
	thread_pool<int> pool;

	std::vector<int> our_array(magic_number);

	srand((unsigned)time(0));

	for (size_t i = 0; i < magic_number; i++)
	{
		our_array[i] = -100 % rand() + 200;
	}

	std::cout << "Our array is: \n";
	for each (int var in our_array)
	{
		std::cout << var << "  ";
	}
	std::cout << std::endl;

	std::cout << "And now we are going to run functions to calculate the sum of the elements of this array, \n";
	
	for (size_t i = 0; i < 4; i++)
	{
		//pool.submit(std::bind(std::packaged_task<int(int, std::vector<int>&)>, i, our_array));
		//pool.submit(std::packaged_task<int(int, std::vector<int>&)>(std::bind(sum, i, our_array)));
		std::future<int> fut(pool.submit(std::packaged_task<int()>(std::bind(sum, i, our_array))));

	}
}

int main()
{
	//thread_pool<int> pool;
	test_func_for_pool();

	system("pause");
	return 0;
}
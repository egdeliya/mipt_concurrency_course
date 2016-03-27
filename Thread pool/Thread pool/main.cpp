#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include "thread_pool.h"

void foo()
{
	std::cout << "Hello\n";
}

int main()
{
	thread_pool<int> pool;

	//std::vector <std::thread> threads(4);

	system("pause");
	return 0;
}
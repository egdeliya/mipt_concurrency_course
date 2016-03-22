#include <iostream>
#include <math.h>
#include <vector>
#include <thread>
#include "thread_safe_queue.h"

const int magic_number = 9;
const int threads_number = 4;
const int stop_flag = -1;

thread_safe_queue<int> my_queue(magic_number);

//consumer's function
void isPrime()
{
	int number = 2;
	int num_sqr;

	while (true)
	{
		try {
			my_queue.pop(number);
		}
		catch (std::exception & e) {
			std::cout << e.what() << std::endl;
		}


		if (number == 0 || number == 1)
		{
			std::cout << number << " isn't a prime number\n";
			continue;
		}

		//to stop this thread
		if (number == stop_flag) return;

		num_sqr = (int)std::sqrt(number) + 1;

		int check = 2;

		for (; check <= num_sqr; ++check)
		{
			if (number % check == 0)
			{
				std::cout << number << " isn't a prime number\n";
				break;
			}
		}

		if (check > num_sqr) std::cout << number << " is a prime number\n";
	}

}

void produsers_func()
{
	srand(magic_number);
	int rand_number;

	for (size_t i = 0; i < 100; i++)
	{
		rand_number = 0 + rand() % 100;

		try
		{
			my_queue.enqueue(rand_number);
		}
		catch (std::exception & e) {
			std::cout << e.what() << std::endl;
		}
	}

	//to stop all threads
	for (size_t i = 0; i < threads_number; i++)
	{
		try
		{
			my_queue.enqueue(stop_flag);
		}
		catch (std::exception & e) {
			std::cout << e.what() << std::endl;
		}
	}
}

void test_function_for_thread_safe_queue()
{
	std::thread produser(produsers_func);
	std::vector <std::thread>  threads(threads_number);

	for (size_t i = 0; i < threads_number; i++)
	{
		threads[i] = std::thread(isPrime);
	}

	produser.join();

	for (size_t i = 0; i < threads_number; i++)
	{
		threads[i].join();
	}

}

int main()
{
	test_function_for_thread_safe_queue();

	system("pause");
	return 0;
}
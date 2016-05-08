#include <iostream>
#include <math.h>
#include <vector>
#include <thread>
#include "Thread safe queue/Thread safe queue/thread_safe_queue.h"

const int magic_number = 9;
const int threads_number = 1;
const int stop_flag = -1;

thread_safe_queue<int> my_queue(1);

//consumer's function
void isPrime()
{
	int number = 2;
	int num_sqr;
	int counter = 0;

	try
	{
		while (true)
		{
			my_queue.pop(number);
			++counter;
			std::cout << counter << std::endl;

			if (number == 0 || number == 1)
			{
				std::cout << number << " isn't a prime number\n";
				continue;
			}

			//to stop this thread
			if (number == stop_flag) return;

			num_sqr = std::round(::sqrt(number)) + 1;

			bool is_prime = true;

			for (int check = 2; check <= num_sqr; ++check)
			{
				if (number % check == 0)
				{
					is_prime = false;
					break;
				}
			}

			(is_prime) ? (std::cout << number << " is a prime number\n") : (std::cout << number << " isn't a prime number\n");
		}
	}
	catch (std::exception & e) {
		std::cout << e.what() << std::endl;
	}
	
	std::cout << "consumer has finished his work\n";
}

void produsers_func()
{
	srand(magic_number);
	int rand_number;

	for (size_t i = 0; i < 100; i++)
	{
		rand_number = 0 + rand() % 1000;

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

	my_queue.shutdown();
	std::cout << "producer has finished his work\n";
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
	try
	{
		test_function_for_thread_safe_queue();
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	int&& a = int(4);
	std::cout << a << std::endl;

	system("pause");
	return 0;
}
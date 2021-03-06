#include "Cyclic barrier/step_cv.h"
#include "barrier.h"
#include "Cyclic barrier/step_sem.h"

#include <iostream>
#include <vector>

static const size_t threads_number = 10;

using namespace std;

void barrier_test()
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
		//хотим, чтобы потоки пять раз прошли через барьер
		th[i] = std::thread(func, i, 5);
	}

	for (size_t i = 0; i < threads_number; i++)
	{
		th[i].join();
	}
}

void main()
{
	/*std::cout << "barrier test" << std::endl;
	barrier_test();*/

	/*std::cout << std::endl << "step with conditional variable test" << std::endl;
	step_cond_test();*/

	std::cout << std::endl << "step with semaphore test" << std::endl;
	step_sem_test();
	
	system("pause");
}

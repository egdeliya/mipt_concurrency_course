#include "step_sem.h"

static const int steps_number = 100;

semaphore left(0);
semaphore right(0);

// робот всегда делает шаг левой ногой
void step_sem(std::string direction)
{
	if (direction == "left")
	{
		//ждет, когда ему просигналят, что можно делать шаг
		left.wait();

		// делает шаг
		std::cout << "left ";

		// сигнали правому, что можно сделать шаг
		right.signal();
	}
	else
	{
		// сигналит левому, что можно сделать шаг
		left.signal();

		// ждет, когда левый сделает шаг и просигналит
		right.wait();

		// делает свой шаг
		std::cout << "right" << std::endl;
	}
}

void step_sem_test()
{
	

	std::thread th2([]()
	{
		auto counter = 0;
		while (counter < steps_number)
		{
			std::cout << counter << " ";
			step_sem("right");
			++counter;

		}
	});

	std::thread th1([]()
	{
		auto counter = 0;
		while (counter < steps_number)
		{
			// std::cout << counter << " ";
			step_sem("left");
			++counter;
		}
	});

	th1.join();
	th2.join();
}
#include "step_cv.h"

static const int steps_number = 100;

std::condition_variable left;
std::condition_variable right;
std::mutex mut;

static std::string last_step;

// первый шаг робот всегда делает левой ногой (судьба у него такая)
void step_cv(std::string direction)
{
	std::unique_lock<std::mutex> locker(mut);

	if (direction == "left")
	{
		// если роботу сказали идти влево, то он говорит, что последний шаг был сделан левой ногой
		last_step = "left";

		std::cout << last_step << " ";

		// буди правую ногу и ждет, пока шаг будет сделан правой ногой
		right.notify_one();

		left.wait(locker, []() { return last_step == "right"; });
		
	}
	else
	{
		// если правой ногой, то он ждет, когда шаг будет сделан левой ногой
		right.wait(locker, []() { return last_step == "left"; });

		// говорит, что последний шаг сделан правой ногой
		last_step = "right";

		std::cout << last_step << std::endl;

		// будит левую ногу
		left.notify_one();
	}
}

void step_cond_test()
{
	std::thread th1([]()
	{
		auto counter = 0;
		while (counter < steps_number)
		{
			std::cout << counter << " ";
			step_cv("left");
			++counter;
		}
	});

	std::thread th2([]()
	{
		auto counter = 0;
		while (counter < steps_number)
		{
			step_cv("right");
			++counter;

		}
	});

	th1.join();
	th2.join();
}

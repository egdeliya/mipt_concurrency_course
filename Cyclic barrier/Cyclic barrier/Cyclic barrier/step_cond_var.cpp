#include "step_cv.h"

int steps_number = 10;

std::condition_variable left;
std::condition_variable right;
std::mutex mut;
std::string last_step;

void step_cv(std::string direction)
{
	std::unique_lock<std::mutex> locker(mut);

	if (direction == "left")
	{
		last_step = "left";

		std::cout << last_step << " ";

		right.notify_one();

		left.wait(locker, []() { return last_step == "right"; });
		
		//right.notify_one();
			
	}
	else
	{

		right.wait(locker, []() { return last_step == "left"; });

		last_step = "right";

		std::cout << last_step << std::endl;

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
			step_cv("left");
			++counter;
			//std::cout << "I'm left and counter is " << counter << std::endl;
		}
	});

	std::thread th2([]()
	{
		auto counter = 0;
		while (counter < steps_number)
		{
			step_cv("right");
			++counter;
			//std::cout << "I'm right and counter is " << counter << std::endl;

		}
	});
	th1.join();
	th2.join();
}

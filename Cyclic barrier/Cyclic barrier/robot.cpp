#include <mutex>
#include <iostream>
#include <string>

static const int steps_number = 100;

/****************** семафор **********************/

class semaphore
{
public:
	semaphore();

	explicit semaphore(size_t);

	void signal();

	void wait();
private:
	std::condition_variable cv;
	std::mutex mtx;
	int counter;
};

semaphore::semaphore() : counter(0) {}

semaphore::semaphore(size_t new_counter) : counter(new_counter) {}

void semaphore::signal()
{
	std::lock_guard<std::mutex> locker(mtx);

	counter++;

	cv.notify_all();
}

void semaphore::wait()
{
	std::unique_lock<std::mutex> locker(mtx);

	cv.wait(locker, [this]() {return counter > 0; });

	counter--;
}

/****************** шагающий робот с семафором **********************/

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

/****************** шагающий робот с условными переменными **********************/

std::condition_variable left1;
std::condition_variable right1;
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
		right1.notify_one();

		left1.wait(locker, []() { return last_step == "right"; });

	}
	else
	{
		// если правой ногой, то он ждет, когда шаг будет сделан левой ногой
		right1.wait(locker, []() { return last_step == "left"; });

		// говорит, что последний шаг сделан правой ногой
		last_step = "right";

		std::cout << last_step << std::endl;

		// будит левую ногу
		left1.notify_one();
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


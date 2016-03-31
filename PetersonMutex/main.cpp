#include <iostream>
#include <thread>
#include <vector>
#include "Peterson_Mutex.h"
#include "Spinlock.h"
#include "tree_mutex.h"

int param = 0;
PetersonMutex mutex;
Spinlock spinlock;
int threads_number = 4;

tree_mutex mtx(threads_number);

int tree_mut_parameter = 0;

void func(int my_id, int others_id)
{
	mutex.lock(my_id, others_id);
    for (int j = 0; j < 10000000; ++j) {
        ++param;
    }
	mutex.unlock(my_id);
}

void test_func_for_PetersonMutex()
{
    std::cout << "Test for Peterson Mutex\n" << std::endl;
	std::vector <std::thread> threads(2);
	
	threads[0] = std::thread(func, 0, 1);
	threads[1] = std::thread(func, 1, 0);

	for (size_t i = 0; i < threads.size(); i++)
	{
		threads[i].join();
	}

	std::cout << "parameter is " << param << std::endl << std::endl;
	std::cout << "The end of the test of the Peterson Mutex" << std::endl << std::endl;
}

void spinlock_func(int id) {
	spinlock.lock();
	std::cout << "current thread id is " << id << std::endl;
	spinlock.unlock();
}

void test_func_for_Spinlock() {

	std::cout << "Test for spinlock" << std::endl << std::endl;
	std::vector <std::thread> threads(10);

	for (size_t i = 0; i < threads.size(); i++)
	{
		threads[i] = std::thread(spinlock_func, i);
	}

	for (size_t i = 0; i < threads.size(); i++)
	{
		threads[i].join();
	}

	std::cout << "\nThe end of the spinlock test!" << std::endl << std::endl; 
}

void tree_mut_func(int my_id)
{
	mtx.lock(my_id);
	std::cout << "Thread id is " << my_id << std::endl;
	mtx.unlock(my_id);
}

void test_func_for_tree_mutex()
{
	std::cout << "Test for mutex tree!!!" << std::endl << std::endl;
	
	std::vector <std::thread> threads(threads_number);

	for (size_t i = 0; i < threads_number; i++)
	{
		threads[i] = std::thread(tree_mut_func, i);
	}

	for (size_t i = 0; i < threads_number; i++)
	{
		threads[i].join();
	}

	std::cout << "\nThe end of the mutex tree test!" << std::endl << std::endl;
}

int main() {

	test_func_for_PetersonMutex();
	test_func_for_Spinlock();
	test_func_for_tree_mutex();

	system("pause");
    return 0;
}
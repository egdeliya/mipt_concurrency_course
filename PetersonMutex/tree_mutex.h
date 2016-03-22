#ifndef TREE_MUTEX_H
#define TREE_MUTEX_H

#include <vector>
#include <atomic>
#include <thread>
#include <array>

class PetMutex {
public:
	
	PetMutex()
	{
		victim.store(0);
		want[0].store(false);
		want[1].store(false);
	}

	PetMutex(const PetMutex&) {}

	void lock(int my_id) 
	{
		want[my_id].store(true);
		victim.store(my_id);

		while (want[1 - my_id].load() && victim.load() == my_id) {
			std::this_thread::yield();
		}
	}

	void unlock(int my_id) 
	{
		want[my_id].store(false);
	}

private:

	std::array <std::atomic <bool>, 2> want;
	std::atomic<int> victim;
};

class tree_mutex {
public:
	
	tree_mutex(size_t num_threads): the_closest_power(1)
	{
		twoDegree(num_threads);
		
		//calculating the closest right power of two
		twoDegree(the_closest_power);

		pet_mutex.resize(the_closest_power);

	}
	
	//up
	void lock(size_t thread_index) 
	{
		int leaf_index = thread_index + the_closest_power - 1;
		while (leaf_index > 0)
		{
			pet_mutex[(leaf_index - 1) / 2].lock((leaf_index + 1) % 2);
			leaf_index = (leaf_index - 1) / 2;
		}
	}
    
	//down
	void unlock(size_t thread_index) 
	{
		//for finding path in tree
		int path = the_closest_power;
		int current_choise;
		int parents_index = 0;
	
		while ( path != 0 && parents_index < the_closest_power) {

			path = path / 2;
			( ( path & thread_index ) == 0 ) ?  (current_choise = 0) : (current_choise = 1);

			pet_mutex[parents_index].unlock(current_choise);
			parents_index = 2 * parents_index + 1 + current_choise;
		}
		
	}

private:

	void twoDegree(int num)
	{
		while (the_closest_power < num) {
			the_closest_power *= 2;
		}
	}

	std::vector <PetMutex> pet_mutex;

	//the closest right power of two
	int the_closest_power;

};

#endif
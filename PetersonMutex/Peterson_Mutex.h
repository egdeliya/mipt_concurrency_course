#ifndef PETERSON_MUTEX_H
#define PETERSON_MUTEX_H

#include <atomic>
#include <map>

class PetersonMutex
{
public:
	PetersonMutex();

	void lock(int, int);

	void unlock(int);

private:

    int number_of_threads;
    std::map <int, std::atomic <bool> > want;
    std::atomic<int> victim;

};

#include "Peterson_Mutex_Realisation.h"

#endif //PETERSON_MUTEX_H

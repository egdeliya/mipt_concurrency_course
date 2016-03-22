#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <atomic>
#include <thread>

class Spinlock {
public:
	
	Spinlock();

	void lock();
	
	void unlock();

private:
	
	std::atomic<int> ticket_counter;
	std::atomic<int> current_ticket;
};

#include "Spinlock_Realisation.hpp"

#endif

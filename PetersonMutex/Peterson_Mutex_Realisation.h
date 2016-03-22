#ifndef PETERSON_MUTEX_REALISATION_H
#define PETERSON_MUTEX_REALISATION_H

#include <thread>

PetersonMutex::PetersonMutex() {
	victim.store(0);
}

void PetersonMutex::lock(int my_id, int others_id) {
    want[my_id].store(true);
    victim.store(my_id);

	while (want[others_id].load() && victim.load() == my_id) {
		std::this_thread::yield();
	}
}

void PetersonMutex::unlock(int my_id) {
    want[my_id].store(false);
}
#endif //PETERSON_MUTEX_REALISATION_H

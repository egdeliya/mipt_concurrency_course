//
// Created by avk on 25.02.16.
//

#ifndef DELYA_PETERSON_MUTEX_PETERSON_MUTEX_REALISATION_H
#define DELYA_PETERSON_MUTEX_PETERSON_MUTEX_REALISATION_H

//#include "Peterson_Mutex.h"

PetersonMutex::PetersonMutex(int number) {
    number_of_threads = number;
    want.resize(number_of_threads, false);
    victim.store(0);
}

void PetersonMutex::lock(int my_id, int others_id) {
    want[my_id] = true;
    victim.store();

    while(want[others_id].load() && victim.load() == my_id) {
        //wait;
    }
}

void PetersonMutex::unlock(int my_id) {
    want[my_id] = false;
}
#endif //DELYA_PETERSON_MUTEX_PETERSON_MUTEX_REALISATION_H

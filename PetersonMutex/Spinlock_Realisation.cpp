#ifndef SPINLOCK_REALISATION_HPP
#define SPINLOCK_REALISATION_HPP

Spinlock::Spinlock() {
	current_ticket.store(0);
	ticket_counter.store(0);
}

void Spinlock::lock() {

	int my_ticket = ticket_counter.fetch_add(1);

	while (my_ticket != current_ticket.load()) {
		std::this_thread::yield();
	}
}

void Spinlock::unlock() {
	current_ticket.fetch_add(1);
}

#endif

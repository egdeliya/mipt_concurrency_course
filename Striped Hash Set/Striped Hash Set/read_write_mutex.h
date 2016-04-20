#include <mutex>
#include <condition_variable>

// честный rw mutex
class rw_mutex
{
public:

	//lock на запись
	void write_lock();

	void write_unlock();

	void read_lock();

	void read_unlock();

private:
	std::mutex ring;
	std::mutex lightswitch;
	std::mutex turnstile;
	unsigned readers;
};

#include "rw_mutex.hpp"
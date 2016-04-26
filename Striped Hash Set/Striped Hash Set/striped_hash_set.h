#include "read_write_mutex.h"
#include <vector>
#include <atomic>
#include <list>

template <typename T, class H = std::hash<T>>
class striped_hash_set
{
public:
	explicit striped_hash_set(size_t num_stripes) : locks(num_stripes)
	{

	};

	striped_hash_set(size_t num_stripes, class H func) : striped_hash_set(num_stripes), hash_function(func) {};

	void add(const T&);

private:

	

	// вектор мьютексов, котрорые защищают нашу таблицу
	std::vector<rw_mutex> locks;

	// коэффициент расширения таблицы
	static constexpr double growth_factor = 0.7;

	//
	H hash_function_;
};
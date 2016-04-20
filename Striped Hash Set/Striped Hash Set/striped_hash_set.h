#include "read_write_mutex.h"
#include <vector>

//template
class striped_hash_set
{
public:
	explicit striped_hash_set(size_t num_stripes) : locks(num_stripes)
	{

	};
private:

	//вектор мьютексов, котрорые защищают нашу таблицу
	std::vector<rw_mutex> locks;

	//коэффициент расширения таблицы
	static constexpr double growth_factor = 0.7;
};
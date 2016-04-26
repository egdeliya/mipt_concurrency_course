#include "read_write_mutex.h"
#include <vector>
#include <atomic>
#include <forward_list>
#include <mutex>

template <typename T, class H = std::hash<T>>
class striped_hash_set
{
public:

	striped_hash_set(size_t num_stripes, double bounder = 0.7) : locks_(num_stripes), bounder_(bounder), table_(num_stripes) {};
	
	striped_hash_set(size_t num_stripes, H func, double bounder = 0.7) : striped_hash_set(num_stripes, bounder), hash_function_(func) {};

	void add(const T&);

	bool contains(const T&);

private:

	void rehash();

	// вектор мьютексов, котрорые защищают нашу таблицу
	std::vector<rw_mutex> locks_;

	// коэффициент расширения таблицы
	double growth_factor_ = 2;

	// пороговое значение для расширения хеш таблицы
	double bounder_ = 0.7;

	// текущее количество элементов в таблице
	int current_num_elements_;
	
	//
	H hash_function_;

	// наша таблица
	std::vector<std::forward_list<T> > table_;

};

#include "striped_hash_set.hpp"
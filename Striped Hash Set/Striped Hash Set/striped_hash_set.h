//#include "read_write_mutex.h"
#include <vector>
#include <atomic>
#include <forward_list>
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

void rw_mutex::write_lock()
{
	// писатель пробует пройти через турникет, а значит больше читатели и другие писатели не смогут пройти
	turnstile.lock();

	ring.lock();

	turnstile.unlock();
}

void rw_mutex::write_unlock()
{
	ring.unlock();
}

void rw_mutex::read_lock()
{
	// когда писатель захочет что-нибудь написать, новые читатели 
	// не смогут пройти
	turnstile.lock();
	turnstile.unlock();

	// атомарно увеличиваем счетчик читателей и захватываем мьютекс на чтение
	lightswitch.lock();

	++readers;

	// если читатель первый, то он захватывает мьютекс на чтение
	if (readers == 1)
	{
		ring.lock();
	}

	lightswitch.unlock();

}

void rw_mutex::read_unlock()
{
	// атомарно уменьшаем счетчик читателей и, если нужно, отпускаем мьютекс
	lightswitch.lock();

	--readers;

	// если больше не осталось читателей, то отпускаем мьютекс
	if (readers == 0)
	{
		ring.unlock();
	}

	lightswitch.unlock();
}

template <typename T, class H = std::hash<T>>
class striped_hash_set
{
public:

	striped_hash_set(size_t num_stripes, double bounder = 0.7) : locks_(num_stripes), bounder_(bounder), table_(num_stripes) {};
	
	striped_hash_set(size_t num_stripes, H func, double bounder = 0.7) : striped_hash_set(num_stripes, bounder), hash_function_(func) {};

	void add(const T&);

	bool contains(const T&);

	void remove(const T&);

private:

	void rehash(size_t);

	int get_bucket_index(size_t ) const;

	int get_stripe_index(size_t ) const;

	// вектор мьютексов, котрорые защищают нашу таблицу
	std::vector<rw_mutex> locks_;

	// коэффициент расширения таблицы
	double growth_factor_ = 2;

	// пороговое значение для расширения хеш таблицы
	double bounder_ = 0.7;

	// текущее количество элементов в таблице
	int current_num_elements_;
	
	// хеш-функция
	H hash_function_;

	// наша таблица
	std::vector<std::forward_list<T> > table_;

};

//#include "striped_hash_set.cpp"

template <typename T, class H = std::hash<T>>
void striped_hash_set<T, H>::add(const T& e)
{
	// проверяем, лежит ли элемент в множестве
	if (contains(e))
	{
		return;
	}

	// проверяем, нужно ли расширить нашу таблицу
	if (static_cast<double>(current_num_elements_) / table_.size() >= bounder_)
	{
		rehash(table_.size());
	}

	// вычисляем хеш от нашего значения, а заодно и индекс мьютекса
	int bucket_index = get_bucket_index(hash_function_(e));
	int mut_index = get_stripe_index(bucket_index);

	// захватываем мьютекс
	locks_[mut_index].write_lock();

	// добавляем элемент
	table_[bucket_index].push_front(e);

	// отпускаем мьютекс!!!!!!
	locks_[mut_index].write_unlock();
}

template <typename T, class H = std::hash<T>>
bool striped_hash_set<T, H>::contains(const T& element)
{
	// вычисляем хеш от нашего значения, а заодно и индекс мьютекса
	int bucket_index = get_bucket_index(hash_function_(element));
	int mut_index = get_stripe_index(bucket_index);

	bool result = false;

	// захватываем мьютекс на чтение
	locks_[mut_index].read_lock();

	// проверяем, есть ли элемент
	for (auto it : table_[bucket_index])
	{
		if (it == element) result = true;
	}

	// отпускаем мьютекс
	locks_[mut_index].read_unlock();

	return result;
}

template <typename T, class H = std::hash<T>>
void striped_hash_set<T, H>::rehash(size_t elements_number)
{
	for (size_t i = 0; i < locks_.size(); i++)
	{
		locks_[i].write_lock();

		// проверяем, нужно ли все ещё расширять таблицу
		if (i == 0 && static_cast<double>(elements_number) / table_.size() >= bounder_) continue;
		else return;
	}
	std::vector<std::forward_list<T>> new_table(table_.size() * growth_factor_);

	//table_.resize(table_.size() * growth_factor_);

	// перемещаем все элементы
	for (size_t ind = 0; ind < table_.size(); ind++)
	{
		while (!table_[ind].empty())
		{
			T element = table_[ind].front();
			table_[ind].pop_front();

			// вычисляем хеш от нашего значения, а заодно и индекс мьютекса
			int bucket_index = get_bucket_index(hash_function_(element));
			int mut_index = get_stripe_index(bucket_index);

			table_[ind].pop_front();

			// добавляем элемент
			new_table[bucket_index].push_front(element);
		}
	}

	table_.swap(new_table);

	for (size_t i = 0; i < locks_.size(); i++)
	{
		locks_[i].write_unlock();
	}
}

template <typename T, class H = std::hash<T>>
int striped_hash_set<T, H>::get_bucket_index(size_t hash_value) const
{
	return (hash_value % table_.size());
}

template <typename T, class H = std::hash<T>>
int striped_hash_set<T, H>::get_stripe_index(size_t bucket_index) const
{
	return (bucket_index % locks_.size());
}

template <typename T, class H = std::hash<T>>
void striped_hash_set<T, H>::remove(const T& element)
{
	// проверяем, лежит ли элемент в множестве
	if (!contains(element))
	{
		return;
	}

	// вычисляем хеш от нашего значения, а заодно и индекс мьютекса
	int bucket_index = get_bucket_index(hash_function_(element));
	int mut_index = get_stripe_index(bucket_index);

	// захватываем мьютекс
	locks_[mut_index].write_lock();

	// удаляем элемент
	for (auto it = table_[bucket_index].before_begin(); it != table_[bucket_index].end(); ++it)
	{
		if (it == table_[bucket_index].end())
		{
			// отпускаем мьютекс!!!!!!
			locks_[mut_index].write_unlock();
			return;
		}

		if (*std::next(it) == element)
		{
			table_[bucket_index].erase_after(it);

			// отпускаем мьютекс!!!!!!
			locks_[mut_index].write_unlock();
			return;
		}
	}

	// отпускаем мьютекс!!!!!!
	locks_[mut_index].write_unlock();
}


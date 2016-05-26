//#include "read_write_mutex.h"
#include <vector>
#include <forward_list>
#include <mutex>
#include <iostream>
#include <atomic>

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

	explicit striped_hash_set(size_t num_stripes, double bounder = 10, size_t new_gr_factor = 5) : locks_(num_stripes), growth_factor_(new_gr_factor), bounder_(bounder), current_num_elements_(0), table_(num_stripes) {};
	void add(const T&);
	bool contains(const T&);
	void remove(const T&);
	void print() const;

private:

	void rehash();
	size_t get_bucket_index(size_t) const;
	size_t get_stripe_index(size_t) const;

	// вектор мьютексов, котрорые защищают нашу таблицу
	std::vector<rw_mutex> locks_;

	// коэффициент расширения таблицы
	double growth_factor_;

	// пороговое значение для расширения хеш таблицы
	double bounder_;

	// текущее количество элементов в таблице
	std::atomic<int> current_num_elements_;

	// хеш-функция
	H hash_function_;

	// наша таблица
	std::vector<std::forward_list<T> > table_;

};

template <typename T, class H>
void striped_hash_set<T, H>::add(const T& e)
{
	// проверяем, нужно ли расширить нашу таблицу
	if (static_cast<double>(current_num_elements_.load()) / table_.size() >= bounder_)
	{
		rehash();
	}

	size_t hash_value = hash_function_(e);

	// захватываем мьютекс
	locks_[get_stripe_index(hash_value)].write_lock();

	size_t bucket_index = get_bucket_index(hash_value);

	bool find_elem = false;

	// проверяем, есть ли элемент
	for (auto it : table_[bucket_index])
	{
		if (it == e)
		{
			find_elem = true;
			break;
		}
	}

	// добавляем элемент
	if (find_elem == false)
	{
		table_[bucket_index].push_front(e);
		current_num_elements_.fetch_add(1);
	}

	// отпускаем мьютекс
	locks_[get_stripe_index(hash_value)].write_unlock();
}

template <typename T, class H>
bool striped_hash_set<T, H>::contains(const T& element)
{
	// вычисляем хеш от нашего значения
	size_t hash_value = hash_function_(element);

	// захватываем мьютекс на чтение
	locks_[get_stripe_index(hash_value)].read_lock();

	size_t bucket_index = get_bucket_index(hash_value);

	bool result = false;

	// проверяем, есть ли элемент
	for (auto it : table_[bucket_index])
	{
		if (it == element) result = true;
	}

	// отпускаем мьютекс
	locks_[get_stripe_index(hash_value)].read_unlock();

	return result;
}

template <typename T, class H>
void striped_hash_set<T, H>::rehash()
{
	size_t s = table_.size();
	for (size_t i = 0; i < locks_.size(); i++)
	{
		locks_[i].write_lock();

		// проверяем, нужно ли все ещё расширять таблицу
		if ((i == 0) && (s != table_.size()))
		{
			locks_[i].write_unlock();
			return;
		}
	}

	size_t new_size = table_.size() * growth_factor_;
	std::vector<std::forward_list<T>> new_table(new_size);

	// перемещаем все элементы
	for (size_t ind = 0; ind < table_.size(); ind++)
	{
		while (!table_[ind].empty())
		{
			T element = table_[ind].front();
			table_[ind].pop_front();

			// добавляем элемент
			new_table[hash_function_(element) % new_size].push_front(element);
		}
	}

	table_.swap(new_table);

	for (size_t i = 0; i < locks_.size(); i++)
	{
		locks_[i].write_unlock();
	}
}

template <typename T, class H>
size_t striped_hash_set<T, H>::get_bucket_index(size_t hash_value) const
{
	return (hash_value % table_.size());
}

template <typename T, class H>
size_t striped_hash_set<T, H>::get_stripe_index(size_t hash_value) const
{
	return (hash_value % locks_.size());
}

template <typename T, class H>
void striped_hash_set<T, H>::remove(const T& element)
{
	// вычисляем хеш от нашего значения, а заодно и индекс мьютекса
	size_t hash_value = hash_function_(element);

	// захватываем мьютекс
	locks_[get_stripe_index(hash_value)].write_lock();

	size_t bucket_index = get_bucket_index(hash_value);

	table_[bucket_index].remove(element);

	// отпускаем мьютекс!!!!!!
	locks_[get_stripe_index(hash_value)].write_unlock();
}

template <typename T, class H>
void striped_hash_set<T, H>::print() const
{
	for (size_t i = 0; i < table_.size(); i++)
	{
		std::cout << i << "-> ";
		for (auto it : table_[i])
		{
			std::cout << it << " ";
		}
		std::cout << std::endl;
	}
}
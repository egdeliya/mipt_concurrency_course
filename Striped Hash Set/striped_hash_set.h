#include <vector>
#include <forward_list>
#include <mutex>
#include <iostream>
#include <atomic>

template <typename T, class H = std::hash<T>>
class striped_hash_set
{
public:

	explicit striped_hash_set(size_t num_stripes, double bounder = 100., size_t new_gr_factor = 5) : locks_(num_stripes), growth_factor_(new_gr_factor), bounder_(bounder), current_num_elements_(0), table_(num_stripes * growth_factor_) {};
	void add(const T&);
	bool contains(const T&);
	void remove(const T&);
	void print() const;

private:

	void rehash(size_t);
	size_t get_bucket_index(size_t) const;
	size_t get_stripe_index(size_t) const;

	// вектор мьютексов, которые защищают нашу таблицу
	std::vector<std::mutex> locks_;

	// коэффициент расширения таблицы
	double growth_factor_;

	// пороговое значение для расширения хеш таблицы
	double bounder_;

	// текущее количество элементов в таблице
	std::atomic<size_t> current_num_elements_;

	// хеш-функция
	H hash_function_;

	// наша таблица
	std::vector<std::forward_list<T> > table_;

};

template <typename T, class H>
void striped_hash_set<T, H>::add(const T& e)
{
	size_t hash_value = hash_function_(e);

	// захватываем мьютекс
	std::unique_lock<std::mutex> lock(locks_[get_stripe_index(hash_value)]);

	size_t bucket_index = get_bucket_index(hash_value);

	table_[bucket_index].push_front(e);
	current_num_elements_.fetch_add(1);

	// проверяем, нужно ли расширить нашу таблицу
	if (static_cast<double>(current_num_elements_.load()) / table_.size() > bounder_)
	{
		size_t size = table_.size();
		lock.unlock();
		rehash(size);
	}
}

template <typename T, class H>
bool striped_hash_set<T, H>::contains(const T& element)
{
	// вычисляем хеш от нашего значения
	size_t hash_value = hash_function_(element);

	// захватываем мьютекс на чтение
	std::unique_lock<std::mutex> lock(locks_[get_stripe_index(hash_value)]);

	// проверяем, есть ли элемент
	for (auto it : table_[get_bucket_index(hash_value)])
	{
		if (it == element) return true;
	}

	return false;
}

template <typename T, class H>
void striped_hash_set<T, H>::rehash(size_t size)
{
	std::vector<std::unique_lock<std::mutex>> local_locks;
	
	local_locks.emplace_back(locks_[0]);

	// проверяем, нужно ли все ещё расширять таблицу
	if (size != table_.size())
	{
		return;
	}
	
	for (size_t i = 1; i < locks_.size(); i++)
	{
		local_locks.emplace_back(locks_[i]);
	}

	size_t new_size = size * growth_factor_;
	std::vector<std::forward_list<T>> new_table(new_size);

	// перемещаем все элементы
	for (size_t ind = 0; ind < size; ind++)
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
	std::unique_lock<std::mutex> lock(locks_[get_stripe_index(hash_value)]);

	table_[get_bucket_index(hash_value)].remove(element);

	current_num_elements_.fetch_sub(1);
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
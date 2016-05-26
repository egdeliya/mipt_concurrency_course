//#include "read_write_mutex.h"
#include <vector>
#include <forward_list>
#include <mutex>
#include <iostream>
#include <atomic>

// ������� rw mutex
class rw_mutex
{
public:

	//lock �� ������
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
	// �������� ������� ������ ����� ��������, � ������ ������ �������� � ������ �������� �� ������ ������
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
	// ����� �������� ������� ���-������ ��������, ����� �������� 
	// �� ������ ������
	turnstile.lock();
	turnstile.unlock();

	// �������� ����������� ������� ��������� � ����������� ������� �� ������
	lightswitch.lock();

	++readers;

	// ���� �������� ������, �� �� ����������� ������� �� ������
	if (readers == 1)
	{
		ring.lock();
	}

	lightswitch.unlock();

}

void rw_mutex::read_unlock()
{
	// �������� ��������� ������� ��������� �, ���� �����, ��������� �������
	lightswitch.lock();

	--readers;

	// ���� ������ �� �������� ���������, �� ��������� �������
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

	explicit striped_hash_set(size_t num_stripes, double bounder = 0.7, size_t new_gr_factor = 5) : locks_(num_stripes), growth_factor_(new_gr_factor), bounder_(bounder), current_num_elements_(0), table_(num_stripes) {};

	void add(const T&);

	bool contains(const T&);

	void remove(const T&);

	void print() const;

private:

	void rehash();

	int get_bucket_index(size_t) const;

	int get_stripe_index(size_t) const;

	// ������ ���������, �������� �������� ���� �������
	std::vector<rw_mutex> locks_;

	// ����������� ���������� �������
	double growth_factor_;

	// ��������� �������� ��� ���������� ��� �������
	double bounder_;

	// ������� ���������� ��������� � �������
	std::atomic<int> current_num_elements_;

	// ���-�������
	H hash_function_;

	// ���� �������
	std::vector<std::forward_list<T> > table_;

};

//#include "striped_hash_set.cpp"

template <typename T, class H>
void striped_hash_set<T, H>::add(const T& e)
{
	// ���������, ����� �� ������� � ���������
	if (contains(e))
	{
		return;
	}

	// ���������, ����� �� ��������� ���� �������
	if (static_cast<double>(current_num_elements_.load()) / table_.size() >= bounder_)
	{
		rehash();
	}

	// ��������� ������ � ������� � ������ ��������
	int bucket_index = get_bucket_index(hash_function_(e));
	int mut_index = get_stripe_index(bucket_index);

	// ����������� �������
	locks_[mut_index].write_lock();

	bool find_elem = false;

	// ���������, ���� �� �������
	for (auto it : table_[bucket_index])
	{
		if (it == e) 
		{
			find_elem = true;
			break;
		}
	}

	// ��������� �������
	if (find_elem == false)
	{
		table_[bucket_index].push_front(e);
		current_num_elements_.fetch_add(1);
	}

	// ��������� �������!!!!!!
	locks_[mut_index].write_unlock();
}

template <typename T, class H>
bool striped_hash_set<T, H>::contains(const T& element)
{
	// ��������� ��� �� ������ ��������, � ������ � ������ ��������
	int bucket_index = get_bucket_index(hash_function_(element));
	int mut_index = get_stripe_index(bucket_index);

	bool result = false;

	// ����������� ������� �� ������
	locks_[mut_index].read_lock();

	// ���������, ���� �� �������
	for (auto it : table_[bucket_index])
	{
		if (it == element) result = true;
	}

	// ��������� �������
	locks_[mut_index].read_unlock();

	return result;
}

template <typename T, class H>
void striped_hash_set<T, H>::rehash()
{
	size_t s = table_.size();
	for (size_t i = 0; i < locks_.size(); i++)
	{
		locks_[i].write_lock();

		// ���������, ����� �� ��� ��� ��������� �������
		if ((i == 0) && (s != table_.size()))
		{
			locks_[i].write_unlock();
			return;
		}
	}
	std::vector<std::forward_list<T>> new_table(table_.size() * growth_factor_);

	// ���������� ��� ��������
	for (size_t ind = 0; ind < table_.size(); ind++)
	{
		while (!table_[ind].empty())
		{
			T element = table_[ind].front();
			table_[ind].pop_front();

			// ��������� ������ �������
			int bucket_index = get_bucket_index(hash_function_(element));

			// ��������� �������
			new_table[bucket_index].push_front(element);
		}
	}

	table_.swap(new_table);

	for (size_t i = 0; i < locks_.size(); i++)
	{
		locks_[i].write_unlock();
	}
}

template <typename T, class H>
int striped_hash_set<T, H>::get_bucket_index(size_t hash_value) const
{
	return (hash_value % table_.size());
}

template <typename T, class H>
int striped_hash_set<T, H>::get_stripe_index(size_t bucket_index) const
{
	return (bucket_index % locks_.size());
}

template <typename T, class H>
void striped_hash_set<T, H>::remove(const T& element)
{
	// ���������, ����� �� ������� � ���������
	if (!contains(element))
	{
		return;
	}

	// ��������� ��� �� ������ ��������, � ������ � ������ ��������
	int bucket_index = get_bucket_index(hash_function_(element));
	int mut_index = get_stripe_index(bucket_index);

	// ����������� �������
	locks_[mut_index].write_lock();

	bool find_elem = false;

	// ���������, ���� �� �������
	for (auto it : table_[bucket_index])
	{
		if (it == element)
		{
			find_elem = true;
			break;
		}
	}

	if (find_elem == true)
	{
		return;
	}
	
	// ������� �������
	for (auto it = table_[bucket_index].before_begin(); it != table_[bucket_index].end(); ++it)
	{
		if (it == table_[bucket_index].end())
		{
			// ��������� �������!!!!!!
			locks_[mut_index].write_unlock();
			return;
		}

		if (*std::next(it) == element)
		{
			table_[bucket_index].erase_after(it);

			current_num_elements_.fetch_add(-1);

			// ��������� �������!!!!!!
			locks_[mut_index].write_unlock();
			return;
		}
	}

	// ��������� �������!!!!!!
	locks_[mut_index].write_unlock();
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
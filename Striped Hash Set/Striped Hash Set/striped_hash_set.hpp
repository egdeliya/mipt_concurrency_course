
template <typename T, class H = std::hash<T>>
void striped_hash_set<T, H>::add(const T& e)
{
	if (contains(e))
	{
		return;
	}
	
	// проверяем, нужно ли расширить нашу таблицу
	if (static_cast<double>(current_num_elements_) / table_.size() >= bounder_)
	{
		rehash();
	}

	// вычисляем хеш от нашего значения, а заодно и индекс мьютекса
	size_t table_index = hash_function_(e) % table_.size() ;
	size_t mut_index = table_index % locks_.size();

	// захватываем мьютекс
	locks_[mut_index].write_lock();

	// добавляем элемент
	table_[table_index].push_front(e);
	
	// отпускаем мьютекс!!!!!!
	locks_[mut_index].write_unlock();
}

template <typename T, class H = std::hash<T>>
bool striped_hash_set<T, H>::contains(const T&)
{
	return false;
}

template <typename T, class H = std::hash<T>>
void striped_hash_set<T, H>::rehash() {}
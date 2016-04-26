
template <typename T, class H = std::hash<T>>
void striped_hash_set<T, H>::add(const T& e)
{
	if (contains(e))
	{
		return;
	}
	
	// проверяем, нужно ли расширить нашу таблицу
	if (static_cast<double>(current_num_elements_) / table.size() >= bounder_)
	{
		rehash();
	}

	// вычисляем хеш от нашего значения, а заодно и индекс мьютекса
	size_t table_index = hash_function_(e) % intern.size() ;
	size_t mut_index = table_index % locks.size();

	// захватываем мьютекс
	locks[mut_index].write_lock();

	// добавляем элемент
	table[table_index].push_front(e);
	
	// отпускаем мьютекс!!!!!!
	locks[mut_index].write_unlock();
}

template <typename T, class H = std::hash<T>>
bool striped_hash_set<T, H>::contauns(const T&)
{
	return false;
}
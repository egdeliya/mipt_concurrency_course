#pragma once

template <typename T, class H = std::hash<T>>
void striped_hash_set<T>::add(const T& e)
{
	// ��������� ��� �� ������ ��������
	size_t value = hash_function_(e);

	// 
}
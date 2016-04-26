#include "read_write_mutex.h"
#include <vector>
#include <atomic>
#include <forward_list>
#include <mutex>

template <typename T, class H = std::hash<T>>
class striped_hash_set
{
public:

	striped_hash_set(size_t num_stripes, double bounder = 0.7) : locks_(num_stripes), bounder_(bounder) {};
	
	striped_hash_set(size_t num_stripes, H func, double bounder = 0.7) : striped_hash_set(num_stripes, bounder), hash_function_(func) {};

	void add(const T&);

	bool contauns(const T&);

private:

	// ������ ���������, �������� �������� ���� �������
	std::vector<rw_mutex> locks_;

	// ����������� ���������� �������
	double growth_factor_ = 2;

	// ��������� �������� ��� ���������� ��� �������
	double bounder_ = 0.7;

	// ������� ���������� ��������� � �������
	int current_num_elements_;
	
	//
	H hash_function_;

	// ���� �������
	std::vector<std::forward_list<T> > table_;

};

#include "striped_hash_set.hpp"
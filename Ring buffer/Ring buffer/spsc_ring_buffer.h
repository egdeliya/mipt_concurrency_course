#include <vector>
#include <atomic>
#include <iostream>

template <class T>
class spsc_ring_buffer {
public:
	explicit spsc_ring_buffer(size_t size) : intern(size + 1), capacity(size + 1), head(0), tail(0) {}
	bool enqueue(T v);
	bool dequeue(T& v);

	void print()
	{
		for (size_t i = 0; i < capacity; i++)
		{
			std::cout << intern[i] << " ";
		}
		std::cout << std::endl;
	}

private:
	std::vector<T> intern;
	size_t capacity;
	std::atomic<size_t> head;
	std::atomic<size_t> tail;
};

template <class T>
bool spsc_ring_buffer<T>::enqueue(T v)
{
	// ������ ������ � �����
	size_t curr_tail = tail.load(std::memory_order_relaxed);
	size_t curr_head = head.load(std::memory_order_acquire);

	// ���������, ��� ������� �� �����
	if ( curr_head == (curr_tail + 1) % capacity) return false;

	// ���������� �������
	intern[curr_tail] = v;

	// ������ �����
	tail.store((curr_tail + 1) % capacity, std::memory_order_release);

	return true;
}

template <class T>
bool spsc_ring_buffer<T>::dequeue(T& v)
{
	// ������ ������ � �����
	size_t curr_head = head.load(std::memory_order_relaxed);
	size_t curr_tail = tail.load(std::memory_order_acquire);

	// ���������, ��� ������� �� �����
	if (curr_head == curr_tail) return false;

	// ������ �������
	v = intern[curr_head];

	// ������ ������
	head.store((curr_head + 1) % capacity, std::memory_order_release);

	return true;
}

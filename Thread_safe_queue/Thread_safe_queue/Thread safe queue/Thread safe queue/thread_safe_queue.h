#include <mutex>
#include <deque>
#include <atomic>

template <class T, class Container = std::deque<T> >
class thread_safe_queue
{
public:
	explicit thread_safe_queue(size_t capacity) : shut_down(false), over_size(capacity) {}

	thread_safe_queue(thread_safe_queue<T> &) = delete;

	thread_safe_queue& operator=(thread_safe_queue<T> &) = delete;

	void enqueue(const T& item)
	{
		if (shut_down)
		{
			throw std::logic_error("Method shut down was invoked");
		}

		std::unique_lock<std::mutex> locker(mut);

		// ���� ���� ���-������ ���-������ ������� � ��������� ��� ���� ����� ������ ����� shutdown
		overflow.wait(locker, [this]() {return (intern.size() < over_size); });

		intern.push_back(std::move(item));

		empty.notify_all();

	}

	// ���� ����� ����� ��� �������� ��� ������������ ����������� 
	void enqueue(T&& item)
	{
		if (shut_down)
		{
			throw std::logic_error("Method shut down was invoked");
		}

		std::unique_lock<std::mutex> locker(mut);

		overflow.wait(locker, [this]() {return (intern.size() < over_size); });

		intern.push_back(std::move(item));

		empty.notify_all();

	}

	void pop(T& item)
	{
		std::unique_lock<std::mutex> locker(mut);

		// ��� ���� ���-������ ���-������  ������� � ������� ��� ���� �� ����� ������ ����� shutdown
		// �� ������ �������
		empty.wait(locker, [this]() {return (intern.size() > 0) || (shut_down && (intern.size() == 0)); });

		// ���� ���-�� ������ shutdown � ������� �����, �� ������ ����������, ����� ��������� ������� �� �������
		if (shut_down && (intern.size() == 0))
		{
			throw std::logic_error("The queue is empty and method shut down was invoked");
		}

		item = std::move(intern.front());

		intern.pop_front();

		overflow.notify_all();

	}

	void shutdown()
	{
		// ������ ������ ������ ���� ��� ������ ����� shut_down  ����� ����, ��� ����� �� wait
		shut_down = true;
		empty.notify_all();
		overflow.notify_all();
	}

private:
	Container intern;
	std::mutex mut;

	std::condition_variable empty;
	std::condition_variable overflow;

	std::atomic_bool shut_down;
	const size_t over_size;
};

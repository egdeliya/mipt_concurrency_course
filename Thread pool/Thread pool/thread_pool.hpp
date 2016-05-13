template <class R>
thread_pool<R>::thread_pool(): task_queue(10), shut_down(false)
{
	unsigned workers_number = default_num_workers();

	workers.resize(workers_number);

	//this method initializes all threads
	initializer();
}

template <class R>
thread_pool<R>::thread_pool(size_t workers_number) : thread_pool(workers_number, 10) {};

template <class R>
thread_pool<R>::thread_pool(size_t workers_number, size_t tasks_number) : 
	workers(workers_number), task_queue(tasks_number), shut_down(false)
{
	initializer();
}

template <class R>
std::future<R> thread_pool<R>::submit(std::function<R()> foo)
{
	try
	{
		async_task pointer_to_packaged_task = std::make_shared<std::packaged_task<R()>>(std::packaged_task<R()>(foo));
		task_queue.enqueue(pointer_to_packaged_task);
		return pointer_to_packaged_task->get_future();
	}
	catch (std::exception& e)
	{
		std::cout << "from submit[" << std::this_thread::get_id() << "]: " << e.what() << std::endl;
	}
}

template <class R>
void thread_pool<R>::shutdown()
{
	if (!shut_down) shut_down = true;

	for (size_t i = 0; i < workers.size(); i++)
	{
		workers[i].first = false;

	}

	for (size_t i = 0; i < workers.size(); i++)
	{
		//we have to do that to thread check workers[i].first
		task_queue.enqueue(std::make_shared<std::packaged_task<R()>>([]() {return R(); }));
	}

	for (size_t i = 0; i < workers.size(); i++)
	{
		workers[i].second.join();
	}
}

template <class R>
thread_pool<R>::~thread_pool()
{
	if (!shut_down) shutdown();
}

template <class R>
void thread_pool<R>::initializer()
{
	for (size_t i = 0; i < workers.size(); i++)
	{
		workers[i].first = true;
		workers[i].second = std::thread(&thread_pool<R>::invoke, this, i);
	}
}

template <class R>
void thread_pool<R>::invoke(size_t thread_index)
{
	try
	{
		while (workers[thread_index].first)
		{
			std::shared_ptr<async_task> current_task = task_queue.pop();
			current_task->operator->();
		}
	}
	catch (std::exception& e)
	{
		std::cout << "from invoke " << e.what() << std::endl;
	}
}

template <class R>
unsigned thread_pool<R>::default_num_workers()
{
	unsigned threads_number = std::thread::hardware_concurrency();

	if (threads_number < 0)
	{
		return 4;
	}
	else
	{
		return threads_number;
	}
}
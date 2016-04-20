#pragma once

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

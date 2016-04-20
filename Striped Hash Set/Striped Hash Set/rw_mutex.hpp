#pragma once

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

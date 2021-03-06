#include "striped_hash_set.h"
#include <iostream>
#include <complex>

static const int stripes_number = 10;
static const int elements_number = 50000;
striped_hash_set<int> hash_table(stripes_number);

void add(int i)
{
	try
	{
		hash_table.add(i);
	} 
	catch (std::exception e)
	{
		std::cout << "from hash table add " << e.what() << std::endl;
	}
}

void striped_hash_set_test()
{

	for (int i = 0; i < elements_number; i++)
	{
		try
		{
			hash_table.add(i);
		}
		catch (std::exception e)
		{
		std::cout << "from test " << e.what() << std::endl;
		}
	}
}

int main()
{
	// минимальный тест надобавление
	striped_hash_set_test();
	
	system("pause");
	return 0;
}
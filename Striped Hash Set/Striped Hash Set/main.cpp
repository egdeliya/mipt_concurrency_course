#include "striped_hash_set.h"
#include <iostream>
#include <complex>

static const int stripes_number = 10;
static const int elements_number = 50;
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
	//std::vector<std::thread> 

	//hash_table.print();

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
		/*try
		{
			std::thread th(add, i); 
		}
		catch (std::exception e)
		{
			std::cout << "from test " << e.what() << std::endl;
		}*/
		
	}

	hash_table.print();


	//hash_table.remove(4);

	//std::cout << hash_table.contains(51) << std::endl;
}

int main()
{/*
	std::vector<int> a{ 1,3,4,5,7 };
	std::vector<int> b{ 9,9,9 };

	a.swap(b);
	for (const auto& it: a)
	{
		std::cout << it << " ";
	}
	std::cout << std::endl;
	for (const auto& it : b)
	{
		std::cout << it << " ";
	}*/
	
	striped_hash_set_test();
	
	//hash_table.rehash(10);

	system("pause");
	return 0;
}
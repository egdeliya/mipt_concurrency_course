#include "striped_hash_set.h"
#include <iostream>

void rw_mutex_test()
{

}
// русско язычный комментарий - работай!!!!
// ну плз
// русский язык...
int main()
{
	striped_hash_set<int> hash_table(5);
	
	for (size_t i = 0; i < 50; i++)
	{
		hash_table.add(i);
	}

	hash_table.remove(4);
	
	std::cout << hash_table.contains(51) << std::endl;
	
	rw_mutex_test();

	system("pause");
	return 0;
}
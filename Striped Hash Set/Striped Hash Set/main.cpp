#include <iostream>
#include "striped_hash_set.h"

void rw_mutex_test()
{

}
// ������ ������� ����������� - �������!!!!
int main()
{
	striped_hash_set<int> hash_table(5);

	rw_mutex_test();
	return 0;
}
#pragma once

#include <vector>

class Heap {
public:
	Heap(size_t s) : size(s) 
	{
		arr.resize(size);
	}
	
	void siftUp(int elements_key)
	{
		if (elements_key <= 0) return;
		int parent = (elements_key - 1) / 2;
		if (arr[elements_key] > arr[parent]) 
		{
			swap(arr[elements_key], arr[parent]);
			siftUp(parent);
		}
	}

	void siftDown(int elements_key) 
	{
		int left = elements_key * 2 + 1;
		int right = elements_key * 2 + 2;

		int large = elements_key;
		if (left < size && arr[large] < arr[left]) large = left;
		if (right < size && arr[large] < arr[right]) large = right;

		if (arr[large] <= arr[elements_key]) return;
		else
		{
			swap(arr[large], arr[elements_key]);
			siftDown(large);
		}

	}

	void heapBuild()
	{
		int middle = (size - 1) / 2;
		for (int i = middle - 1; i >= 0; i--)
		{
			siftDown(i);
		}
	}

private:
	
	void swap(int& a, int& b) 
	{
		int buf = a;
		a = b;
		b = buf;
	}

	size_t size;
	std::vector <int> arr;
};
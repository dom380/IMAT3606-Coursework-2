#pragma once
#ifndef HANDLE_H
#define HANDLE_H
#include <assert.h>

class Handle {
public:
	Handle()
	{
		this->index = 0;
		this->counter = 0;
	}
	Handle(unsigned int index) 
	{
		assert(index <= MAX_INDEX);
		++currentCounter;
		if (currentCounter > MAX_COUNTER) currentCounter = 1;
		this->index = index;
		this->counter = currentCounter;
	}
	unsigned int getIndex() 
	{
		return index;
	}
	unsigned int getCounter() 
	{
		return counter;
	}

	bool isNull()
	{
		return counter == 0;
	}
private:
	static unsigned int currentCounter;
	unsigned int MAX_INDEX = (1 << 16) - 1;
	unsigned int MAX_COUNTER = (1 << 16) - 1;
	unsigned index : 16;
	unsigned counter : 16;
};

unsigned int Handle::currentCounter = 0;
#endif // !HANDLE

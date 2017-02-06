#pragma once
#ifndef HANDLE_H
#define HANDLE_H
#include <assert.h>

class Handle {
public:
	Handle();
	Handle(unsigned int index);
	unsigned int getIndex();
	unsigned int getCounter();
	bool isNull();

private:
	static unsigned int currentCounter;
	unsigned int MAX_INDEX = (1 << 16) - 1;
	unsigned int MAX_COUNTER = (1 << 16) - 1;
	unsigned index : 16;
	unsigned counter : 16;
};
bool operator!=(Handle lhs, Handle rhs);
bool operator==(Handle lhs, Handle rhs);
#endif // !HANDLE

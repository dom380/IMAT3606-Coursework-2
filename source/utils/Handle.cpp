#include <utils\Handle.h>

unsigned int Handle::currentCounter = 0;

Handle::Handle()
{	
	index = 0;
	counter = 0;
}

Handle::Handle(unsigned int index)
{
	assert(index <= MAX_INDEX);
	++currentCounter;
	if (currentCounter > MAX_COUNTER) currentCounter = 1;//0 reserved to signal pointing to a null object
	this->index = index;
	this->counter = currentCounter;
}

unsigned int Handle::getIndex()
{
	return index;
}

unsigned int Handle::getCounter()
{
	return counter;
}

bool Handle::isNull()
{
	return counter == 0;
}

bool operator!=(Handle lhs, Handle rhs)
{
	return (lhs.getIndex() != rhs.getIndex() || lhs.getCounter() != rhs.getCounter());
}

bool operator==(Handle lhs, Handle rhs)
{
	return (lhs.getCounter() == rhs.getIndex()) && (lhs.getCounter() == rhs.getCounter());
}

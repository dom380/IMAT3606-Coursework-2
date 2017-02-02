#pragma once

#ifndef HANDLEMANAGER_H
#define HANDLEMANAGER_H

#include <vector>
#include "Handle.h"

template <typename T>
class HandleManager {
public:
	T* get(Handle handle) 
	{
		if (handle.isNull()) return nullptr;
		auto index = handle.getIndex();
		if (index >= data.size() || counters[index] != handle.getCounter())
		{
			return nullptr; //Handle is pointing to something that doesn't exist anymore (if it ever did)
		}
		return &data[index];
	}

	Handle add(T object)
	{
		Handle handle;
		if (freeIndices.empty()) 
		{
			unsigned int index = data.size();
			handle = Handle(index);
			data.push_back(object);
			counters.push_back(handle.getCounter());
		} 
		else
		{
			unsigned int index = freeIndices.back();
			freeIndices.pop_back();
			handle = Handle(index);
			data[index] = object;
			counters[index] = handle.getCounter();
		}
	}


private:
	std::vector<T> data;
	std::vector<unsigned int> freeIndices;
	std::vector<unsigned int> counters;


};

#endif // !HANDLEMANAGER_H

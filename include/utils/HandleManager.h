#pragma once
#pragma warning( disable : 4267)
#ifndef HANDLEMANAGER_H
#define HANDLEMANAGER_H

#include <vector>
#include "Handle.h"

template <typename T>
class HandleManager {
public:
	HandleManager() {};
	/*
		Retrieves a point to the corresponding object for the given handle.
		If the handle is no longer valid then a nullptr is returned instead. 
	*/
	T* get(Handle handle) 
	{
		if (handle.isNull()) return nullptr;
		auto index = handle.getIndex();
		if (index >= data.size() || counters[index] != handle.getCounter())
		{
			return nullptr; //Handle is pointing to something that doesn't exist anymore (if it ever did)
		}
		return &data[index].second;
	}

	/*
		Stores the object and returns a Handle to it.
	*/
	Handle add(T &object)
	{
		Handle handle;
		if (freeIndices.empty()) 
		{
			unsigned int index = data.size();
			handle = Handle(index);
			data.push_back(std::pair<int, T>(index,object));
			counters.push_back(handle.getCounter());
		} 
		else
		{
			unsigned int index = freeIndices.back();
			freeIndices.pop_back();
			handle = Handle(index);
			data[index].second = object;
			counters[index] = handle.getCounter();
		}
		return handle;
	}

	/*
		Removes the corresponding object of the handle.
	*/
	void remove(Handle handle)
	{
		auto index = handle.getIndex();
		assert(index < data.size());
		assert(counters[index] == handle.getCounter());
		freeIndices.push_back(index);
		data[index].first = -1;
		counters[index] = 0;
	}
	
	std::vector<std::pair<int, T>>* getAll()
	{
		return &data;
	}

private:
	std::vector<std::pair<int,T>> data;
	std::vector<unsigned int> freeIndices;
	std::vector<unsigned int> counters;


};

#endif // !HANDLEMANAGER_H

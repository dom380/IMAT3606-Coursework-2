#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include "Components\Component.h"
#include <vector>
using std::vector;
#include <memory> 
using std::shared_ptr;

/*
	Generic GameObject class.
*/
class GameObject
{
public:
	//Default constructor.
	GameObject() {};
	/*
		Returns the object's component of specified type.
		If Object doesn't have a component of that type nullptr is returned.
	*/
	shared_ptr<Component> GetComponent(ComponentType type);
	/*
		Adds the specified component to the GameObject.
		shared_ptr<Component> comp, The component to add.
	*/
	void AddComponent(shared_ptr<Component> comp);
	/*
		Checks if GameObject has component of this type.
		ComponentType type, The type to check for.
		Returns true if Object has a component of this type.
	*/
	bool HasComponent(ComponentType type);
	/*
		Calls the update method on all components
		double dt, time step.
	*/
	void updateComponents(double dt);
	/*
		Passes the message to all components.
		Message* msg, Pointer to the message to pass.
	*/
	void notifyAll(Message* msg);
private:
	vector<shared_ptr<Component>> components;
};

#endif // !GAMEOBJECT_H

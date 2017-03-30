#pragma once
#ifndef COMPONENT_H
#define COMPONENT_H
#include "ComponentType.h"
#include "Message.h"
#include "MessageType.h"
/*
	Abstract base class of GameObject Components.
*/
class Component
{
public:
	/*
		Constructor
		ComponentType type, Enum specifying the type of the Component.
	*/
	Component(ComponentType type);
	/*
		Returns the component's type.
	*/
	ComponentType getType();
	/*
		Checks if the component is the specified type.
		ComponentType type, Type to check against.
		Returns true if the same type.
	*/
	bool isType(ComponentType type);
	/*
		Pure Virtual update function. 
		Implementations can use this method to handle time based processing that occurs 
		every frame (Physics, animation etc.).
	*/
	virtual void update(double dt) = 0;
	/*
		Pure Virtual messaging function.
		Implementations should use this method to listen for specific events
		or data from other components. For example, A Model may listen for
		RenderMessages containing camera and lighting data or a Collectible
		could listen for player location updates.

		Message* msg, Pointer to some implementation of Message.
	*/
	virtual void RecieveMessage(Message* msg) = 0;
private:
	ComponentType type;

};
#endif // !COMPONENT_H

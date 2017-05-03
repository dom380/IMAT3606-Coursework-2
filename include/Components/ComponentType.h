#pragma once
#ifndef COMPONENTTYPE_H
#define COMPONENTTYPE_H
/*
	Enum specifying the type of component. Also used to index the component array of GameObjects
*/
enum ComponentType
{
	MODEL,
	TRANSFORM,
	ANIMATION,
	RIGID_BODY,
	LOGIC,
	TRIGGER,
	CONTROLLER,
	COMPONENT_TYPE_COUNT //ALWAYS LEAVE THIS AS THE LAST ENUM
};
#endif // !COMPONENTTYPE_H

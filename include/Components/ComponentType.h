#pragma once
#ifndef COMPONENTTYPE_H
#define COMPONENTTYPE_H
/*
	Enum specifying the type of component. Also used to index the component array of GameObjects
*/
enum ComponentType
{
	MODEL = 0,
	ANIMATION = 1,
	RIGID_BODY = 2,
	LOGIC = 3,
	TRANSFORM = 4,
	TRIGGER = 5,
	CONTROLLER = 6,
	COMPONENT_TYPE_COUNT //ALWAYS LEAVE THIS AS THE LAST ENUM
};
#endif // !COMPONENTTYPE_H

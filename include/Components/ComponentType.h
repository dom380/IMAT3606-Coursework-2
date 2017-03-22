#pragma once
#ifndef COMPONENTTYPE_H
#define COMPONENTTYPE_H
/*
	Enum specifying the type of component. Also used to index the component array of GameObjects
*/
namespace ComponentType {
	enum ComponentTypes
	{
		MODEL,
		ANIMATION,
		RIGID_BODY,
		LOGIC,
		TRANSFORM,
		COMPONENT_TYPE_COUNT //ALWAYS LEAVE THIS AS THE LAST ENUM
	};
	static const char* ComponentNames[] = { "model", "animation", "rigid_body", "logic", "transform" };
}
#endif // !COMPONENTTYPE_H

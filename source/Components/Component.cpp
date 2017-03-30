#include "Components\Component.h"

Component::Component(ComponentType type)
{
	this->type = type;
}

ComponentType Component::getType()
{
	return type;
}

bool Component::isType(ComponentType type)
{
	return this->type == type;
}

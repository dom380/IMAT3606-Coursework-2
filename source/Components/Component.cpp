#include "Components\Component.h"

Component::Component(ComponentType::ComponentTypes type)
{
	this->type = type;
}

ComponentType::ComponentTypes Component::getType()
{
	return type;
}

bool Component::isType(ComponentType::ComponentTypes type)
{
	return this->type == type;
}

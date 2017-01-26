#pragma once
#ifndef LOCATIONMESSAGE_H
#define LOCATIONMESSAGE_H
#include "Message.h"
#include <gl\glm\glm\glm.hpp>

/*
	Implementation of the Message interface that passes a location
	to a component.
*/
class LocationMessage : public Message
{
public:
	LocationMessage(glm::vec3 location) : Message(MsgType::LOCATION)
	{
		this->location = location;
	};
	glm::vec3 location;
};
#endif // !LOCATIONMESSAGE_H

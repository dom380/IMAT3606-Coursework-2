#pragma once
#ifndef COLLISIONMESSAGE_H
#define COLLISIONMESSAGE_H
#include "Message.h"
#include <GameObject.h>
class GameObject;

class CollisionMessage : public Message
{
public:

	CollisionMessage(GameObject* other) : Message(MsgType::COLLISION)
	{
		this->other = other;
	};
	GameObject* other;
};

#endif // !COLLISIONMESSAGE_H

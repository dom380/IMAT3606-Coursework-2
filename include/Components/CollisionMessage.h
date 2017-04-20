#pragma once
#ifndef COLLISIONMESSAGE_H
#define COLLISIONMESSAGE_H
#include "Message.h"
#include <GameObject.h>
class GameObject;

class CollisionMessage : public Message
{
public:

	CollisionMessage(GameObject* other, double dt) : Message(MsgType::COLLISION)
	{
		this->other = other;
		this->dt = dt;
	};
	GameObject* other;
	double dt;
};

#endif // !COLLISIONMESSAGE_H

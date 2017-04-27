#pragma once
#ifndef ANIMMESSAGE_H
#define ANIMMESSAGE_H
#include "Message.h"
#include <string>
class AnimMessage : public Message
{
public:
	AnimMessage(std::string animationTag) : Message(MsgType::ANIMATION)
	{
		animation = animationTag;
	}
	std::string animation;
};

#endif // !ANIMMESSAGE_H

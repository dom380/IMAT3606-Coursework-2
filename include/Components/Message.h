#pragma once
#ifndef MESSAGE_H
#define MESSAGE_H
#include"MessageType.h"

/*
	Base component message class
	TODO - Expand out to replace Mouse and Keyboard events.
*/
class Message
{
public:
	Message() {};
	Message(MsgType id)
	{
		this->id = id;
	};
	MsgType id;
};

#endif // !MESSAGE_H

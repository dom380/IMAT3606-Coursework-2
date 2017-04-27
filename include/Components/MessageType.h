#pragma once
#ifndef MESSAGETYPE_H
#define MESSAGETYPE_H

/*
	Enum specifying the type of Message.
*/
enum class MsgType
{
	ACTIVATE,
	DEACTIVATE,
	RENDER,
	LOCATION,
	MATERIAL,
	COLLISION,
	ANIMATION
};

#endif // !MESSAGETYPE_H

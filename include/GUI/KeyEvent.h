#pragma once
#ifndef KEYEVENT_H
#define KEYEVENT_H
#include "KeyEventType.h"
#include <Input\KeyCodes.h>
/*
	Class representing a Keyboard event.
	This event is passed to all KeyEvent subscribers.
*/
class KeyEvent
{
public:
	/*
		Constructor
		KeyEventType eventType, Enum specifying the event type.
		KeyCodes keyCode, An enum specifying the code of the key,
		int modifierCode, Shift, Alt and Ctrl modifier code
	*/
	KeyEvent(KeyEventType eventType, KeyCodes keyCode, int modifierCode)
	{
		type = eventType;
		key = keyCode;
		modifier = modifierCode;
	};
	KeyEventType type;
	KeyCodes key;
	int modifier;

};

#endif // !KEYEVENT_H

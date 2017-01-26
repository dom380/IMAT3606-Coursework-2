#pragma once
#ifndef KEYEVENT_H
#define KEYEVENT_H
#include "KeyEventType.h"

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
		int keyCode, The ASCII code of the key,
		int modifierCode, Shift, Alt and Ctrl modifier code
	*/
	KeyEvent(KeyEventType eventType, int keyCode, int modifierCode)
	{
		type = eventType;
		key = keyCode;
		modifier = modifierCode;
	};
	KeyEventType type;
	int key;
	int modifier;

};

#endif // !KEYEVENT_H

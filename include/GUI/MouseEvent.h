#pragma once
#ifndef EVENT_H
#define EVENT_H
#include "MouseEventType.h"
/*
	Class representing a Mouse event.
	This event is passed to all Mouse subscribers.
*/
class MouseEvent {
public:
	/*
		Constructor
		MouseEventType type, Enum specifying the type of Mouse event.
		int posX, The x coordinate, relative to the top-left of client window, of the mouse.
		int posY, The y coordinate, relative to the top-left of client window, of the mouse.
		On MOUSE_MOVED events these represent the new mouse position.
	*/
	MouseEvent(MouseEventType type, int posX, int posY) {
		x = posX;
		y = posY;
		eventType = type;
	};
	MouseEventType eventType;
	int x, y;
};

#endif // !EVENT_H

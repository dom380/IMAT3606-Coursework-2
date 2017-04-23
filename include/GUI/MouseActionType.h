#pragma once
#ifndef ACTIONTYPE_H
#define ACTIONTYPE_H
/*
Enum specifying the type of Mouse Event.
MOUSE_RELEASE == mouse button released.
MOUSE_PRESS ==  mouse button clicked.
MOUSE_REPEAT == mouse button down.
*/
enum class MouseActionType {
	MOUSE_RELEASE,
	MOUSE_PRESS,
	MOUSE_REPEAT
};

#endif // !ACTIONTYPE_H



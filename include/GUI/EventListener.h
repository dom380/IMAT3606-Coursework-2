#pragma once
#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H
#include "MouseEvent.h"
#include "KeyEvent.h"

/*
	Interface for objects that respond to input events.
*/
class EventListener {
public:
	virtual void  handle(MouseEvent event) = 0;
	virtual void  handle(KeyEvent event) = 0;
};
#endif // !EVENTLISTENER_H

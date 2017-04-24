#pragma once
#ifndef INPUT_H
#define INPUT_H
#include <vector>
using std::vector;
#include "GUI\EventListener.h"
#include "GUI\MouseEvent.h"
#include "GUI\KeyEventType.h"
#include <memory>
using std::shared_ptr;

/*
	Abstract base for Input management.
*/
class Input {
public:
	Input();
	~Input();
	/*
		Registers an EventListener for Keyboard events.
	*/
	virtual void registerKeyListener(shared_ptr<EventListener> listener);
	/*
		Registers an EventListener for Mouse events.
	*/
	virtual void registerMouseListener(shared_ptr<EventListener> listener);
	/*
		Removes an EventListener from the Key listeners.
	*/
	virtual void removeKeyListener(shared_ptr<EventListener> listener);
	/*
		Removes an EventListener from the Mouse listeners.
	*/
	virtual void removeMouseListener(shared_ptr<EventListener> listener);
	/*
		Sets the Input systems focus to the specified listener. 
		If an EventListener is the focus mouse events will only be sent to that listener.
	*/
	virtual void setMouseFocus(shared_ptr<EventListener> listener);
	/*
		Sets the Input systems focus to the specified listener.
		If an EventListener is the focus key events will only be sent to that listener.
	*/
	virtual void setKeyFocus(shared_ptr<EventListener> listener);
	/*
		Returns the current state of the specified key.
	*/
	virtual KeyEventType getKeyState(KeyCodes key) = 0;
	/*
		Clean up Key and Mouse listeners.
	*/
	void exit();
	/*
		Enum specifying the implementation of the Input system.
	*/
	enum InputImpl
	{
		GLFW,
		GLUT,
	};
protected:
	vector<shared_ptr<EventListener>>keySubs;
	vector<shared_ptr<EventListener>>mouseSubs;
	shared_ptr<EventListener> mouseFocus = nullptr;
	shared_ptr<EventListener> keyFocus = nullptr;
	bool closed = false;
};

#endif // !INPUT_H

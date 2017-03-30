#include "Input.h"

void Input::registerKeyListener(shared_ptr<EventListener> listener)
{
	keySubs.push_back(listener);
}

void Input::registerMouseListener(shared_ptr<EventListener> listener)
{
	mouseSubs.push_back(listener);
}

void Input::removeKeyListener(shared_ptr<EventListener> listener)
{
	auto it = std::find(keySubs.begin(), keySubs.end(), listener);
	if (it != keySubs.end())
	{
		keySubs.erase(it);
	}
}

void Input::removeMouseListener(shared_ptr<EventListener> listener)
{
	auto it = std::find(mouseSubs.begin(), mouseSubs.end(), listener);
	if (it != mouseSubs.end())
	{
		mouseSubs.erase(it);
	}
}

void Input::exit()
{
	if (!closed)
	{
		keySubs.clear();
		mouseSubs.clear();
		closed = true;
	}
}

Input::~Input()
{
	exit();
}

Input::Input()
{

}

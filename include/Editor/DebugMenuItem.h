#pragma once
#ifndef DEBUGMENUITEM_H
#define DEBUGMENUITEM_H
#ifndef NDEBUG
#include <algorithm>
#include <memory>
class DebugMenuItem;

using std::shared_ptr;

/*
DEBUG MENU Item

a menu item to be added to the main menu bar in debug menu.

*/

class DebugMenuItem
{
private:
	static bool initialised;

public:
	DebugMenuItem();
	virtual void debugMenuItemUpdate();
};
#endif
#endif // !DEBUGMENUITEM_H

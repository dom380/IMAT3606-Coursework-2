#pragma once
#ifndef DEBUGMENUITEM_H
#define DEBUGMENUITEM_H

#include <algorithm>
#include <memory>
#include <Editor\imgui\imgui.h>
class DebugMenuItem;

using std::shared_ptr;

/*
DEBUG MENU Item

a menu item to be added to the main menu bar in debug menu.

*/

class DebugMenuItem
{
protected:
	int id;
	bool clearable;
public:
	DebugMenuItem();
	virtual void debugMenuItemUpdate() = 0;
	bool canClear();
};

#endif // !DEBUGMENUITEM_H

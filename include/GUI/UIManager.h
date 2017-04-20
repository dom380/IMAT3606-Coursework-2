#pragma once
#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <Engine.h>
#include <Editor\DebugMenu.h>
#include <GUI\UIElement.h>

/*
UI Manager
SINGLETON

*/

class UIManager : public DebugMenuItem
{
private:
	UIManager(){};
	UIManager(UIManager const&) {};
	void operator=(UIManager const&) {};

	static bool initialised;
	static shared_ptr<UIManager> instance;

	void debugMenuItemUpdate();
public:
	static shared_ptr<UIManager> getInstance();
	void update();

};

#endif
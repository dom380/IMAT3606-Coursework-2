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

	vector<bool> opened;
	bool hasButton;
	void debugMenuItemUpdate();
	void reset();

	char uiID[64];
	char uiTextValue[64];
	char buttonScriptName[64];
	char buttonFuncName[64];
	char buttonParamID[64];
	char buttonParamText[64];
public:
	static shared_ptr<UIManager> getInstance();
	void update();

};

#endif
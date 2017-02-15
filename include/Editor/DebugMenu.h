#ifndef DEBUGMENU_H
#define DEBUGMENU_H

#include "Editor/imgui/imgui.h"
#include "Referable.h"

#include <memory>
using std::shared_ptr;
/*
DEBUG MENU
SINGLETON

Uses imgui to organise a menu bar full of options for debug.

*/



class DebugMenu
{
private:
	static bool initialised;
	static shared_ptr<DebugMenu> instance;
	bool showCube;
	bool showGameObjects;
	static float createPosition[3];
	static float createScale[3];
	static float createOrientation[3];
	//updates
	void updateMainMenu();
	void updateLogic();
	//debug
	void debugGameObjectsMenu();

	//creates
	void createCubeMenu();


public:
	DebugMenu() {};
	DebugMenu(DebugMenu const&) {};
	void operator=(DebugMenu const&) {};
	static shared_ptr<DebugMenu> getInstance();

	void init();
	void update();

	void render();
};
#endif // !DEBUGMENU_h

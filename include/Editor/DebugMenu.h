#ifndef DEBUGMENU_H
#define DEBUGMENU_H



#include <memory>
#include "Editor/imgui/imgui.h"

using std::shared_ptr;
/*
DEBUG MENU
SINGLETON

Uses imgui to organise a menu bar full of options for debug.

*/


class ModelComponent;
class DebugMenu
{
private:
	static bool initialised;
	static shared_ptr<DebugMenu> instance;
	/*
		Menu Bools
	*/
	bool showCube;
	bool showGameObjects;

	/*
		MainMenu bar update.
	*/
	void updateMainMenu();
	/*
		Specific updates for menu trees
	*/
	void updateLogic();
	/*
		Specfic menu for game objects.
	*/
	void debugGameObjectsMenu();

	/*
		Create menus
	*/
	void createCubeMenu();

	/*
		debugGameObjectsMenu Component Functions
	*/

	void gameObjectsMenuModel(int i, ModelComponent* model);
	void gameObjectsMenuAnimation();
	void gameObjectsMenuRigidBody();
	void gameObjectsMenuLogic();
	void gameObjectsMenuTransform(int i, ModelComponent* model);

public:
	DebugMenu() {};
	DebugMenu(DebugMenu const&) {};
	void operator=(DebugMenu const&) {};
	static shared_ptr<DebugMenu> getInstance();

	void init();
	/*
		Master update
	*/
	void update();

	void render();
};
#endif // !DEBUGMENU_h

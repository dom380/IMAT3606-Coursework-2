#ifndef DEBUGMENU_H
#define DEBUGMENU_H
#define NOMINMAX

#include <algorithm>
#include <memory>
#include <Editor/imgui/imgui.h>
#include <Graphics\ModelComponent.h>

using std::shared_ptr;
/*
DEBUG MENU
SINGLETON

Uses imgui to organise a menu bar full of options for debug.

*/

class DebugMenu
{
private:
	DebugMenu() {};
	DebugMenu(DebugMenu const&) {};
	void operator=(DebugMenu const&) {};

	static bool initialised;
	static shared_ptr<DebugMenu> instance;
	/*
		Menu Bools
	*/
	bool showCube;
	bool showGameObjects;
	bool showSaveAsMenu;
	bool popupActive;

	string popupText;

	//
	vector<std::string> objList;
	vector<std::string> textureList;
	vector<bool> objCreateActive;

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
		File Menus
	*/
	bool saveCurrentLevel(string fileName);
	bool saveAsLevel(string fileName);
	void saveAsMenu();

	/*
		Create menus
	*/
	void createCubeMenu();
	void createObjectWindow(std::string objName, int iterator);

	/*
		debugGameObjectsMenu Component Functions
	*/

	void gameObjectsMenuModel(int i, ModelComponent* model);
	void gameObjectsMenuAnimation();
	void gameObjectsMenuRigidBody();
	void gameObjectsMenuLogic();
	void gameObjectsMenuTransform(int i, ModelComponent* model);

	/*
		popups
	*/
	void popup(string text);

public:
	static shared_ptr<DebugMenu> getInstance();

	void init();
	/*
		Master update
	*/
	void update();

	void render();
};
#endif // !DEBUGMENU_h

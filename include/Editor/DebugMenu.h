#ifndef DEBUGMENU_H
#define DEBUGMENU_H

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
	bool showLoadMenu;
	bool popupActive;

	string popupText;

	//vectors
	vector<std::string> objList;
	vector<std::string> textureList;
	vector<std::string> levelList;
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
	/*
		Choose a name for the save file, reuses savecurrentlevel to do saving.
		NOTE: duplicates active screen as its base and then changes file name
		DOES NOT CHANGE TO THAT SCENE, ACTIVE SCENE IS STILL THE ORIGINAL LOADED FILE.
	*/
	void saveAsMenu();

	/*
		Load level provided by file name, reusues level loader class
	*/
	bool loadLevel(string fileName);
	/*
		Choose from a list of available levels. Then reuse loadLevel()
	*/
	void loadSpecificLevel();

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

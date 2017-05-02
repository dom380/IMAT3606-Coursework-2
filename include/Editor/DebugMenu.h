#ifndef DEBUGMENU_H
#define DEBUGMENU_H

#include <algorithm>
#include <memory>
#include <vector>
#include <string>
#include <Editor/imgui/imgui.h>
//#include <Graphics\ModelComponent.h>
#include <Editor\DebugMenuItem.h>
#include <GUI\UIType.h>
#include <GUI\Button.h>

using namespace std;
/*
DEBUG MENU
SINGLETON

Uses imgui to organise a menu bar full of options for debug.

*/
class ModelComponent;
class PhysicsComponent;
class Transform;
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

	string popupText;

	//vectors
	vector<DebugMenuItem*> mainMenuBarItems;
	vector<std::string> objList;
	vector<std::string> textureList;
	vector<std::string> fontList;
	std::vector<const char *> textureCStyleArray;
	std::vector<const char *> fontCStyleArray;
	vector<std::string> levelList;
	/*
		Vectors test if the window is active.
	*/
	vector<bool> objCreateActive;
	bool uiCreateActive[UIType::UI_TYPE_COUNT];

	//The current selected item in a listbox
	int listbox_item_current;
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
	void createUIWindow(UIType type, int iterator);
	shared_ptr<Button> createButton();

public:
	static shared_ptr<DebugMenu> getInstance();

	void init();
	/*
		Master update
	*/
	void update();

	void render();

	void addMenuItem(DebugMenuItem* dmi);
	void refreshMenuItems();
	vector<DebugMenuItem*> getMenuItems();

	/*
	Creates a list box of textures that are loaded from texture dir.
	discards textures with size < 4
	*/
	void createTextureListBox();
	/*
	Creates a list box of fonts that are loaded from fonts dir.
	discards fonts with size < 4
	*/
	void createFontsListBox();
	string listBoxItemSelected(UIType type);

	/*
	debugGameObjectsMenu Component Functions
	*/

	void gameObjectsMenuModel(int i, ModelComponent* model);
	void gameObjectsMenuAnimation();
	void gameObjectsMenuRigidBody(int i, PhysicsComponent* phyComp);
	void gameObjectsMenuLogic();
	void gameObjectsMenuTransform(int i, Transform* transform);
};

#endif // !DEBUGMENU_h

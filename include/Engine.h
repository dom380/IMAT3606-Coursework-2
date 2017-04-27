#pragma once
#ifndef ENGINE_H
#define ENGINE_H

#include "EngineState.h"
#include "utils\Timer.h"
#include "AssetManager.h"
#include "Renderers\Graphics.h"
#include "Input\Input.h"
#include "Screens\Screen.h"
#include "Screens\MenuScreen.h"
#include "Graphics\Window.h"
#include "Physics\Physics.h"
#include <utils\EnumParser.h>
#include <map>
using std::map;


#include <Editor\imgui\imgui.h>
#include <Editor\DebugMenu.h>
class DebugMenu;


/*
	Core engine class.
*/
class Engine {
public:
	//Default constructor.
	Engine();
	/*
		Constructor.
		int width, window width.
		int height window height.
	*/
	Engine(int width, int height);
	~Engine();
	/*
		Initialisation method that reads the configuration file
		and constructs the specifed implementations of the Window, Graphics and Input
		Systems. It then loads the specified inital Screen.
	*/
	void init();
	/*
		The core loop.
	*/
	void mainLoop();
	/*
		Shut down and clean up any resources before
		stopping the process.
	*/
	void exit();

	/*
		Registers the provided implementation of Screen with the engine. 
		Returns the ID of the newly registered screen. This ID is used in
		all subsequent calls. 
	*/
	string registerScreen(shared_ptr<Screen> screen);
	/*
		Returns a pointer to the current active screen.
	*/
	shared_ptr<Screen> getActiveScreen();
	/*
		Switches the current active screen to the screen with the specified ID.
		The screen must have been registered with the engine before hand.
		The previous screen will not be disposed of and will remain in memory.
	*/
	void switchScreen(string screenId);
	/*
		Replaces the current active screen to the screen with the specified ID.
		The screen must have been registered with the engine before hand.
		The previous screen will be disposed of and all shared assets freed.
	*/
	void replaceScreen(string screenId);

	/*
		Parses the main configuration file.
	*/
	void loadConfig();

	/*
		Loads the first Screen specified in the configuration file.
	*/
	void loadFirstLevel();

	/*
		Enum to specify Graphics implementation.
	*/
	enum GraphicsContext {
		OPEN_GL
	};

	/*
		Returns the window width in pixels.
	*/
	int getWindowWidth();

	/*
		Returns the window height in pixels.
	*/
	int getWindowHeight();

	/*
		Returns the Id of the initial screen as specified in the configuration file.
	*/
	string getInitialScreenId();
	
	/*
		Returns a pointer to the physics implementation.
	*/
	shared_ptr<Physics> getPhysics();

	/*
		Return engine window
	*/
	shared_ptr<Window> GetWindow();

	/*
		Return engine renderer
	*/
	shared_ptr<Graphics> Engine::getRenderer();

	/*
		Return ptr to InputHandler
	*/
	shared_ptr<Input> getInput();


	shared_ptr<DebugMenu> getDebugMenu();

	shared_ptr<EngineState> getEngineState();

	/*
		Global accesor to the game engine.
	*/
	static shared_ptr<Engine> g_pEngine;

private:
	//Private members
	shared_ptr<EngineState> engineState;
	shared_ptr<Window> window;
	shared_ptr<Graphics> renderer;
	shared_ptr<Input> inputHandler;
	shared_ptr<Physics> physics;
	map<string, shared_ptr<Screen>> gameScreens;
	std::pair<string,shared_ptr<Screen>> activeScreen;
	bool closed;
	bool vsync = false;
	int width;
	int height;
	Timer timer;
	string initialScreenId;
	EnumParser<GraphicsContext> enumParser = EnumParser<GraphicsContext>();
	GraphicsContext graphicsContext;
	Input::InputImpl inputImplementation;
	Physics::PhysicsImpl physicsImplementation;
	//Private Methods
	shared_ptr<Graphics> buildRenderer(GraphicsContext renderType);
	shared_ptr<Window> buildWindow(GraphicsContext context);
	shared_ptr<Input> buildInput(Input::InputImpl impl);
	shared_ptr<Physics> buildPhysics(Physics::PhysicsImpl impl);
};

#endif // !ENGINE_H


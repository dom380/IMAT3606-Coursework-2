#pragma once
#ifndef ONCLICKFUNCTIONS_H
#define ONCLICKFUNCTIONS_H
#include <string>
using std::string;
#include <functional>
using std::function;
#include <Engine.h>

/*
	Class containing all supported OnClick Callback functions.
*/
class OnClickFunctions {
public:
	/*
		Notify the engine to switch screens.
		Engine* engine, Pointer to the engine.
		string screenId, The Id of the screen to switch to.
	*/
	static void switchScreen(Engine* engine, string screenId) {
		engine->switchScreen(screenId);
	}
	/*
		Notify the engine to replace the screen. 
		This will dispose of the current active screen and all resources.
		Engine* engine, Pointer to the engine.
		string screenId, The Id of the screen to switch to.
	*/
	static void replaceScreen(Engine* engine, string screenId) {
		engine->replaceScreen(screenId);
	}

	/*
		Default, empty call back.
	*/
	static void doNothing() {};

	/*
		Notify the engine to shut down.
		Engine* engine, Pointer to the engine.
	*/
	static void exit(Engine* engine) {
		engine->exit();
	}
	
	/*
		Enum to specify which function to use.
	*/
	enum FunctionType
	{
		SWITCH_SCREEN,
		REPLACE_SCREEN,
		EXIT,
		DO_NOTHING
	};	
};

#endif // !ONCLICKFUNCTIONS_H
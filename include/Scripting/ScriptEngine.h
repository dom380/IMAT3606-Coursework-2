#pragma once
#ifndef SCRIPTFACTORY_H
#define SCRIPTFACTORY_H
#include <memory>
using std::shared_ptr;
#include <map>
using std::map;
#include <set>
using std::set;
#include <string>
using std::string;

#include <Components\Component.h>
#include <Components\LocationMessage.h>
extern "C"
{
#include <lua\lua.hpp>
#include <lua\lauxlib.h>
#include <lua\lualib.h>
}
#include <LuaBridge\LuaBridge.h>
#include "LuaStateHolder.h"

/*
	Singleton to manage the loading of scripts and binding of functions to and from lua.
*/
struct lua_State;
class ScriptEngine {
public:
	/*
		Static method to get the ScriptEngine instance.
	*/
	static shared_ptr<ScriptEngine> getInstance();
	/*
		Loads the specified script file into the lua runtime.
		string filePath - The full file path to the script file.
		string scriptName - The name (without extension) of the script file.
		Returns true if the script was loaded successfully. 
	*/
	bool loadScript(string filePath, string scriptName);
	/*
		Returns a reference to the specified function from the specified script.
		string scriptName - The name (without extension) of the script file.
		string functionName - The name of the function to retrieve.
		Returns a LuaRef. If the function or script was not found the LuaRef is nil.
	*/
	luabridge::LuaRef getFunction(string scriptName, string functionName);
	
	/*
		Cleans up the lua state. Further calls to the ScriptEngine after close has been called results in undefined behaviour.
	*/
	void close();
private:
	//Singleton setup
	ScriptEngine();
	ScriptEngine(ScriptEngine const&) {}; // prevent copies
	void operator=(ScriptEngine const&) {}; // prevent assignments
	static bool initialised;
	static shared_ptr<ScriptEngine> instance;
	//instance variables
	lua_State* luaState;
	map<std::pair<string, string>, luabridge::LuaRef>  luaFunctions;
	set<string> loadedScripts;

};

#endif
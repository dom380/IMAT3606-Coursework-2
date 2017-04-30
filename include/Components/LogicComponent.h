#pragma once
#ifndef LogicComponent_H
#define LogicComponent_H
#include "Component.h"
#include "LocationMessage.h"
#include "CollisionMessage.h"
#include <memory>
#include <Screens\GameScreen.h>
extern "C" 
{
	#include <lua\lua.h>
	#include <lua\lauxlib.h>
	#include <lua\lualib.h>
}
#include <LuaBridge\LuaBridge.h>
#include <Scripting\LuaStateHolder.h>

/*
	Component resposible for handeling game logic of objects.
	Class contains a number of Lua bindings to minimise exposure of 
	internal details to scripts.
*/
class GameScreen;

class LogicComponent : public Component
{
	friend class ScriptEngine;
public:
	LogicComponent(std::weak_ptr<GameObject> owner, std::weak_ptr<GameScreen> screen, const char* scriptFile);
	LogicComponent(std::weak_ptr<GameObject> owner, std::weak_ptr<GameScreen> screen, std::string scriptFile);
	~LogicComponent();
	virtual void update(double dt);

	virtual void RecieveMessage(Message* msg);
	void dispose() {}; //Component Interface stub
private:
	lua_State* luaState = LuaStateHolder::getLuaState();
	luabridge::LuaRef updateFunc;
	luabridge::LuaRef recieveMsgFunc;
	std::weak_ptr<GameObject> owner;
	std::weak_ptr<GameScreen> screen;
	const char* script;
	string scriptName;
	//private member functions
	void registerLuaBindings();
	//private lua bindings
	void applyTransform(glm::vec3 position, float scale, float orientation);
	void toggleRender();
	void updateScore(int incValue);
	bool isRendering();
	glm::vec3 getPosition();

};

#endif // !LogicComponent_H

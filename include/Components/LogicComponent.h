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
	/*
		Default constructor
	*/
	LogicComponent();
	/*
		Constructor
		std::weak_ptr<GameObject> owner - Weak pointer to the Gameobject this belongs to.
		std::weak_ptr<GameScreen> screen - Weak pointer to the GameScreen this component is a part of.
		const char* scriptFile - The file path to the script file.
	*/
	LogicComponent(std::weak_ptr<GameObject> owner, std::weak_ptr<GameScreen> screen, const char* scriptFile);
	/*
		Constructor
		std::weak_ptr<GameObject> owner - Weak pointer to the Gameobject this belongs to.
		std::weak_ptr<GameScreen> screen - Weak pointer to the GameScreen this component is a part of.
		std::string scriptFile - The file path to the script file.
	*/
	LogicComponent(std::weak_ptr<GameObject> owner, std::weak_ptr<GameScreen> screen, std::string scriptFile);
	/*
		Constructor
		std::weak_ptr<GameObject> owner - Weak pointer to the Gameobject this belongs to.
		std::weak_ptr<GameScreen> screen - Weak pointer to the GameScreen this component is a part of.
		std::string scriptFile - The file path to the script file.
		luabridge::LuaRef params - A Lua table containing any parameters passed to the script. Optional. Can be null.
	*/
	LogicComponent(std::weak_ptr<GameObject> owner, std::weak_ptr<GameScreen> screen, std::string scriptFile, luabridge::LuaRef params);
	/*
		Constructor
		std::weak_ptr<GameObject> owner - Weak pointer to the Gameobject this belongs to.
		std::weak_ptr<GameScreen> screen - Weak pointer to the GameScreen this component is a part of.
	*/
	LogicComponent(std::weak_ptr<GameObject> owner, std::weak_ptr<GameScreen> screen);
	~LogicComponent();
	/*
		Update function
		doublt dt - Current time step.
	*/
	virtual void update(double dt);
	/*
		Component interface
		Handles collision callback messages if the game object this belongs to also has a physics component.
		Message* msg - The message.
	*/
	virtual void RecieveMessage(Message* msg);
	void dispose() {}; //Component Interface stub

	void setOwner(std::weak_ptr<GameObject> owner);
	void setScreen(std::weak_ptr<GameScreen> screen);
	luabridge::LuaRef getParams();
	string getScriptName();
	void setScriptName(string scriptName);
	void setScriptFullPath(const char* scriptFullName);

	void registerLuaBindings();
private:
	lua_State* luaState = LuaStateHolder::getLuaState();
	luabridge::LuaRef params;
	luabridge::LuaRef updateFunc;
	luabridge::LuaRef recieveMsgFunc;
	std::weak_ptr<GameObject> owner;
	std::weak_ptr<GameScreen> screen;
	const char* script;
	string scriptName;
	//private member functions

	//private lua bindings
	void applyTransform(glm::vec3 position, float scale, glm::quat orientation);
	void resetTransform();
	void toggleRender();
	void updateScore(int incValue, string idToUpdat);
	int getUIValueInt(string id);
	bool isRendering();
	glm::vec3 getPosition();
	//Vector and Quat maths
	glm::vec3 vec3Addition(glm::vec3 v1, glm::vec3 v2);
	glm::vec3 vec3Subtract(glm::vec3 v1, glm::vec3 v2);
	glm::vec3 vec3Normalise(glm::vec3 v1);
	float getDistance(glm::vec3 v1, glm::vec3 v2);
	glm::quat angleAxis(float angle, glm::vec3 axis);
	glm::quat getOrientation();
	glm::quat quatMultiply(glm::quat q1, glm::quat q2);

};

#endif // !LogicComponent_H

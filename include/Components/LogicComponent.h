#pragma once
#ifndef LogicComponent_H
#define LogicComponent_H
#include "Component.h"
#include "LocationMessage.h"
#include <memory>
#include <Screens\GameScreen.h>
extern "C" 
{
	#include <lua\lua.h>
	#include <lua\lauxlib.h>
	#include <lua\lualib.h>
}
#include <LuaBridge\LuaBridge.h>

/*
	Component resposible for handeling game logic of objects.
	TODO : Use some form of scripting instead of hard coded logic here
*/
class GameScreen;
class LogicComponent : public Component
{
public:
	LogicComponent(std::weak_ptr<GameObject> owner, std::weak_ptr<GameScreen> screen);

	virtual void update(double dt);

	virtual void RecieveMessage(Message* msg);
private:
	std::weak_ptr<GameObject> owner;
	std::weak_ptr<GameScreen> screen;
	double angle = 0;
	double rotationSpeed = 50.0;
};

#endif // !LogicComponent_H

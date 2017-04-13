#pragma once
#ifndef COLLISIONTRIGGER_H
#define COLLISIONTRIGGER_H
#include "Component.h"
#include <Physics\Physics.h>
#include <GameObject.h>
#include <Physics\ShapeData.h>
#include <Scripting\ScriptEngine.h>
#include <Scripting\LuaStateHolder.h>
#include <memory>
#include <BulletCollision\CollisionDispatch\btGhostObject.h>

class CollisionTrigger : public Component
{
public:
	CollisionTrigger(std::shared_ptr<Physics> &physics, std::weak_ptr<GameObject>& owner, ShapeData& boundingShape, const char* scriptFile, bool triggerOnce);
	CollisionTrigger(std::shared_ptr<Physics> &physics, std::weak_ptr<GameObject>& owner, ShapeData& boundingShape, std::string scriptFile, bool triggerOnce);
	void update(double dt);
	void RecieveMessage(Message* msg);
	btGhostObject* getBody();
	/*
		Calls the script associated with this trigger.
		std::shared_ptr<GameObject> collider - Pointer to the GameObject that caused the trigger.
	*/
	void trigger(std::shared_ptr<GameObject> collider);
	void trigger(GameObject* collider);
	void setTransform(Transform transform);
	bool isTriggerOnce();
	bool isTriggered();
private:
	std::weak_ptr<GameObject> owner;
	std::string scriptName;
	std::string script;
	lua_State* luaState = LuaStateHolder::getLuaState();
	luabridge::LuaRef triggerFunc;
	bool triggerOnce = false;
	bool triggered = false;
	//TODO - This is tied to the bullet physics implementation. Make Generic.
	btGhostObject* body;
	btCollisionShape* shape;
};


#endif // !COLLISIONTRIGGER_H

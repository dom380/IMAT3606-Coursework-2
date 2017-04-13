#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include <vector>
using std::vector;
#include <memory>
#include "utils\Handle.h"
#include "ComponentStore.h"
#include <utils\EnumParser.h>
class ComponentStore;
class ModelComponent;
class Transform;
class PhysicsComponent;
class LogicComponent;
class CollisionTrigger;
/*
	Generic GameObject class.
*/

class GameObject
{
public:
	//Constructor.
	GameObject(std::shared_ptr<ComponentStore> componentStore);

	/*
		Returns the handle to the component of specified type.
		If Object doesn't have the specified component type an empty handle is return;
	*/
	Handle GetComponentHandle (ComponentType type);

	/*
		Returns a pointer to the component of the specified type.
		If the object doesn't have the component a nullptr is returned.
		Note the use of the template. The should be the concrete type of the requested component. 
		This is to avoid the caller having to perform a dynamic cast on the returned pointer.
	*/
	template <typename T>
	T* getComponent(ComponentType type)
	{
		if (!HasComponent(type))
		{
			return nullptr;
		}
		auto spStore = componentStore.lock();
		if (spStore != nullptr) 
		{
			return spStore->getComponent<T>(componentHandles[type], type);
		} 
		else
		{
			return nullptr;
		}
	};

	//TODO think of a better way of exposing this to lua
	ModelComponent* getModel();
	Transform* getTransform();
	PhysicsComponent* getPhysics();
	LogicComponent* getLogic();
	CollisionTrigger* getTrigger();
	
	/*
		Adds the specified component to the GameObject.
		T comp, The component to add.
	*/
	void AddComponent(std::shared_ptr<Component> comp, ComponentType type);
	/*
		Checks if GameObject has component of this type.
		ComponentType type, The type to check for.
		Returns true if Object has a component of this type.
	*/
	bool HasComponent(ComponentType type);
private:
	Handle componentHandles[ComponentType::COMPONENT_TYPE_COUNT];
	std::weak_ptr<ComponentStore> componentStore;
	EnumParser<ComponentType> typeParser;
};

#endif // !GAMEOBJECT_H

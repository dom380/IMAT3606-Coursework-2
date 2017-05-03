#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include "GameObjectTag.h"
#include <vector>
using std::vector;
#include <memory>
#include "utils\Handle.h"
#include "ComponentStore.h"
#include <utils\EnumParser.h>
#include <utils\UUID\guid.h>
//Forward declares
class ComponentStore;
class ModelComponent;
class Transform;
class PhysicsComponent;
class LogicComponent;
class CollisionTrigger;
class ControllerComponent;
/*
	Generic GameObject class.
*/

class GameObject
{
public:
	//Constructor.
	GameObject(std::shared_ptr<ComponentStore> componentStore);
	/*
		Constructor.
		std::shared_ptr<ComponentStore> componentStore - Pointer to the component store
		GameObjectTag tag - An enum specifying what this game object is.
	*/
	GameObject(std::shared_ptr<ComponentStore> componentStore, GameObjectTag tag);

	GameObject& operator=(GameObject& other) {
		this->id = other.id;
		this->tag = other.tag;
		this->typeParser = other.typeParser;
		this->tagParser = other.tagParser;
		this->guidGen = other.guidGen;
		this->componentStore = other.componentStore;
		
	}
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
		if (!componentStore.expired()) 
		{
			auto spStore = componentStore.lock();
			if (spStore != nullptr)
			{
				return spStore->getComponent<T>(componentHandles[type], type);
			}
			else
			{
				return nullptr;
			}
		}
		return nullptr;
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

	/*
		Returns this game object's GUID.
	*/
	Guid getId();

	/*
		Returns this game object's tag.
	*/
	GameObjectTag getTag();
	/*
		Returns the game object's tag as a string.
	*/
	string getTagString();
private:
	Handle componentHandles[ComponentType::COMPONENT_TYPE_COUNT];
	std::weak_ptr<ComponentStore> componentStore;
	EnumParser<ComponentType> typeParser;
	EnumParser<GameObjectTag> tagParser;
	GuidGenerator guidGen;
	Guid id;
	GameObjectTag tag;
};

#endif // !GAMEOBJECT_H

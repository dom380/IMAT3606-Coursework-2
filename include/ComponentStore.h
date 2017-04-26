#pragma once
#ifndef COMPONENTSTORE_H
#define COMPONENTSTORE_H
#include <memory>
#include "utils\HandleManager.h"
#include "Graphics\ModelComponent.h"
#include "Graphics\Transform.h"
#include <Components\LogicComponent.h>
#include <Components\PhysicsComponent.h>
#include <Components\CollisionTrigger.h>
class LogicComponent;
class PhysicsComponent;
class CollisionTrigger;

class ComponentStore
{
public:
	template <typename T>
	T* getComponent(Handle handle, ComponentType type)
	{
		static_assert(std::is_base_of<Component, T>::value, "T must derive from component");
		switch (type)
		{
		case ComponentType::MODEL:
			return (T*)models.get(handle);
			break;
		case ComponentType::ANIMATION:
			return nullptr;
			break;
		case ComponentType::RIGID_BODY:
			return (T*)physics.get(handle);
			break;
		case ComponentType::LOGIC:
			return (T*)logic.get(handle);
			break;
		case ComponentType::TRANSFORM:
			return (T*)transforms.get(handle);
			break;
		case TRIGGER:
			return (T*)triggers.get(handle);
		case COMPONENT_TYPE_COUNT:
			//no-op
			return nullptr;
			break;
		default:
			return nullptr;
			break;
		}
	}
	
	Handle storeComponent(std::shared_ptr<ModelComponent> component);

	Handle storeComponent(std::shared_ptr<LogicComponent> component);

	Handle storeComponent(std::shared_ptr<Transform> component);

	Handle storeComponent(std::shared_ptr<PhysicsComponent> component);
	
	Handle storeComponent(std::shared_ptr<CollisionTrigger> component);

	template <typename T>
	std::vector<std::pair<int, T>>* getAllComponents(ComponentType type)
	{
		static_assert(std::is_base_of<Component, T>::value, "T must derive from component");
		switch (type)
		{
		case ComponentType::MODEL:
			return (std::vector<std::pair<int, T>>*)models.getAll();
			break;
		case ComponentType::ANIMATION:
			return nullptr;
			break;
		case ComponentType::RIGID_BODY:
			return (std::vector<std::pair<int, T>>*)physics.getAll();
			break;
		case ComponentType::LOGIC:
			return (std::vector<std::pair<int, T>>*)logic.getAll();
			break;
		case ComponentType::TRANSFORM:
			return (std::vector<std::pair<int, T>>*)transforms.getAll();
			break;
		case TRIGGER:
			return (std::vector<std::pair<int, T>>*)triggers.getAll();
		case COMPONENT_TYPE_COUNT:
			//no-op
			return nullptr;
			break;
		default:
			return nullptr;
			break;
		}
	}
private:
	HandleManager<ModelComponent> models;
	HandleManager<LogicComponent> logic;
	HandleManager<Transform> transforms;
	HandleManager<PhysicsComponent> physics;
	HandleManager<CollisionTrigger> triggers;
};

#endif // !COMPONENTSTORE_H


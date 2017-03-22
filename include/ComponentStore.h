#pragma once
#ifndef COMPONENTSTORE_H
#define COMPONENTSTORE_H
#include <memory>
#include "utils\HandleManager.h"
#include "Graphics\ModelComponent.h"
#include "Graphics\Transform.h"
#include <Components\LogicComponent.h>
class LogicComponent;

class ComponentStore
{
public:
	template <typename T>
	T* getComponent(Handle handle, ComponentType::ComponentTypes type)
	{
		static_assert(std::is_base_of<Component, T>::value, "T must derive from component");
		switch (type)
		{
		case ComponentType::ComponentTypes::MODEL:
			return (T*)models.get(handle);
			break;
		case ComponentType::ComponentTypes::ANIMATION:
			return nullptr;
			break;
		case ComponentType::ComponentTypes::RIGID_BODY:
			return nullptr;
			break;
		case ComponentType::ComponentTypes::LOGIC:
			return (T*)logic.get(handle);
			break;
		case ComponentType::ComponentTypes::TRANSFORM:
			return (T*)transforms.get(handle);
			break;
		case ComponentType::ComponentTypes::COMPONENT_TYPE_COUNT:
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
	

	template <typename T>
	std::vector<std::pair<int, T>>* getAllComponents(ComponentType::ComponentTypes type)
	{
		static_assert(std::is_base_of<Component, T>::value, "T must derive from component");
		switch (type)
		{
		case ComponentType::ComponentTypes::MODEL:
			return (std::vector<std::pair<int, T>>*)models.getAll();
			break;
		case ComponentType::ComponentTypes::ANIMATION:
			return nullptr;
			break;
		case ComponentType::ComponentTypes::RIGID_BODY:
			return nullptr;
			break;
		case ComponentType::ComponentTypes::LOGIC:
			return (std::vector<std::pair<int, T>>*)logic.getAll();
			break;
		case ComponentType::ComponentTypes::TRANSFORM:
			return (std::vector<std::pair<int, T>>*)transforms.getAll();
			break;
		case ComponentType::ComponentTypes::COMPONENT_TYPE_COUNT:
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
};

#endif // !COMPONENTSTORE_H


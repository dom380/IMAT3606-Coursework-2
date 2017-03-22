#include "..\include\GameObject.h"

GameObject::GameObject(std::shared_ptr<ComponentStore> componentStore)
{
	this->componentStore = componentStore;
}


Handle GameObject::GetComponentHandle(ComponentType::ComponentTypes type)
{
	return componentHandles[type];
}

void GameObject::AddComponent(std::shared_ptr<Component> comp, ComponentType::ComponentTypes type)
{
	auto storePtr = componentStore.lock();
	if (storePtr != nullptr)
	{
		switch (type)
		{
		case ComponentType::MODEL:
			componentHandles[type] = storePtr->storeComponent(std::dynamic_pointer_cast<ModelComponent>(comp));
			break;
		case ComponentType::ANIMATION:
			break;
		case ComponentType::RIGID_BODY:
			break;
		case ComponentType::LOGIC:
			componentHandles[type] = storePtr->storeComponent(std::dynamic_pointer_cast<LogicComponent>(comp));
			break;
		case ComponentType::TRANSFORM:
			componentHandles[type] = storePtr->storeComponent(std::dynamic_pointer_cast<Transform>(comp));
			break;
		default:
			break;
		}
	}
}


bool GameObject::HasComponent(ComponentType::ComponentTypes type)
{
	Handle handle = componentHandles[type];
	return !handle.isNull(); //If handle is not null, it has the component
}
#include "..\include\GameObject.h"

GameObject::GameObject(std::shared_ptr<ComponentStore> componentStore):
	GameObject(componentStore, GameObjectTag::UNKNOWN)
{
	
}

GameObject::GameObject(std::shared_ptr<ComponentStore> componentStore, GameObjectTag tag)
{
	this->componentStore = componentStore;
	this->tag = tag;
	id = guidGen.newGuid();
}


Handle GameObject::GetComponentHandle(ComponentType type)
{
	return componentHandles[type];
}

ModelComponent* GameObject::getModel()
{
	return getComponent<ModelComponent>(ComponentType::MODEL);
}

Transform* GameObject::getTransform()
{
	return getComponent<Transform>(ComponentType::TRANSFORM);
}

PhysicsComponent* GameObject::getPhysics()
{
	return getComponent<PhysicsComponent>(ComponentType::RIGID_BODY);
}

LogicComponent* GameObject::getLogic()
{
	return getComponent<LogicComponent>(ComponentType::LOGIC);
}

CollisionTrigger* GameObject::getTrigger()
{
	return getComponent<CollisionTrigger>(ComponentType::TRIGGER);
}



void GameObject::AddComponent(std::shared_ptr<Component> comp, ComponentType type)
{
	auto storePtr = componentStore.lock();
	if (storePtr != nullptr)
	{
		switch (type)
		{
		case MODEL:
			componentHandles[type] = storePtr->storeComponent(std::dynamic_pointer_cast<ModelComponent>(comp));
			break;
		case ANIMATION:
			componentHandles[type] = storePtr->storeComponent(std::dynamic_pointer_cast<AnimatedModelComponent>(comp));
			break;
		case RIGID_BODY:
			componentHandles[type] = storePtr->storeComponent(std::dynamic_pointer_cast<PhysicsComponent>(comp));
			break;
		case LOGIC:
			componentHandles[type] = storePtr->storeComponent(std::dynamic_pointer_cast<LogicComponent>(comp));
			break;
		case TRANSFORM:
			componentHandles[type] = storePtr->storeComponent(std::dynamic_pointer_cast<Transform>(comp));
			break;
		case TRIGGER:
			componentHandles[type] = storePtr->storeComponent(std::dynamic_pointer_cast<CollisionTrigger>(comp));
		default:
			break;
		}
	}
}

bool GameObject::HasComponent(ComponentType type)
{
	Handle handle = componentHandles[type];
	return !handle.isNull(); //If handle is not null, it has the component
}

Guid GameObject::getId()
{
	return id;
}

GameObjectTag GameObject::getTag()
{
	return tag;
}

string GameObject::getTagString()
{
	return tagParser.getString(tag);
}

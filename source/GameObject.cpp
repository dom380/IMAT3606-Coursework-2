#include "..\include\GameObject.h"

shared_ptr<Component> GameObject::GetComponent(ComponentType type)
{
	for (shared_ptr<Component> comp : components) {
		if (comp->isType(type))
		{
			return comp;
		}
	}
	return nullptr;
}

void GameObject::AddComponent(shared_ptr<Component> comp)
{
	components.push_back(comp);
}

bool GameObject::HasComponent(ComponentType type)
{
	for (shared_ptr<Component> comp : components) {
		if (comp->isType(type))
		{
			return true;
		}
	}
	return false;
}

void GameObject::updateComponents(double dt)
{
	for (shared_ptr<Component> comp : components) {
		comp->update(dt);
	}
}

void GameObject::notifyAll(Message* msg)
{
	for (shared_ptr<Component> comp : components) {
		comp->RecieveMessage(msg);
	}
}

#include "Components\LogicComponent.h"

LogicComponent::LogicComponent(std::weak_ptr<GameObject> owner, std::weak_ptr<GameScreen> screen) : Component(ComponentType::LOGIC)
{
	this->owner = owner;
	this->screen = screen;
}

void LogicComponent::update(double dt)
{
	angle += rotationSpeed * dt;
	if (angle > 360) angle -= angle;
	std::shared_ptr<GameScreen> sp_Screen = screen.lock();
	auto sp_Owner = owner.lock(); //Access GameObject that owns this component
	if (sp_Owner != nullptr && sp_Screen != nullptr)  //If it still exists 
	{
		Handle comp = sp_Owner->GetComponentHandle (ComponentType::TRANSFORM);
		if (!comp.isNull()) { //If the GameObject has a transform component, update it's orientation.
			auto transformPtr = sp_Screen->getComponentStore()->getComponent<Transform>(comp, ComponentType::TRANSFORM);
			transformPtr->orientation.w = angle;
		}
	}
}

void LogicComponent::RecieveMessage(Message * msg)
{
	switch (msg->id)
	{
	case MsgType::LOCATION: //Notified of player location
	{
		LocationMessage* locationMsg = ((LocationMessage *)msg);
		glm::vec3 location = locationMsg->location;
		auto sp_Owner = owner.lock();
		auto sp_Screen = screen.lock();
		if (sp_Owner != nullptr && sp_Screen != nullptr)
		{
			auto transformComp = sp_Owner->GetComponentHandle(ComponentType::TRANSFORM);
			if (!transformComp.isNull()) {	//Access owner GameObject's transform if it's there.
				auto transformPtr = sp_Screen->getComponentStore()->getComponent<Transform>(transformComp, ComponentType::TRANSFORM);
				glm::vec2 dist(location.x - transformPtr->position.x, location.z - transformPtr->position.z);
				float distance = glm::length(dist);
				if (distance < 8) //If player is near to this GameObject
				{
					auto modelComp = sp_Owner->GetComponentHandle(ComponentType::MODEL);
					if (!modelComp.isNull())
					{
						//std::shared_ptr<ModelComponent> model = std::dynamic_pointer_cast<ModelComponent, Component>(modelComp);
						ModelComponent* model = sp_Screen->getComponentStore()->getComponent<ModelComponent>(modelComp, ComponentType::MODEL);
						if (model->isDrawing()) //Tell GameObject's model to stop rendering if it exists and hasn't already been collected
						{
							model->toggleDrawing();
							auto sp_screen = screen.lock();
							if (sp_screen != nullptr) sp_screen->updateScore(); //Update the score as object has been collected.
						}
					}
				}
			}
		}
	}
	break;
	default:
		break;
	}
}


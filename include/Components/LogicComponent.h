#pragma once
#ifndef LogicComponent_H
#define LogicComponent_H
#include "Component.h"
#include "LocationMessage.h"
#include "GameObject.h"
#include "Graphics\Transform.h"
#include "Graphics\ModelComponent.h"
#include <memory>
#include "Screens\GameScreen.h"

/*
	Component resposible for handeling game logic of objects.
	TODO : Use some form of scripting instead of hard coded logic here
*/
class LogicComponent : public Component
{
public:
	LogicComponent(std::weak_ptr<GameObject> owner, std::weak_ptr<GameScreen> screen) : Component(ComponentType::LOGIC)
	{
		this->owner = owner;
		this->screen = screen;
	};

	virtual void update(double dt)
	{
		angle += rotationSpeed * dt;
		if (angle > 360) angle -= angle;
		auto sp_Owner = owner.lock(); //Access GameObject that owns this component
		if (sp_Owner != nullptr)  //If it still exists 
		{
			auto comp = sp_Owner->GetComponent(ComponentType::TRANSFORM); 
			if (comp != nullptr) { //If the GameObject has a transform component, update it's orientation.
				std::shared_ptr<Transform> transformPtr = std::dynamic_pointer_cast<Transform, Component>(comp);
				transformPtr->orientation.w = angle;
			}
		}
	};

	virtual void RecieveMessage(Message* msg)
	{
		switch (msg->id)
		{
		case MsgType::LOCATION: //Notified of player location
			{
				LocationMessage* locationMsg = ((LocationMessage *)msg);
				glm::vec3 location = locationMsg->location;
				auto sp_Owner = owner.lock();
				if (sp_Owner != nullptr)
				{
					auto transformComp = sp_Owner->GetComponent(ComponentType::TRANSFORM); 
					if (transformComp != nullptr) {	//Access owner GameObject's transform if it's there.
						std::shared_ptr<Transform> transformPtr = std::dynamic_pointer_cast<Transform, Component>(transformComp);
						glm::vec2 dist(location.x - transformPtr->position.x, location.z - transformPtr->position.z);
						float distance = glm::length(dist);
						if (distance < 8) //If player is near to this GameObject
						{
							auto modelComp = sp_Owner->GetComponent(ComponentType::MODEL); 
							if (modelComp != nullptr)
							{
								std::shared_ptr<ModelComponent> model = std::dynamic_pointer_cast<ModelComponent, Component>(modelComp);
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
	};
private:
	std::weak_ptr<GameObject> owner;
	std::weak_ptr<GameScreen> screen;
	double angle = 0;
	double rotationSpeed = 50.0;
};

#endif // !LogicComponent_H

#include "Components\LogicComponent.h"
#include <Scripting\ScriptEngine.h>
#include <utils/Utilities.h>

LogicComponent::LogicComponent(std::weak_ptr<GameObject> owner, std::weak_ptr<GameScreen> screen, const char* scriptFile) : Component(ComponentType::LOGIC), updateFunc(luaState), recieveMsgFunc(luaState)
{
	this->owner = owner;
	this->screen = screen;
	script = scriptFile;
	auto splitPath = Utilities::splitFilePath(script);
	scriptName = Utilities::removeExtension(splitPath.at(splitPath.size() - 1));
	registerLuaBindings();
}

LogicComponent::LogicComponent(std::weak_ptr<GameObject> owner, std::weak_ptr<GameScreen> screen, std::string scriptFile) : 
	LogicComponent(owner, screen, scriptFile.c_str())
{
	//Just a delegation constructor
}

LogicComponent::~LogicComponent()
{

}

void LogicComponent::update(double dt)
{
	try
	{
		if (updateFunc.isFunction())
		{
			updateFunc(this, dt);
		}
	}
	catch (luabridge::LuaException e)
	{
		//Todo - better error handling here
		std::cout << e.what() << std::endl;
		throw std::runtime_error(e.what());
	}
}

void LogicComponent::RecieveMessage(Message * msg)
{
	switch (msg->id)
	{
	case MsgType::LOCATION: //Notified of player location
	{
		LocationMessage* locationMsg = ((LocationMessage *)msg);
		try
		{
			if (recieveMsgFunc.isFunction())
			{
				recieveMsgFunc(this, locationMsg, "LOCATION"); //TODO - Lua doesn't support enums so find better way of translating to scripts
			}
		}
		catch (luabridge::LuaException e)
		{
			//Todo - better error handling here
			std::cout << e.what() << std::endl;
			throw std::runtime_error(e.what());
		}
	}
	break;
	default:
		break;
	}
}

void LogicComponent::registerLuaBindings()
{
	auto scriptEngine = ScriptEngine::getInstance();
	scriptEngine->loadScript(script, scriptName);
	updateFunc = scriptEngine->getFunction(scriptName, "update");
	recieveMsgFunc = scriptEngine->getFunction(scriptName, "RecieveMessage");
}

void LogicComponent::applyTransform(glm::vec3 position, float scale, float orientation)
{
	std::shared_ptr<GameScreen> sp_Screen = screen.lock(); //Access GameScreen
	auto sp_Owner = owner.lock(); //Access GameObject that owns this component
	if (sp_Owner != nullptr && sp_Screen != nullptr)  //If it still exists 
	{
		Handle comp = sp_Owner->GetComponentHandle (ComponentType::TRANSFORM);
		Transform* transformPtr = nullptr;
		if (!comp.isNull()) //If the GameObject has a transform component, update it's transform.
		{ 
			transformPtr = sp_Screen->getComponentStore()->getComponent<Transform>(comp, ComponentType::TRANSFORM);
			transformPtr->position += position;
			transformPtr->scale *= scale;
			transformPtr->orientation = glm::angleAxis(glm::radians(orientation), glm::vec3(0.0, 1.0, 0.0));
		}
		comp = sp_Owner->GetComponentHandle(ComponentType::RIGID_BODY);
		if (!comp.isNull()) //If the GameObject also has a physics component, update it's transform.
		{
			auto physicsPtr = sp_Screen->getComponentStore()->getComponent<PhysicsComponent>(comp, ComponentType::RIGID_BODY);
			if(transformPtr != nullptr)
				physicsPtr->setTransform(transformPtr);
		}
	}
}

void LogicComponent::toggleRender()
{
	auto sp_Owner = owner.lock();
	auto sp_Screen = screen.lock();
	if (sp_Owner != nullptr && sp_Screen != nullptr)
	{
		auto modelComp = sp_Owner->GetComponentHandle(ComponentType::MODEL);
		if (!modelComp.isNull())
		{
			ModelComponent* model = sp_Screen->getComponentStore()->getComponent<ModelComponent>(modelComp, ComponentType::MODEL);
			if (model->isDrawing()) //Tell GameObject's model to stop rendering if it exists and hasn't already been collected
			{
				model->toggleDrawing();
			}
		}
	}
}

void LogicComponent::updateScore(int incValue)
{
	auto sp_Screen = screen.lock();
	if (sp_Screen != nullptr)
	{
		sp_Screen->updateScore(incValue);
	}
}

bool LogicComponent::isRendering()
{
	auto sp_Owner = owner.lock();
	auto sp_Screen = screen.lock();
	if (sp_Owner != nullptr && sp_Screen != nullptr)
	{
		auto modelComp = sp_Owner->GetComponentHandle(ComponentType::MODEL);
		if (!modelComp.isNull())
		{
			ModelComponent* model = sp_Screen->getComponentStore()->getComponent<ModelComponent>(modelComp, ComponentType::MODEL);
			return model->isDrawing();
		}
	}
	else
	{
		return false;
	}
}

glm::vec3 LogicComponent::getPosition()
{
	std::shared_ptr<GameScreen> sp_Screen = screen.lock(); //Access GameScreen
	auto sp_Owner = owner.lock(); //Access GameObject that owns this component
	if (sp_Owner != nullptr && sp_Screen != nullptr)  //If it still exists 
	{
		Handle comp = sp_Owner->GetComponentHandle(ComponentType::TRANSFORM);
		if (!comp.isNull())  //If the GameObject has a transform component, update it's orientation.
		{
			auto transform = sp_Screen->getComponentStore()->getComponent<Transform>(comp, ComponentType::TRANSFORM);
			return transform->position;
		}
	} 
	else
	{
		return vec3();
	}
}



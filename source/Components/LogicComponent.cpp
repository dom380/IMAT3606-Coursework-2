#include "Components\LogicComponent.h"
#include <Scripting\ScriptEngine.h>
#include <utils/Utilities.h>

LogicComponent::LogicComponent() : Component(ComponentType::LOGIC), updateFunc(luaState), recieveMsgFunc(luaState), params(luaState)
{
}

LogicComponent::LogicComponent(std::weak_ptr<GameObject> owner, std::weak_ptr<GameScreen> screen, const char* scriptFile) : Component(ComponentType::LOGIC), updateFunc(luaState), recieveMsgFunc(luaState), params(luaState)
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

LogicComponent::LogicComponent(std::weak_ptr<GameObject> owner, std::weak_ptr<GameScreen> screen, std::string scriptFile, luabridge::LuaRef params) : Component(ComponentType::LOGIC), updateFunc(luaState), recieveMsgFunc(luaState), params(luaState)
{
	this->owner = owner;
	this->screen = screen;
	this->params = params;
	script = scriptFile.c_str();
	auto splitPath = Utilities::splitFilePath(script);
	scriptName = Utilities::removeExtension(splitPath.at(splitPath.size() - 1));
	registerLuaBindings();
}

LogicComponent::LogicComponent(std::weak_ptr<GameObject> owner, std::weak_ptr<GameScreen> screen) : Component(ComponentType::LOGIC), updateFunc(luaState), recieveMsgFunc(luaState), params(luaState)
{
	this->owner = owner;
	this->screen = screen;
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
			updateFunc(this, dt, params);
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
				recieveMsgFunc(this, locationMsg, "LOCATION", params); //TODO - Lua doesn't support enums so find better way of translating to scripts
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
	case MsgType::COLLISION:
	{
		CollisionMessage* collisionMsg = ((CollisionMessage *)msg);
		try
		{
			if (recieveMsgFunc.isFunction())
			{
				recieveMsgFunc(this, collisionMsg, "COLLISION", params); //TODO - Lua doesn't support enums so find better way of translating to scripts
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

void LogicComponent::setOwner(std::weak_ptr<GameObject> passedOwner)
{
	owner = passedOwner;
}

void LogicComponent::setScreen(std::weak_ptr<GameScreen> passedScreen)
{
	screen = passedScreen;
}

luabridge::LuaRef LogicComponent::getParams()
{
	return params;
}

string LogicComponent::getScriptName()
{
	return scriptName;
}

void LogicComponent::setScriptName(string passedName)
{
	scriptName = passedName;
}

void LogicComponent::setScriptFullPath(const char* scriptFullName)
{
	script = scriptFullName;
}

void LogicComponent::registerLuaBindings()
{
	auto scriptEngine = ScriptEngine::getInstance();
	scriptEngine->loadScript(script, scriptName);
	updateFunc = scriptEngine->getFunction(scriptName, "update");
	recieveMsgFunc = scriptEngine->getFunction(scriptName, "RecieveMessage");
}

void LogicComponent::applyTransform(glm::vec3 position, float scale, glm::quat orientation)
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
			transformPtr->orientation = orientation;
		}
		comp = sp_Owner->GetComponentHandle(ComponentType::RIGID_BODY);
		if (!comp.isNull()) //If the GameObject also has a physics component, update it's transform.
		{
			auto physicsPtr = sp_Screen->getComponentStore()->getComponent<PhysicsComponent>(comp, ComponentType::RIGID_BODY);
			if(transformPtr != nullptr)
				physicsPtr->setTransform(transformPtr);
		}
		comp = sp_Owner->GetComponentHandle(ComponentType::CONTROLLER);
		if (!comp.isNull()) //If the GameObject has a controller, update it's transform.
		{
			auto controllerPtr = sp_Screen->getComponentStore()->getComponent<ControllerComponent>(comp, ComponentType::CONTROLLER);
			if (controllerPtr != nullptr)
				controllerPtr->setTransform(transformPtr);
		}
		comp = sp_Owner->GetComponentHandle(ComponentType::TRIGGER);
		if (!comp.isNull()) //If the GameObject has a controller, update it's transform.
		{
			auto triggerPtr = sp_Screen->getComponentStore()->getComponent<CollisionTrigger>(comp, ComponentType::TRIGGER);
			if (triggerPtr != nullptr)
				triggerPtr->setTransform(*transformPtr);
		}
	}
}

void LogicComponent::resetTransform()
{
	std::shared_ptr<GameScreen> sp_Screen = screen.lock(); //Access GameScreen
	auto sp_Owner = owner.lock(); //Access GameObject that owns this component
	if (sp_Owner != nullptr && sp_Screen != nullptr)  //If it still exists 
	{
		Handle comp = sp_Owner->GetComponentHandle(ComponentType::TRANSFORM);
		Transform* transformPtr = nullptr;
		if (!comp.isNull()) //If the GameObject has a transform component, update it's transform.
		{
			transformPtr = sp_Screen->getComponentStore()->getComponent<Transform>(comp, ComponentType::TRANSFORM);
			transformPtr->position = transformPtr->getOriginalPos();
			transformPtr->orientation = transformPtr->getOrigianlOrient();
		}
		comp = sp_Owner->GetComponentHandle(ComponentType::RIGID_BODY);
		if (!comp.isNull()) //If the GameObject also has a physics component, update it's transform.
		{
			auto physicsPtr = sp_Screen->getComponentStore()->getComponent<PhysicsComponent>(comp, ComponentType::RIGID_BODY);
			if (transformPtr != nullptr)
			{
				physicsPtr->setTransform(transformPtr);
			}
		}
		comp = sp_Owner->GetComponentHandle(ComponentType::CONTROLLER);
		if (!comp.isNull()) //If the GameObject has a controller, update it's transform.
		{
			auto controllerPtr = sp_Screen->getComponentStore()->getComponent<ControllerComponent>(comp, ComponentType::CONTROLLER);
			if (controllerPtr != nullptr)
				controllerPtr->setTransform(transformPtr);
		}
		comp = sp_Owner->GetComponentHandle(ComponentType::TRIGGER);
		if (!comp.isNull()) //If the GameObject has a controller, update it's transform.
		{
			auto triggerPtr = sp_Screen->getComponentStore()->getComponent<CollisionTrigger>(comp, ComponentType::TRIGGER);
			if (triggerPtr != nullptr)
				triggerPtr->setTransform(*transformPtr);
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

void LogicComponent::updateScore(int incValue, string idToUpdate)
{
	auto sp_Screen = screen.lock();
	if (sp_Screen != nullptr)
	{
		sp_Screen->updateScore(incValue, idToUpdate);
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
		return false;
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
		return vec3();
	} 
	else
	{
		return vec3();
	}
}

glm::vec3 LogicComponent::vec3Addition(glm::vec3 v1, glm::vec3 v2)
{
	return v1 + v2;
}

glm::vec3 LogicComponent::vec3Subtract(glm::vec3 v1, glm::vec3 v2)
{
	return v1 - v2;
}

glm::vec3 LogicComponent::vec3Normalise(glm::vec3 v1)
{
	return glm::normalize(v1);
}

float LogicComponent::getDistance(glm::vec3 v1, glm::vec3 v2)
{
	
	return glm::distance(v1, v2);
}

glm::quat LogicComponent::angleAxis(float angle, glm::vec3 axis)
{
	return glm::angleAxis(glm::radians(angle), axis);
}

glm::quat LogicComponent::getOrientation()
{
	std::shared_ptr<GameScreen> sp_Screen = screen.lock(); //Access GameScreen
	auto sp_Owner = owner.lock(); //Access GameObject that owns this component
	if (sp_Owner != nullptr && sp_Screen != nullptr)  //If it still exists 
	{
		Handle comp = sp_Owner->GetComponentHandle(ComponentType::TRANSFORM);
		if (!comp.isNull())  //If the GameObject has a transform component, update it's orientation.
		{
			auto transform = sp_Screen->getComponentStore()->getComponent<Transform>(comp, ComponentType::TRANSFORM);
			return transform->orientation;
		}
		return glm::quat();
	}
	else
	{
		return glm::quat();
	}
}

glm::quat LogicComponent::quatMultiply(glm::quat q1, glm::quat q2)
{
	return q1 * q2;
}



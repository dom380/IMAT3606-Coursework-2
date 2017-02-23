#include "Components\LogicComponent.h"

LogicComponent::LogicComponent(std::weak_ptr<GameObject> owner, std::weak_ptr<GameScreen> screen, const char* scriptFile) : Component(ComponentType::LOGIC)
{
	this->owner = owner;
	this->screen = screen;
	script = scriptFile;
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
	if (!luaL_loadfile(luaState, script) == LUA_OK)
	{
		std::string errorString = "Failed to load script file ";
		errorString += script;
		throw std::runtime_error(errorString);
	}
	luaL_openlibs(luaState);
	luabridge::getGlobalNamespace(luaState)
		.beginNamespace("engine")
			.beginClass<glm::vec3>("vec3")
				.addConstructor<void(*) (void)>()
				.addData("x", &glm::vec3::x)
				.addData("y", &glm::vec3::y)
				.addData("z", &glm::vec3::z)
			.endClass()
			.beginClass<glm::quat>("quat")
				.addConstructor<void(*) (void)>()
				.addData("w", &glm::quat::w)
				.addData("x", &glm::quat::x)
				.addData("y", &glm::quat::y)
				.addData("z", &glm::quat::z)
			.endClass()
			.beginClass<LogicComponent>("LogicComponent")
				.addFunction("applyTransform", &LogicComponent::applyTransform)
				.addFunction("getPosition", &LogicComponent::getPosition)
				.addFunction("toggleRender", &LogicComponent::toggleRender)
				.addFunction("isRendering", &LogicComponent::isRendering)
				.addFunction("updateScore", &LogicComponent::updateScore)
			.endClass()
			.beginClass<MsgType>("MsgType")
			.endClass()
			.beginClass<Message>("Message")
				.addData("id", &Message::id)
			.endClass()
			.deriveClass<LocationMessage, Message> ("LocationMessage")
				.addData("location", &LocationMessage::location)
			.endClass()
		.endNamespace();
	if (luaL_dofile(luaState, script) != 0) {
		//todo error handling
		std::string errorString = "Failed to read script file ";
		errorString += script;
		throw std::runtime_error(errorString);
	}
	updateFunc = luabridge::getGlobal(luaState, "update");
	recieveMsgFunc = luabridge::getGlobal(luaState, "RecieveMessage");
}

void LogicComponent::applyTransform(glm::vec3 position, float scale, float orientation)
{
	std::shared_ptr<GameScreen> sp_Screen = screen.lock(); //Access GameScreen
	auto sp_Owner = owner.lock(); //Access GameObject that owns this component
	if (sp_Owner != nullptr && sp_Screen != nullptr)  //If it still exists 
	{
		Handle comp = sp_Owner->GetComponentHandle (ComponentType::TRANSFORM);
		if (!comp.isNull()) //If the GameObject has a transform component, update it's orientation.
		{ 
			auto transformPtr = sp_Screen->getComponentStore()->getComponent<Transform>(comp, ComponentType::TRANSFORM);
			transformPtr->position += position;
			transformPtr->scale *= scale;
			transformPtr->orientation.w += orientation;
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



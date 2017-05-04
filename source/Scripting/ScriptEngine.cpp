#include <Scripting\ScriptEngine.h>
#include <Components\LogicComponent.h>
#include <Components\CollisionMessage.h>
#include <Engine.h>
#include <Graphics\Material.h>
#include <utils\UUID\guid.h>

bool ScriptEngine::initialised = false;
shared_ptr<ScriptEngine> ScriptEngine::instance;

shared_ptr<ScriptEngine> ScriptEngine::getInstance()
{
	if (!initialised)
	{
		instance = shared_ptr<ScriptEngine>(new ScriptEngine());
		initialised = true;
	}
	return instance;
}

bool ScriptEngine::loadScript(string filePath, string scriptName)
{
	auto scriptIterator = loadedScripts.find(scriptName);
	if (scriptIterator != loadedScripts.end())
	{
		return true;
	}
	if (luaL_loadfile(luaState, filePath.c_str()) != LUA_OK)
	{
		std::string errorString = lua_tostring(luaState, -1);
		std::string errorMsg = "Failed to load script file ";
		errorMsg += filePath;
		errorMsg += " - " + errorString;
		std::cout << errorMsg << std::endl;
		return false;
	}
	if (luaL_dofile(luaState, filePath.c_str()) != LUA_OK) 
	{
		//todo error handling
		std::string errorString = lua_tostring(luaState, -1);
		std::string errorMsg = "Failed to read script file ";
		errorMsg += filePath;
		errorMsg += " - " + errorString;
		std::cout << errorMsg << std::endl;
		return false;
	}
	else 
	{
		loadedScripts.emplace(scriptName);
		return true;
	}
}

luabridge::LuaRef ScriptEngine::getFunction(string scriptName, string functionName)
{
	auto scriptFunName = std::pair<string, string>(scriptName, functionName);
	auto funcIterator = luaFunctions.find(scriptFunName);
	if (funcIterator != luaFunctions.end())
	{
		return funcIterator->second;
	}
	auto scriptIterator = loadedScripts.find(scriptName);
	if (scriptIterator != loadedScripts.end()) 
	{
		auto obj = luabridge::getGlobal(luaState, scriptName.c_str());
		
		if (!obj.isNil()) 
		{
			auto luaFun = obj[functionName];
			if (luaFun.isFunction()) 
			{
				luaFunctions.emplace(scriptFunName, luaFun);
				return luaFun;
			}
		}
	}
	else 
	{
		std::cout << "The requested function either doesn't exists or the script file hasn't been loaded." << std::endl;
	}
	return  luabridge::LuaRef(luaState);
}

void ScriptEngine::close()
{
	loadedScripts.clear();
	luaFunctions.clear();
	LuaStateHolder::destroyState();
}

ScriptEngine::ScriptEngine()
{
	luaState = LuaStateHolder::getLuaState();
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
			.beginClass<Guid>("Guid")
				.addFunction("__eq", &Guid::operator==)
			.endClass()
			.beginClass<Material>("Material")
				.addConstructor<void(*) (void)>()
				.addData("Kd", &Material::Kd)
				.addData("Ka", &Material::Ka)
			.endClass()
			.beginClass<Component>("Component")
				.addFunction("recieveMessage", &Component::RecieveMessage)
			.endClass()
			.deriveClass<LogicComponent, Component>("LogicComponent")
				.addFunction("applyTransform", &LogicComponent::applyTransform)
				.addFunction("getPosition", &LogicComponent::getPosition)
				.addFunction("toggleRender", &LogicComponent::toggleRender)
				.addFunction("isRendering", &LogicComponent::isRendering)
				.addFunction("updateScore", &LogicComponent::updateScore)
				.addFunction("resetTransform", &LogicComponent::resetTransform)
				.addFunction("getDistance", &LogicComponent::getDistance)
				.addFunction("vec3Addition", &LogicComponent::vec3Addition)
				.addFunction("vec3Subtract", &LogicComponent::vec3Subtract)
				.addFunction("vec3Normalise", &LogicComponent::vec3Normalise)
				.addFunction("angleAxis", &LogicComponent::angleAxis)
				.addFunction("getOrientation", &LogicComponent::getOrientation)
				.addFunction("quatMultiply", &LogicComponent::quatMultiply)
			.endClass()
			.deriveClass<ModelComponent, Component>("ModelComponent")
				.addFunction("getId", &ModelComponent::getId)
			.endClass()
			.deriveClass<PhysicsComponent, Component>("PhysicsComponent")
			.endClass()
			.beginClass<MsgType>("MsgType")
			.endClass()
			.beginClass<Message>("Message")
				.addConstructor<void(*) (void)>()
				.addData("id", &Message::id)
			.endClass()
			.deriveClass<LocationMessage, Message>("LocationMessage")
				.addData("location", &LocationMessage::location)
			.endClass()
			.deriveClass<MaterialMessage, Message>("MaterialMessage")
				.addConstructor<void(*) (Material)>()
				.addData("material", &MaterialMessage::material)
			.endClass()
			.deriveClass<CollisionMessage, Message>("CollisionMessage")
				.addData("other", &CollisionMessage::other)
				.addData("dt", &CollisionMessage::dt)
			.endClass()
			.beginClass<Engine>("engineObj")
				.addFunction("switchScreen", &Engine::switchScreen)
				.addFunction("replaceScreen", &Engine::replaceScreen)
				.addFunction("exit", &Engine::exit)
			.endClass()
			.beginClass<GameObject>("GameObject")
				.addFunction("getModel", &GameObject::getModel)
				.addFunction("getTransform", &GameObject::getTransform)
				.addFunction("getLogic", &GameObject::getLogic)
				.addFunction("getPhysics", &GameObject::getPhysics)
				.addFunction("getTrigger", &GameObject::getTrigger)
				.addFunction("getId", &GameObject::getId)
				.addFunction("getTag", &GameObject::getTagString)
			.endClass()
			.beginClass<Transform>("Transform")
				.addData("position", &Transform::position)
				.addData("scale", &Transform::scale)
				.addData("orientation", &Transform::orientation)
				.addFunction("getOriginalPos", &Transform::getOriginalPos)
			.endClass()
		.endNamespace();
}

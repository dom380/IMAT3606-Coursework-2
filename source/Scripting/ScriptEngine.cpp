#include <Scripting\ScriptEngine.h>
#include <Components\LogicComponent.h>

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
		return false;
	}
	if (luaL_dofile(luaState, filePath.c_str()) != LUA_OK) 
	{
		//todo error handling
		std::string errorString = lua_tostring(luaState, -1);
		std::string errorMsg = "Failed to read script file ";
		errorMsg += filePath;
		errorMsg += " - " + errorString;
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
	LuaStateHolder::destroyState();
	loadedScripts.clear();
	luaFunctions.clear();
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
			.deriveClass<LocationMessage, Message>("LocationMessage")
				.addData("location", &LocationMessage::location)
			.endClass()
		.endNamespace();
}

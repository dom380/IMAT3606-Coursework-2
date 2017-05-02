#pragma once
#ifndef LEVELLOADER_H
#define LEVELLOADER_H
#include <gl/glm/glm/gtc/quaternion.hpp>
#include <gl/glm/glm/gtx/quaternion.hpp>
#include <Engine.h>
#include <Renderers\Graphics.h>
#include <Screens\MenuScreen.h>
#include <Screens\GameScreen.h>
#include <GameObject.h>
#include <Components\LogicComponent.h>
#include <Components\ControllerComponent.h>
#include "tinyxml2.h"
#include <Scripting\ScriptEngine.h>
#include <Components\AnimatedModelComponent.h>
#include <Utils/Utilities.h>
#include "utils\DebugUtils.h"
#include <GUI\UITextureElement.h>
#include <GUI\UIType.h>
#include <Camera\OrthographicCamera.h>
#include <Camera\PerspectiveCamera.h>
#include <Camera\EngineCamera.h>
#include <Camera\FollowCamera.h>
#include <gl/glm/glm/gtc/quaternion.hpp>
#include <gl/glm/glm/gtx/quaternion.hpp>

#ifndef NDEBUG
#include "Timer.h"
using namespace std;
#endif
/*
Factory class to parse level XML descriptors and create the relevant objects.
Calling these methods on a separate thread using OpenGL implementation requires a second
context and all Vertex Array Objects to be rebuild on the rendering thread.
*/
class LevelLoader {
public:
	/*
	Read the specified XML file then load the requested objects and register the screen with the engine.

	Returns false if an error occured.
	*/
	static bool loadLevel(Engine* engine, shared_ptr<Graphics> renderer, shared_ptr<Input> input, const char* filePath)
	{
		try
		{
			tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
			tinyxml2::XMLError check = doc->LoadFile(filePath);
			if (check != tinyxml2::XML_SUCCESS) {
				std::cerr << "Failed to load file" << filePath << std::endl;
				return false;
			}
			tinyxml2::XMLElement* screenElement = doc->FirstChildElement("screen");
			/*
				Safety check, filename must be the same as the name attribute in file!
			*/
			string fileName = filePath;
			std::size_t pos = fileName.find_last_of("/");
			fileName = fileName.substr(pos+1, fileName.length());
			fileName = fileName.substr(0, fileName.length()-4);
			if (strcmp(fileName.c_str(), screenElement->Attribute("name")) != 0)
			{
				std::cerr << "Failed to load file, filename != to levelname" << filePath << std::endl;

				DebugUtils::getInstance()->popup("FAIL","Failed to load file, filename != to levelname");

				return false;
			}
			engine->getDebugMenu()->refreshMenuItems();
			const char* type = screenElement->Attribute("type");
			if (string(type) == string("menu")) {
				return loadMenu(engine, renderer, input, doc, filePath);
			}
			else if (string(type) == string("level")) {
				return loadGameLevel(engine, renderer, input, doc, filePath);
			}
			else
			{
				std::cerr << "Unrecognised screen type: " << type << std::endl;
				return false;
			}

		}
		catch (const std::runtime_error& re)
		{
			std::cerr << "Runtime error: " << re.what() << std::endl;
			return false;
		}
		catch (const std::exception& ex)
		{
			std::cerr << "Error occurred: " << ex.what() << std::endl;
			return false;
		}
		catch (...)
		{
			std::cerr << "Unknown failure occurred. Possible memory corruption" << std::endl;
			return false;
		}
	}
	static void duplicateGameObject(shared_ptr<GameScreen> gameScreen, shared_ptr<GameObject> go)
	{
		shared_ptr<GameObject> newGO = std::make_shared<GameObject>(*go);
		for (int x = ComponentType::MODEL; x < ComponentType::COMPONENT_TYPE_COUNT; x++)
		{
			ComponentType cType = static_cast<ComponentType>(x);
			if (go->HasComponent(cType))
			{
				switch (cType)
				{
				case MODEL:
				{
					auto model = gameScreen->getComponentStore()->getComponent<ModelComponent>(go->GetComponentHandle(ComponentType::MODEL), ComponentType::MODEL);
					shared_ptr<ModelComponent> mc = std::make_shared<ModelComponent>(*model);
					newGO->AddComponent(mc, ComponentType::MODEL);
					break;
				}
				case ANIMATION:
					break;
				case RIGID_BODY:
				{
					auto rigid = gameScreen->getComponentStore()->getComponent<PhysicsComponent>(go->GetComponentHandle(ComponentType::RIGID_BODY), ComponentType::RIGID_BODY);
					shared_ptr<PhysicsComponent> pc = std::make_shared<PhysicsComponent>(*rigid);
					newGO->AddComponent(pc, ComponentType::RIGID_BODY);
					break;
				}
				case LOGIC:
					break;
				case TRANSFORM:
				{
					auto transform = gameScreen->getComponentStore()->getComponent<Transform>(go->GetComponentHandle(ComponentType::TRANSFORM), ComponentType::TRANSFORM);
					shared_ptr<Transform> tr = std::make_shared<Transform>(*transform);
					newGO->AddComponent(tr, ComponentType::TRANSFORM);
					auto model = gameScreen->getComponentStore()->getComponent<ModelComponent>(newGO->GetComponentHandle(ComponentType::MODEL), ComponentType::MODEL);
					model->transformHandle = newGO->GetComponentHandle(ComponentType::TRANSFORM);//transform;
					break;
				}
				case TRIGGER:
					break;
				case CONTROLLER:
					break;
				case COMPONENT_TYPE_COUNT:
					break;
				default:
					break;
				}

			}
			//this->componentHandles[x] = other.componentHandles[x];
		}
		gameScreen->addGameObject(newGO);

	}
	static void loadGameObject(shared_ptr<Graphics>& renderer, shared_ptr<Physics>& physics, shared_ptr<GameScreen> gameSceen, tinyxml2::XMLElement* gameObjElement, shared_ptr<ComponentStore> componentStore)
	{
		if (gameObjElement->FirstChildElement("components") == NULL)
			return;
		EnumParser<GameObjectTag> tagParser;
		std::string tagText = gameObjElement->FirstChildElement("tag") != NULL ? gameObjElement->FirstChildElement("tag")->GetText() : "UNKNOWN";
		GameObjectTag tag = tagParser.parse(tagText);
		shared_ptr<GameObject> gameObject = std::make_shared<GameObject>(componentStore, tag);
		tinyxml2::XMLElement* componentElement = gameObjElement->FirstChildElement("components")->FirstChildElement();
		while (componentElement != NULL)
		{
			if (componentElement->Attribute("type") == NULL)
			{
				std::cerr << "Unknown component type, skipping" << std::endl;
				continue;
			}
			ComponentType type = componentEnumParser.parse(string(componentElement->Attribute("type")));
			switch (type)
			{
			case ComponentType::MODEL:
				loadModel(renderer, gameObject, componentElement);
				break;
			case ComponentType::ANIMATION:
				loadAnimation(renderer, gameObject, componentElement);
				break;
			case ComponentType::RIGID_BODY:
				loadPhysics(renderer, physics, gameObject, componentElement);
				break;
			case ComponentType::LOGIC:
			{
				auto scriptElement = componentElement->FirstChildElement("script");
				const char* scriptName = scriptElement != NULL ? scriptElement->GetText() : "default.lua";
				shared_ptr<LogicComponent> logicComp = std::make_shared<LogicComponent>(gameObject, gameSceen, AssetManager::getInstance()->getScript(scriptName));
				gameObject->AddComponent(logicComp, ComponentType::LOGIC);
			}
			break;
			case ComponentType::TRANSFORM:
			{
				shared_ptr<Transform> transform = std::make_shared<Transform>();
				loadTransform(transform, componentElement);
				gameObject->AddComponent(transform, ComponentType::TRANSFORM);
			}
			break;
			case ComponentType::TRIGGER:
			{
				loadCollisionTrigger(physics, gameObject, componentElement);
			}
			break;
			case ComponentType::CONTROLLER:
			{
				loadController(physics, gameObject, componentElement);
			}
			break;
			default:
				break;
			}
			if (gameObject->HasComponent(ComponentType::TRANSFORM) && gameObject->HasComponent(ComponentType::MODEL)) //Ensure the model is using the same transform as the object
			{
				//auto transform = componentStore->getComponent<Transform>(gameObject->GetComponent(ComponentType::TRANSFORM), ComponentType::TRANSFORM);
				auto model = componentStore->getComponent<ModelComponent>(gameObject->GetComponentHandle(ComponentType::MODEL), ComponentType::MODEL);
				model->transformHandle = gameObject->GetComponentHandle(ComponentType::TRANSFORM);//transform;
			}
			componentElement = componentElement->NextSiblingElement();
		}
		gameSceen->addGameObject(gameObject);

	}
	static void loadGameObject(shared_ptr<Graphics>& renderer, shared_ptr<GameScreen> gameScreen, std::pair<string,string>objectInfo, shared_ptr<Transform> transform)
	{

		shared_ptr<ComponentStore> componentStore = gameScreen->getComponentStore();
		shared_ptr<GameObject> gameObject = std::make_shared<GameObject>(componentStore);
		loadModel(renderer, gameObject, objectInfo.first, objectInfo.second);
		gameObject->AddComponent(transform, ComponentType::TRANSFORM);

		if (gameObject->HasComponent(ComponentType::TRANSFORM) && gameObject->HasComponent(ComponentType::MODEL)) //Ensure the model is using the same transform as the object
		{
			//auto transform = componentStore->getComponent<Transform>(gameObject->GetComponent(ComponentType::TRANSFORM), ComponentType::TRANSFORM);
			auto model = componentStore->getComponent<ModelComponent>(gameObject->GetComponentHandle(ComponentType::MODEL), ComponentType::MODEL);
			model->transformHandle = gameObject->GetComponentHandle(ComponentType::TRANSFORM);//transform;
		}
		gameScreen->addGameObject(gameObject);
	}
	static void loadButtonFunc(shared_ptr<Button> button)
	{
		string scriptName = button->getScript();
		string scriptFile = AssetManager::getInstance()->getScript(scriptName.c_str());
		string function = button->getFunc();
		//Remove file extension then load the script into lua
		string rawname = Utilities::removeExtension(scriptName);
		ScriptEngine::getInstance()->loadScript(scriptFile, rawname);
		//Retrieve a reference to the onClick function
		auto callback = ScriptEngine::getInstance()->getFunction(rawname, function);
		//Build up a table of the provided parameters for the lua script
		luabridge::LuaRef paramTable = luabridge::newTable(LuaStateHolder::getLuaState());
		//tinyxml2::XMLElement* paramElement = onClickElement->FirstChildElement("params") != NULL ? onClickElement->FirstChildElement("params")->FirstChildElement() : NULL;

		for (int x = 0; x < button->getParams().size(); x++)
		{
			string paramName = button->getParams().at(x).first;
			transform(paramName.begin(), paramName.end(), paramName.begin(), tolower);
			paramTable[paramName] = button->getParams().at(x).second;
		}
		//Bind the lua callback function, engine and parameter table as a c++ function for the button.
		button->addOnClickFn(std::bind(callback, Engine::g_pEngine.get(), paramTable));
	}
	/*
	Utility method to load Button elements
	*/
	static void loadButtonElement(shared_ptr<UIElement> uiE, tinyxml2::XMLElement* buttonElement)
	{
		shared_ptr<Button> button;
		switch (uiE->getType())
		{
		case UIType::TEXT:
		{
			shared_ptr<TextBox> tb = dynamic_pointer_cast<TextBox>(uiE);
			button = std::make_shared<Button>(Engine::g_pEngine->getRenderer(), tb->getTransform(), tb->getFont(), tb->getText());
			break;
		}
		case UIType::TEXTURE:
		{
			shared_ptr<UITextureElement> tb = dynamic_pointer_cast<UITextureElement>(uiE);
			button = std::make_shared<Button>(Engine::g_pEngine->getRenderer(), tb->getTransform());
			break;
		}
		default:
			break;
		}
		Engine::g_pEngine->getInput()->registerMouseListener(button);

		loadButtonFunc(buttonElement, button);
		uiE->setButton(button);
	}
	/*
	Utility method to load Button elements
	*/
	static void loadButtonElement(shared_ptr<UIElement> uiE)
	{
		shared_ptr<Button> button;
		switch (uiE->getType())
		{
		case UIType::TEXT:
		{
			shared_ptr<TextBox> tb = dynamic_pointer_cast<TextBox>(uiE);
			button = std::make_shared<Button>(Engine::g_pEngine->getRenderer(), tb->getTransform(), tb->getFont(), tb->getText());
			break;
		}
		case UIType::TEXTURE:
		{
			shared_ptr<UITextureElement> tb = dynamic_pointer_cast<UITextureElement>(uiE);
			button = std::make_shared<Button>(Engine::g_pEngine->getRenderer(), tb->getTransform());
			break;
		}
		default:
			break;
		}
		Engine::g_pEngine->getInput()->registerMouseListener(button);

		loadButtonFunc(button);
		uiE->setButton(button);
	}
private:
	/*
	Utility method to load MenuScreens
	Returns true on success.
	*/
	static bool loadMenu(Engine* engine, shared_ptr<Graphics>& renderer, shared_ptr<Input>& input, tinyxml2::XMLDocument* screenDocument, string filepath)
	{
		tinyxml2::XMLElement* screenElement = screenDocument->FirstChildElement("screen");
		shared_ptr<MenuScreen> menuScreen = std::make_shared<MenuScreen>(renderer);
		menuScreen->setID(screenElement->Attribute("name"));
		menuScreen->setXMLDocument(screenDocument);
		menuScreen->setXMLFilePath(filepath);
		loadUIElements(renderer, menuScreen, screenDocument, filepath);
		engine->registerScreen(menuScreen);
		return true;
	}

	/*
	Utility method to load Text elements
	*/
	static void loadStringElement(shared_ptr<Graphics>& renderer, shared_ptr<Screen> screen, shared_ptr<Transform> transform, string id, tinyxml2::XMLElement* stringElement)
	{
		Font font = *AssetManager::getInstance()->getFont("arial.ttf", renderer);
		const char* text = stringElement->FirstChildElement("value") != NULL ? stringElement->FirstChildElement("value")->GetText() : "MISSING_STRING";
		if (!transform)
		{
			loadTransform(transform, stringElement);
		}
		glm::vec3 colour;
		stringElement->FirstChildElement("colour") != NULL ? loadColour(colour, stringElement->FirstChildElement("colour"), glm::vec3(1.0, 1.0, 1.0)) : colour = glm::vec3(1.0, 1.0, 1.0);
		shared_ptr<TextBox> textBox = std::make_shared<TextBox>(text, font, transform, renderer, colour, id);
		//HasButton?
		tinyxml2::XMLElement* UIButtonElement = stringElement->FirstChildElement("onClick");
		if (UIButtonElement)
		{
			loadButtonElement(textBox, UIButtonElement);
		}
		screen->addUIElement(textBox);
	}

	

	/*
		Utility method to bind the supplied parameters to the requested callback function.
	*/
	static void loadButtonFunc(tinyxml2::XMLElement* onClickElement, shared_ptr<Button> button)
	{
		string scriptName = onClickElement->FirstChildElement("script") != NULL ? onClickElement->FirstChildElement("script")->GetText() : "default.lua"; 
		button->setScript(scriptName);
		string scriptFile = AssetManager::getInstance()->getScript(scriptName.c_str());
		string function = onClickElement->FirstChildElement("function") != NULL ? onClickElement->FirstChildElement("function")->GetText() : "doNothing";
		button->setFunc(function);
		//Remove file extension then load the script into lua
		string rawname = Utilities::removeExtension(scriptName);
		ScriptEngine::getInstance()->loadScript(scriptFile, rawname);
		//Retrieve a reference to the onClick function
		auto callback = ScriptEngine::getInstance()->getFunction(rawname, function);
		//Build up a table of the provided parameters for the lua script
		luabridge::LuaRef paramTable = luabridge::newTable(LuaStateHolder::getLuaState());
		tinyxml2::XMLElement* paramElement = onClickElement->FirstChildElement("params") != NULL ? onClickElement->FirstChildElement("params")->FirstChildElement():NULL;
		while (paramElement != NULL) {
			string paramName = paramElement->Attribute("name");
			transform(paramName.begin(), paramName.end(), paramName.begin(), tolower);
			paramTable[paramName] = paramElement->GetText();
			button->setParam(pair<string, string>(paramName, paramElement->GetText()));
			paramElement = paramElement->NextSiblingElement();
		}
		//Bind the lua callback function, engine and parameter table as a c++ function for the button.
		button->addOnClickFn(std::bind(callback, Engine::g_pEngine.get(), paramTable));
	}

	/*
		Load UI elements

	*/
	static bool loadUIElements(shared_ptr<Graphics>& renderer, shared_ptr<Screen> screen, tinyxml2::XMLDocument* screenDocument, string filepath)
	{
		bool loadSuccess = true;

		tinyxml2::XMLElement* screenElement = screenDocument->FirstChildElement("screen");
		tinyxml2::XMLElement* UIDocElement = screenElement->FirstChildElement("uiElements");
		
		if (UIDocElement)
		{
			UIDocElement = UIDocElement->FirstChildElement();
		}
		
		while (UIDocElement != NULL) {
			shared_ptr<Transform> transform = std::make_shared<Transform>();
			loadTransform(transform, UIDocElement);
			//get ID
			string ID = "";
			tinyxml2::XMLElement* UIID = UIDocElement->FirstChildElement("ID");
			if (UIID)
			{
				UIID->GetText() ? ID = string(UIID->GetText()) : ID = "";
			}
				
			tinyxml2::XMLElement* UITexture;

			
			//get type
			tinyxml2::XMLElement* UITypeElement = UIDocElement->FirstChildElement("Type");
			EnumParser<UIType> uiTypeParser;
			UIType type = uiTypeParser.parse(string(UITypeElement->GetText()));

			switch (type)
			{
			case TEXT:
			{
				loadStringElement(renderer, screen, transform, ID, UIDocElement);
				
				break;
			}
			case TEXTURE:
				//load UI using texture
				UITexture = UIDocElement->FirstChildElement("Texture");
				if (UITexture)
				{
					shared_ptr<UITextureElement> uiT = std::make_shared<UITextureElement>(renderer, transform, UIID->GetText(), UITexture->GetText());
					//HasButton?
					tinyxml2::XMLElement* UIButtonElement = UIDocElement->FirstChildElement("onClick");
					if (UIButtonElement)
					{
						loadButtonElement(uiT, UIButtonElement);
					}
					screen->addUIElement(uiT);
				}
				break;
			default:
				break;
			}
			
			UIDocElement = UIDocElement->NextSiblingElement();
		}

		return loadSuccess;
	}
	/*
		Utility method to load GameScreens
		Returns true on sucess.
	*/
	static bool loadGameLevel(Engine* engine, shared_ptr<Graphics>& renderer, shared_ptr<Input>& input, tinyxml2::XMLDocument* screenDocument, string filepath)
	{
#ifndef NDEBUG
		Timer timer;
		timer.start();
#endif
		tinyxml2::XMLElement* screenElement = screenDocument->FirstChildElement("screen");
		std::vector<shared_ptr<Camera>> cameras;
		shared_ptr<Camera> camera = std::make_shared<PerspectiveCamera>(engine->getWindowWidth(), engine->getWindowHeight(), 45.f);
		tinyxml2::XMLElement* cameraElement = screenElement->FirstChildElement("cameras")->FirstChildElement();
		while (cameraElement != NULL)
		{
			cameras.push_back(loadCamera(renderer, input, cameraElement));
			cameraElement = cameraElement->NextSiblingElement();
		}
		if (cameras.empty()) cameras.push_back(camera);
		shared_ptr<GameScreen> gameScreen = std::make_shared<GameScreen>(renderer, input, engine->getPhysics(), cameras);
		input->setKeyFocus(gameScreen);
		gameScreen->setID(screenElement->Attribute("name"));
		gameScreen->setXMLDocument(screenDocument);
		gameScreen->setXMLFilePath(filepath);
		tinyxml2::XMLElement* gameObjElement = screenElement->FirstChildElement("gameObjects")->FirstChildElement();
		while (gameObjElement != NULL) 
		{
			loadGameObject(renderer, engine->getPhysics(), gameScreen, gameObjElement, gameScreen->getComponentStore());
			gameObjElement = gameObjElement->NextSiblingElement();
		}
		tinyxml2::XMLElement* lightElement = screenElement->FirstChildElement("lights")->FirstChildElement();
		int lightCount = 0;
		while (lightElement != NULL && lightCount <10) //TODO define global light max
		{
			loadLight(gameScreen, lightElement);
			lightElement = lightElement->NextSiblingElement();
			lightCount++;
		}

		loadUIElements(renderer, gameScreen, screenDocument, filepath);

		gameScreen->updateLighting();
		engine->registerScreen(gameScreen);
		input->registerKeyListener(gameScreen);
#ifndef NDEBUG
		double time = timer.getElapsedTime();
#endif
		return true;
	}



	/*
	Utility method to load a Model from obj name

	Currently only supports Wavefront .obj format.
	*/
	static void loadModel(shared_ptr<Graphics>& renderer, shared_ptr<GameObject> gameObject, string objName, string textureName="")
	{
		shared_ptr<ModelComponent> mesh = std::make_shared<ModelComponent>(renderer, gameObject);
		const char* modelPath = objName.c_str();
		const char* texturePath = textureName.empty() ? NULL : textureName.c_str();
		string id = objName;
		//modelElement->Attribute("id") != NULL ? id = modelElement->Attribute("id") : id = "";
		mesh->init(modelPath, texturePath, id);
		//loadTransform(mesh->transform, modelElement);
		gameObject->AddComponent(mesh, ComponentType::MODEL);
	}

	/*
	Utility method to load a Model.

	Currently only supports Wavefront .obj format.
	*/
	static void loadModel(shared_ptr<Graphics>& renderer, shared_ptr<GameObject> gameObject, tinyxml2::XMLElement* modelElement)
	{
		shared_ptr<ModelComponent> mesh = std::make_shared<ModelComponent>(renderer, gameObject);
		const char* modelPath = modelElement->FirstChildElement("file")->GetText();
		const char* texturePath = modelElement->FirstChildElement("texture") != NULL ? modelElement->FirstChildElement("texture")->GetText() : NULL;
		const char* active = modelElement->FirstChildElement("active") != NULL ? modelElement->FirstChildElement("active")->GetText() : NULL;
		if (active)
		{ 
			if (strcmp(active, "false") == 0)
			{
				mesh->toggleDrawing();
			}
		}
		
		string id;
		modelElement->Attribute("id") != NULL ? id = modelElement->Attribute("id") : id = modelPath;
		mesh->init(modelPath, texturePath, id);
		//loadTransform(mesh->transform, modelElement);
		gameObject->AddComponent(mesh, ComponentType::MODEL);
	}

	static void loadAnimation(shared_ptr<Graphics>& renderer, shared_ptr<GameObject> gameObject, tinyxml2::XMLElement* animElement)
	{
		shared_ptr<AnimatedModelComponent> animation = std::make_shared<AnimatedModelComponent>(renderer, gameObject);
		tinyxml2::XMLElement* fileElement = animElement->FirstChildElement("files");
		std::vector<std::pair<const char*, const char*>> files;
		if (fileElement != NULL) fileElement = fileElement->FirstChildElement();
		while (fileElement != NULL) {
			files.push_back(std::make_pair(fileElement->FirstChildElement("id")->GetText(),fileElement->FirstChildElement("filePath")->GetText()));
			fileElement = fileElement->NextSiblingElement();
		}
		animation->init(animElement->FirstChildElement("default")->GetText(),files, "");
		gameObject->AddComponent(animation, ComponentType::ANIMATION);
	}

	/*
		Utility method to load physics component.
	*/
	static void loadPhysics(shared_ptr<Graphics>& graphics, shared_ptr<Physics>& physics, shared_ptr<GameObject> gameObject, tinyxml2::XMLElement* physicsElement)
	{
		float mass = 0.f;
		shared_ptr<PhysicsComponent> physComp;
		//Get mass
		mass = physicsElement->FirstChildElement("mass") != NULL ? physicsElement->FirstChildElement("mass")->FloatText() : 0.f;
		//Check if convex
		bool convex = physicsElement->FirstChildElement("convex") != NULL ? physicsElement->FirstChildElement("convex")->BoolText() : true;
		//Load collision shape
		if (physicsElement->FirstChildElement("collision_mesh") != NULL)
		{
			const char* meshFile = physicsElement->FirstChildElement("collision_mesh")->GetText();
			if (convex)
			{
				auto mesh = AssetManager::getInstance()->getModelData(meshFile, graphics);
				physComp = std::make_shared<PhysicsComponent>(physics, std::weak_ptr<GameObject>(gameObject), mesh, mass, convex);
			}
			else
			{
				auto mesh = AssetManager::getInstance()->getCollisionData(meshFile);
				physComp = std::make_shared<PhysicsComponent>(physics, std::weak_ptr<GameObject>(gameObject), mesh, mass, convex);
			}
		}
		else
		{
			ShapeData shapeData;
			readShapeData(physicsElement, shapeData);
			physComp = std::make_shared<PhysicsComponent>(physics, std::weak_ptr<GameObject>(gameObject), shapeData, mass);
		}
		//Get restitution
		double restitution = physicsElement->FirstChildElement("restitution") != NULL ? physicsElement->FirstChildElement("restitution")->DoubleText() : 0.0f;
		physComp->setRestitution(restitution);
		//Get friction
		double friction = physicsElement->FirstChildElement("friction") != NULL ? physicsElement->FirstChildElement("friction")->DoubleText() : 0.5;
		physComp->setFriction(friction);
		//Get rotational friction. 
		friction = physicsElement->FirstChildElement("rotational_friction") != NULL ? physicsElement->FirstChildElement("rotational_friction")->DoubleText() : 0.0;
		physComp->setRotationalFriction(friction);
		//Get constant velocity
		bool constVel = physicsElement->FirstChildElement("constant_velocity") != NULL ? physicsElement->FirstChildElement("constant_velocity")->BoolText() : false;
		physComp->setConstVelocity(constVel);
		//Get Velocity
		tinyxml2::XMLElement* velElement = physicsElement->FirstChildElement("velocity");
		glm::vec3 velocity;
		if (velElement != NULL) 
		{
			velocity = glm::vec3
				(
					velElement->FirstChildElement("x") != NULL ? velElement->FirstChildElement("x")->FloatText() : 0.0f,
					velElement->FirstChildElement("y") != NULL ? velElement->FirstChildElement("y")->FloatText() : 0.0f,
					velElement->FirstChildElement("z") != NULL ? velElement->FirstChildElement("z")->FloatText() : 0.0f
				);
		}
		physComp->setVelocity(velocity.x, velocity.y, velocity.z);
		gameObject->AddComponent(physComp, ComponentType::RIGID_BODY);
	}

	/*
		Utility method to pare bounding shape data from XML
	*/
	static void readShapeData(tinyxml2::XMLElement* physicsElement, ShapeData& shapeData)
	{
		EnumParser<ShapeData::BoundingShape> boundingParser;
		tinyxml2::XMLElement* shapeElement = physicsElement->FirstChildElement("bounding_shape");
		const char* shapeStr = shapeElement != NULL ? shapeElement->Attribute("type") : NULL;
		if (shapeStr == NULL)
		{
			throw std::runtime_error("No collision mesh or bounding shape specified for physics component");
		}
		
		ShapeData::BoundingShape shapeType = boundingParser.parse(string(shapeStr));
		shapeData.boundingShape = shapeType;
		switch (shapeType)
		{
			case ShapeData::BoundingShape::BOX:
			{
				tinyxml2::XMLElement* extentsElemenet = shapeElement->FirstChildElement("half_extents");
				shapeData.halfExtents = glm::vec3
					(
						extentsElemenet->FirstChildElement("x") != NULL ? extentsElemenet->FirstChildElement("x")->FloatText() : 1.0f,
						extentsElemenet->FirstChildElement("y") != NULL ? extentsElemenet->FirstChildElement("y")->FloatText() : 1.0f,
						extentsElemenet->FirstChildElement("z") != NULL ? extentsElemenet->FirstChildElement("z")->FloatText() : 1.0f
						);
				break;
			}
			case ShapeData::BoundingShape::SPHERE:
				shapeData.radius = shapeElement->FirstChildElement("radius") != NULL ? shapeElement->FirstChildElement("radius")->FloatText() : 1.0f;
				break;
			case ShapeData::BoundingShape::CONE:
				shapeData.radius = shapeElement->FirstChildElement("radius") != NULL ? shapeElement->FirstChildElement("radius")->FloatText() : 1.0f;
				shapeData.height = shapeElement->FirstChildElement("height") != NULL ? shapeElement->FirstChildElement("height")->FloatText() : 1.0f;
				break;
			case ShapeData::BoundingShape::CYLINDER:
			{
				tinyxml2::XMLElement* extentsElemenet = shapeElement->FirstChildElement("half_extents");
				shapeData.halfExtents = glm::vec3
					(
						extentsElemenet->FirstChildElement("x") != NULL ? extentsElemenet->FirstChildElement("x")->FloatText() : 1.0f,
						extentsElemenet->FirstChildElement("y") != NULL ? extentsElemenet->FirstChildElement("y")->FloatText() : 1.0f,
						extentsElemenet->FirstChildElement("z") != NULL ? extentsElemenet->FirstChildElement("z")->FloatText() : 1.0f
						);
				break;
			}
			case ShapeData::BoundingShape::CAPSULE:
			{
				shapeData.radius = shapeElement->FirstChildElement("radius") != NULL ? shapeElement->FirstChildElement("radius")->FloatText() : 1.0f;
				shapeData.height = shapeElement->FirstChildElement("height") != NULL ? shapeElement->FirstChildElement("height")->FloatText() : 1.0f;
				break;
			}
		}
	}

	/*
		Utility method to load collision triggers.
	*/
	static void loadCollisionTrigger(shared_ptr<Physics>& physics, shared_ptr<GameObject> gameObject, tinyxml2::XMLElement* triggerElement)
	{
		shared_ptr<CollisionTrigger> collisionTrigger;
		ShapeData shapeData;
		readShapeData(triggerElement, shapeData);
		const char* scriptName = triggerElement->FirstChildElement("script") != NULL ? triggerElement->FirstChildElement("script")->GetText() : "default.lua";
		auto scriptPath = AssetManager::getInstance()->getScript(scriptName);
		bool triggerOnce = triggerElement->FirstChildElement("trigger_once") != NULL ? triggerElement->FirstChildElement("trigger_once")->BoolText() : true;
		collisionTrigger = std::make_shared<CollisionTrigger>(physics, std::weak_ptr<GameObject>(gameObject), shapeData, scriptPath, triggerOnce);
		gameObject->AddComponent(collisionTrigger, ComponentType::TRIGGER);
		std::shared_ptr<BulletPhysics> physicsPtr = std::dynamic_pointer_cast<BulletPhysics>(physics);
		if (physicsPtr != nullptr)
		{
			physicsPtr->addTrigger(collisionTrigger);
		}
	}

	/*
		Utility method to load kinematic controllers.
	*/
	static void loadController(shared_ptr<Physics>& physics, shared_ptr<GameObject> gameObject, tinyxml2::XMLElement* controllerElement)
	{
		ShapeData shapeData;
		readShapeData(controllerElement, shapeData);
		bool flip = controllerElement->FirstChildElement("flip") != NULL ? controllerElement->FirstChildElement("flip")->BoolText() : false;
		float offset = controllerElement->FirstChildElement("offset") != NULL ? controllerElement->FirstChildElement("offset")->FloatText() : 0.0f;
		float jumpRayVal = controllerElement->FirstChildElement("jump_ray") != NULL ? controllerElement->FirstChildElement("jump_ray")->FloatText() : 0.13f;
		shared_ptr<ControllerComponent> controller = std::make_shared<ControllerComponent>(physics, gameObject, shapeData, offset, jumpRayVal, flip);
		float movementSpeed = controllerElement->FirstChildElement("movement_speed") != NULL ? controllerElement->FirstChildElement("movement_speed")->FloatText() : 0.1f;
		float jumpForce = controllerElement->FirstChildElement("jump_force") != NULL ? controllerElement->FirstChildElement("jump_force")->FloatText() : 10.0f;
		controller->setMovementSpeed(movementSpeed);
		controller->setJumpForce(jumpForce);
		gameObject->AddComponent(controller, ComponentType::CONTROLLER);

	}

	/*
	Utility method to load Transform objects
	*/
	static void loadTransform(Transform &transform, tinyxml2::XMLElement* element)
	{
		glm::vec3 pos;
		glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0);
		glm::quat quat; quat.y = 1.0f; quat.w = 0.0f;
		readTransformData(pos, scale, quat, element);
		transform.orientation = quat;
		transform.position = pos;
		transform.scale = scale;
		transform.originalPosition = pos;
		transform.originalOrientation = quat;
	}

	/*
	Utility method to load Transform objects
	*/
	static void loadTransform(std::shared_ptr<Transform> &transform)
	{
		glm::vec3 pos;
		glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0);
		glm::quat quat; quat.y = 1.0f; quat.w = 0.0f;
		transform->orientation = quat;
		transform->position = pos;
		transform->scale = scale;
		transform->originalPosition = pos;
		transform->originalOrientation = quat;
	}

	/*
	Utility method to load Transform objects
	*/
	static void loadTransform(std::shared_ptr<Transform> &transform, tinyxml2::XMLElement* element)
	{
		glm::vec3 pos;
		glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0);
		glm::quat quat;
		readTransformData(pos, scale, quat, element);
		transform->orientation = quat;
		transform->position = pos;
		transform->scale = scale;
		transform->originalPosition = pos;
		transform->originalOrientation = quat;
	}

	static void readTransformData(glm::vec3 &pos, glm::vec3 &scale, glm::quat &quat, tinyxml2::XMLElement* element)
	{
		tinyxml2::XMLElement* posElement = element->FirstChildElement("position");
		if (posElement != NULL) {
			pos = glm::vec3
			(
				posElement->FirstChildElement("x") != NULL ? posElement->FirstChildElement("x")->FloatText() : 0.0f,
				posElement->FirstChildElement("y") != NULL ? posElement->FirstChildElement("y")->FloatText() : 0.0f,
				posElement->FirstChildElement("z") != NULL ? posElement->FirstChildElement("z")->FloatText() : 0.0f
			);
		}
		tinyxml2::XMLElement* scaleElement = element->FirstChildElement("scale");
		if (scaleElement != NULL) {
			scale = glm::vec3
			(
				scaleElement->FirstChildElement("x") != NULL ? scaleElement->FirstChildElement("x")->FloatText(1.0f) : 1.0f,
				scaleElement->FirstChildElement("y") != NULL ? scaleElement->FirstChildElement("y")->FloatText(1.0f) : 1.0f,
				scaleElement->FirstChildElement("z") != NULL ? scaleElement->FirstChildElement("z")->FloatText(1.0f) : 1.0f
			);
		}
		tinyxml2::XMLElement* quatElement = element->FirstChildElement("orientation");
		if (quatElement != NULL) {
			quat = glm::quat
			(
				quatElement->FirstChildElement("w") != NULL ? quatElement->FirstChildElement("w")->FloatText() : 0.0f,
				quatElement->FirstChildElement("x") != NULL ? quatElement->FirstChildElement("x")->FloatText() : 0.0f,
				quatElement->FirstChildElement("y") != NULL ? quatElement->FirstChildElement("y")->FloatText() : 0.0f,
				quatElement->FirstChildElement("z") != NULL ? quatElement->FirstChildElement("z")->FloatText() : 1.0f
			);

			quat = glm::angleAxis(glm::radians(quat.w), glm::vec3(quat.x, quat.y, quat.z));
		}
	}
	/*
		Utility method to load Camera objects.
	*/
	static std::shared_ptr<Camera> loadCamera(shared_ptr<Graphics>& renderer, shared_ptr<Input>& input, tinyxml2::XMLElement* cameraElement)
	{
		string typeStr = cameraElement->Attribute("type") != NULL ? cameraElement->Attribute("type") : "PERSPECTIVE";
		EnumParser<Camera::CameraClass> parser;
		Camera::CameraClass type = parser.parse(typeStr);
		string id = cameraElement->FirstChildElement("id") != NULL ? cameraElement->FirstChildElement("id")->GetText() : string("UNKNOWN");
		int width = cameraElement->FirstChildElement("width") != NULL ? cameraElement->FirstChildElement("width")->IntText() : renderer->getWidth();
		int height = cameraElement->FirstChildElement("height") != NULL ? cameraElement->FirstChildElement("height")->IntText() : renderer->getHeight();
		float aspect = cameraElement->FirstChildElement("aspect") != NULL ? cameraElement->FirstChildElement("aspect")->FloatText() : 45.f;
		bool mouseListener = cameraElement->FirstChildElement("mouse_input") != NULL ? cameraElement->FirstChildElement("mouse_input")->BoolText() : false;
		bool keyListener = cameraElement->FirstChildElement("key_input") != NULL ? cameraElement->FirstChildElement("key_input")->BoolText() : false;
		glm::vec3 pos(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f), direction(0.0f, 0.0f, -1.0f);
		tinyxml2::XMLElement* posElement = cameraElement->FirstChildElement("position");
		shared_ptr<Camera> camera;
		if (posElement != NULL) 
		{
			pos = glm::vec3
				(
					posElement->FirstChildElement("x") != NULL ? posElement->FirstChildElement("x")->FloatText() : 0.0f,
					posElement->FirstChildElement("y") != NULL ? posElement->FirstChildElement("y")->FloatText() : 0.0f,
					posElement->FirstChildElement("z") != NULL ? posElement->FirstChildElement("z")->FloatText() : 0.0f
				);
		}
		tinyxml2::XMLElement* upElement = cameraElement->FirstChildElement("up");
		if (upElement != NULL)
		{
			up = glm::vec3
				(
					upElement->FirstChildElement("x") != NULL ? upElement->FirstChildElement("x")->FloatText() : 0.0f,
					upElement->FirstChildElement("y") != NULL ? upElement->FirstChildElement("y")->FloatText() : 1.0f,
					upElement->FirstChildElement("z") != NULL ? upElement->FirstChildElement("z")->FloatText() : 0.0f
				);
		}
		tinyxml2::XMLElement* directionElement = cameraElement->FirstChildElement("direction");
		if (directionElement != NULL)
		{
			direction = glm::vec3
				(
					directionElement->FirstChildElement("x") != NULL ? directionElement->FirstChildElement("x")->FloatText() : 0.0f,
					directionElement->FirstChildElement("y") != NULL ? directionElement->FirstChildElement("y")->FloatText() : 0.0f,
					directionElement->FirstChildElement("z") != NULL ? directionElement->FirstChildElement("z")->FloatText() : -1.0f
				);
		}
		switch (type)
		{
			case Camera::CameraClass::ORTHOGRAPHIC:
			{
				//todo
				//shared_ptr<OrthographicCamera> cam = std::make_shared<OrthographicCamera>();
				camera = nullptr;
			}
				break;
			case Camera::CameraClass::PERSPECTIVE:
			{
				camera = std::make_shared<PerspectiveCamera>(width, height, aspect, pos, up, direction);
			}
				break;
			case Camera::CameraClass::FOLLOW:
			{
				tinyxml2::XMLElement* targetDistElement = cameraElement->FirstChildElement("follow_distance");
				glm::vec3 targetDist(0.f,-6.f,-15.f);
				if (targetDistElement != NULL)
				{
					targetDist = glm::vec3
						(
							targetDistElement->FirstChildElement("x") != NULL ? targetDistElement->FirstChildElement("x")->FloatText() : 0.0f,
							targetDistElement->FirstChildElement("y") != NULL ? targetDistElement->FirstChildElement("y")->FloatText() : -6.0f,
							targetDistElement->FirstChildElement("z") != NULL ? targetDistElement->FirstChildElement("z")->FloatText() : -15.0f
						);
				}
				camera = std::make_shared<FollowCamera>(width, height, aspect, targetDist, pos, up, direction);
			}
				break;
			case Camera::CameraClass::ENGINE:
			{
				camera = std::make_shared<EngineCamera>(width, height, aspect, pos, up, direction);
			}
				break;
			default:
				camera = std::make_shared<PerspectiveCamera>(width, height, aspect, pos, up, direction);
		}
		camera->setId(id);
		if (mouseListener) input->registerMouseListener(camera);
		if (keyListener) input->registerKeyListener(camera);
		return camera;
	}
	/*
	Utility method to load Light objcts
	*/
	static void loadLight(shared_ptr<GameScreen> gameScreen, tinyxml2::XMLElement* element)
	{
		Light light = Light();
		tinyxml2::XMLElement* posElement = element->FirstChildElement("position");
		if (posElement != NULL)
		{
			light.pos = glm::vec3
			(
				posElement->FirstChildElement("x") != NULL ? posElement->FirstChildElement("x")->FloatText() : 0.0f,
				posElement->FirstChildElement("y") != NULL ? posElement->FirstChildElement("y")->FloatText() : 0.0f,
				posElement->FirstChildElement("z") != NULL ? posElement->FirstChildElement("z")->FloatText() : 0.0f
			);
		}
		tinyxml2::XMLElement* lightElement = element->FirstChildElement("ambient");
		if (lightElement != NULL)
		{
			loadColour(light.ambient, lightElement);
		}
		lightElement = element->FirstChildElement("diffuse");
		if (lightElement != NULL)
		{
			loadColour(light.diffuse, lightElement);
		}
		lightElement = element->FirstChildElement("specular");
		if (lightElement != NULL)
		{
			loadColour(light.specular, lightElement);
		}
		gameScreen->addLight(light);
	}

	/*
	Utility method to load Colour objcts
	*/
	static void loadColour(glm::vec3& colour, tinyxml2::XMLElement* element, glm::vec3 defaultVal = glm::vec3(0.0, 0.0, 0.0))
	{
		colour = glm::vec3
		(
			element->FirstChildElement("r") != NULL ? element->FirstChildElement("r")->FloatText() : defaultVal.r,
			element->FirstChildElement("g") != NULL ? element->FirstChildElement("g")->FloatText() : defaultVal.g,
			element->FirstChildElement("b") != NULL ? element->FirstChildElement("b")->FloatText() : defaultVal.b
		);
	}

	static EnumParser<ComponentType> componentEnumParser;

};

#endif // !LEVELLOADER_H

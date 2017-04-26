#pragma once
#ifndef LEVELLOADER_H
#define LEVELLOADER_H
#include <Engine.h>
#include <Renderers\Graphics.h>
#include <Screens\MenuScreen.h>
#include <Screens\GameScreen.h>
#include <GameObject.h>
#include <Components\LogicComponent.h>
#include <Components\ControllerComponent.h>
#include "tinyxml2.h"
#include <Scripting\ScriptEngine.h>
#include <Utils/Utilities.h>
#include "utils\DebugUtils.h"
#include <GUI\UITextureElement.h>
#include <gl/glm/glm/gtc/quaternion.hpp>
#include <gl/glm/glm/gtx/quaternion.hpp>

#ifndef NDEBUG
#include "Timer.h"

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
				//todo
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
		tinyxml2::XMLElement* stringElement = screenElement->FirstChildElement("strings");
		if (stringElement != NULL) stringElement = stringElement->FirstChildElement();
		while (stringElement != NULL) {
			loadStringElement(renderer, menuScreen, stringElement);
			stringElement = stringElement->NextSiblingElement();
		}
		tinyxml2::XMLElement* buttonElement = screenElement->FirstChildElement("buttons")->FirstChildElement();
		while (buttonElement != NULL) {
			loadButtonElement(engine, renderer, input, menuScreen, buttonElement);
			buttonElement = buttonElement->NextSiblingElement();
		}

		loadUIElements(renderer, menuScreen, screenDocument, filepath);
		engine->registerScreen(menuScreen);
		return true;
	}

	/*
	Utility method to load Text elements
	*/
	static void loadStringElement(shared_ptr<Graphics>& renderer, shared_ptr<Screen> screen, tinyxml2::XMLElement* stringElement)
	{
		Font font = *AssetManager::getInstance()->getFont("arial.ttf", renderer);
		const char* text = stringElement->FirstChildElement("value") != NULL ? stringElement->FirstChildElement("value")->GetText() : "MISSING_STRING";
		shared_ptr<Transform> transform = std::make_shared<Transform>();
		loadTransform(transform, stringElement);
		glm::vec3 colour;
		stringElement->FirstChildElement("colour") != NULL ? loadColour(colour, stringElement->FirstChildElement("colour"), glm::vec3(1.0, 1.0, 1.0)) : colour = glm::vec3(1.0, 1.0, 1.0);
		string id;
		stringElement->Attribute("id") != NULL ? id = stringElement->Attribute("id") : id = "";
		shared_ptr<TextBox> textBox = std::make_shared<TextBox>(text, font, transform, renderer, colour, id);
		screen->addTextBox(textBox);
	}

	/*
	Utility method to load Button elements
	*/
	static void loadButtonElement(Engine* engine, shared_ptr<Graphics>& renderer, shared_ptr<Input> input, shared_ptr<Screen> screen, tinyxml2::XMLElement* buttonElement)
	{
		Font font = *AssetManager::getInstance()->getFont("arial.ttf", renderer);
		const char* text = buttonElement->FirstChildElement("value") != NULL ? buttonElement->FirstChildElement("value")->GetText() : "MISSING_STRING";
		shared_ptr<Transform> transform = std::make_shared<Transform>();
		loadTransform(transform, buttonElement);
		glm::vec3 colour;
		buttonElement->FirstChildElement("colour") != NULL ? loadColour(colour, buttonElement->FirstChildElement("colour"), glm::vec3(1.0, 1.0, 1.0)) : colour = glm::vec3(1.0, 1.0, 1.0);
		string id;
		buttonElement->Attribute("id") != NULL ? id = buttonElement->Attribute("id") : id = "";
		shared_ptr<Button> button = std::make_shared<Button>(text, font, transform, renderer, colour, id);
		screen->addButton(button);
		input->registerMouseListener(button);
		//string funcName = string(buttonElement->FirstChildElement("function")->Attribute("type"));
		loadButtonFunc(buttonElement, button, engine);
	}

	/*
		Utility method to bind the supplied parameters to the requested callback function.
	*/
	static void loadButtonFunc(tinyxml2::XMLElement* buttonElement, shared_ptr<Button> button, Engine* engine)
	{
		tinyxml2::XMLElement* onClickElement = buttonElement->FirstChildElement("onClick");
		string scriptName = onClickElement->FirstChildElement("script") != NULL ? onClickElement->FirstChildElement("script")->GetText() : "default.lua"; 
		string scriptFile = AssetManager::getInstance()->getScript(scriptName.c_str());
		string function = onClickElement->FirstChildElement("function") != NULL ? onClickElement->FirstChildElement("function")->GetText() : "doNothing";
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
			std::transform(paramName.begin(), paramName.end(), paramName.begin(), std::tolower);
			paramTable[paramName] = paramElement->GetText();
			paramElement = paramElement->NextSiblingElement();
		}
		//Bind the lua callback function, engine and parameter table as a c++ function for the button.
		button->addOnClickFn(std::bind(callback, engine, paramTable));
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
			tinyxml2::XMLElement* UIID = UIDocElement->FirstChildElement("ID");
			//load UI using texture
			tinyxml2::XMLElement* UITexture = UIDocElement->FirstChildElement("Texture");
			if (UITexture)
			{
				screen->addUIElement(std::make_shared<UITextureElement>(renderer, transform, UIID->GetText(), UITexture->GetText()));
			}
			else
			{
				//text?
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
		shared_ptr<Camera> camera = std::make_shared<PerspectiveCamera>(engine->getWindowWidth(), engine->getWindowHeight(), 45.f);
		shared_ptr<GameScreen> gameScreen = std::make_shared<GameScreen>(renderer, input, engine->getPhysics(), camera);
		input->setKeyFocus(gameScreen);
		gameScreen->setID(screenElement->Attribute("name"));
		gameScreen->setXMLDocument(screenDocument);
		gameScreen->setXMLFilePath(filepath);
		tinyxml2::XMLElement* gameObjElement = screenElement->FirstChildElement("gameObjects")->FirstChildElement();
		while (gameObjElement != NULL) {
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
		tinyxml2::XMLElement* stringElement = screenElement->FirstChildElement("strings");
		if (stringElement != NULL) stringElement = stringElement->FirstChildElement();
		while (stringElement != NULL) {
			loadStringElement(renderer, gameScreen, stringElement);
			stringElement = stringElement->NextSiblingElement();
		}

		tinyxml2::XMLElement* buttonElement = screenElement->FirstChildElement("buttons");
		if (buttonElement)
			buttonElement = buttonElement->FirstChildElement();
		while (buttonElement != NULL) {
			loadButtonElement(engine, renderer, input, gameScreen, buttonElement);
			buttonElement = buttonElement->NextSiblingElement();
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
		shared_ptr<ControllerComponent> controller = std::make_shared<ControllerComponent>(physics, gameObject, shapeData);
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

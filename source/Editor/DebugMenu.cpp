#include "Editor/DebugMenu.h"
#include "utils\DebugUtils.h"
#include "utils/levelloader.h"
#include <utils\DirectoryReader.h>
#include <utils\FileSaver.h>

bool DebugMenu::initialised = false;
shared_ptr<DebugMenu> DebugMenu::instance;
#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))


shared_ptr<DebugMenu> DebugMenu::getInstance()
{
	if (initialised) {
		return instance;
	}
	instance = shared_ptr<DebugMenu>(new DebugMenu());
	initialised = true;
	return instance;
}

void DebugMenu::init()
{
	showGameObjects = false;
	showCube = false;
	showSaveAsMenu = false;
	listbox_item_current = 1;
}

void DebugMenu::update()
{
	updateMainMenu();
	updateLogic();
	DebugUtils::getInstance()->update();
}

void DebugMenu::updateMainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::Button("Save"))
			{
				if (saveCurrentLevel(Engine::g_pEngine->getActiveScreen()->getXMLFilePath()))
				{
					popupText = "Saved successfully";
				}
				else
				{
					popupText = "Save FAILED";
				}
				DebugUtils::getInstance()->popup(popupText);


			}
			if (ImGui::Button("SaveAs..."))
			{
				showSaveAsMenu = true;
			}

			if (ImGui::Button("Reload Scene"))
			{
				if (Engine::g_pEngine->replaceScreen(Engine::g_pEngine->getActiveScreen()->getID()))
				{
					popupText = "Load successfully";
				}
				else
				{
					popupText = "Load FAILED";
				}
				DebugUtils::getInstance()->popup(popupText);
			}

			if (ImGui::Button("Load..."))
			{
				showLoadMenu = true;
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Game"))
		{
			bool editorState = (bool)Engine::g_pEngine->getEngineState();
			if (ImGui::Checkbox("Play", &editorState))
			{
				Engine::g_pEngine->getEngineState()->switchEngineMode();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Debug"))
		{
			if (ImGui::Button("Objects"))
			{
				showGameObjects = true;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Create"))
		{
			shared_ptr<GameScreen> gameScreen = std::static_pointer_cast<GameScreen>(Engine::g_pEngine->getActiveScreen());
			if (Engine::g_pEngine->getActiveScreen()->getType() == Screen::type::GAMESCREEN)
			{
				if (ImGui::TreeNode("GameObject"))
				{
					//Load objects from dir
					if (objList.empty())
					{
						std::string path = AssetManager::getInstance()->getRootFolder(AssetManager::ResourceType::MODEL) + "*.obj";
						objList = DirectoryReader::getFilesInDirectory(path.c_str());
					}
					//Iterate through object list
					for (int x = 0; x < objList.size(); x++)
					{
						ImGui::PushID(x);
						/*
						each object needs its own window
						another vector for checking if window is active.
						*/
						if (objCreateActive.size() != objList.size())
						{
							objCreateActive.push_back(false);
						}
						//If the object button pressed
						if (ImGui::Button(objList[x].c_str()))
						{
							objCreateActive[x] = true;
						}
						//open the window for it
						if (objCreateActive[x])
						{
							createObjectWindow(objList[x], x);
						}
						ImGui::PopID();
					}
					ImGui::TreePop();
				}
				else
				{
					objList.clear();
				}
				if (ImGui::TreeNode("Lights"))
				{
					static Light* light = new Light();
					lightsMenu(0, light);
					if (ImGui::Button("Create"))
					{
						gameScreen->addLight(*light);
					}
					ImGui::TreePop();
				}
			}
			
			//Create a UI element.
			if (ImGui::TreeNode("UI"))
			{
				//For every element.
				for (int i = UIType::TEXT; i < UIType::UI_TYPE_COUNT; i++)
				{
					ImGui::PushID(i);
					UIType uiType = static_cast<UIType>(i);
					
					//A button for each UIElement
					if (ImGui::Button(EnumParser<UIType>().getString(uiType).c_str()))
					{
						uiCreateActive[i]= true;
					}
					ImGui::PopID();
				}
				ImGui::TreePop();
			}

			
			

			ImGui::EndMenu();
		}
		//Create a UI element.
		if (ImGui::BeginMenu("Lights"))
		{
			shared_ptr<GameScreen> gameScreen = std::static_pointer_cast<GameScreen>(Engine::g_pEngine->getActiveScreen());
			if (Engine::g_pEngine->getActiveScreen()->getType() == Screen::type::GAMESCREEN)
			{
				for (int x = 0; x < gameScreen->getLights()->size(); x++)
				{
					char lightName[14];
					snprintf(lightName, sizeof(lightName), "L_%d", x);
					if (ImGui::TreeNode(lightName))
					{
						lightsMenu(x, &gameScreen->getLights()->at(x));
						gameScreen->updateLighting();
						ImGui::TreePop();
					}
				}
			}
			
			
			ImGui::EndMenu();
		}
		/*

		inherit from.
		class debugmenuitem


		provide definition for:
		virtual update;

		example
		debugMenuItemUpdate()
		{
			if(beginmenu "")
			{
			dostuff.
			}
		}

		debugmenu has list of debugmenuitems.
		iterates -> debugMenuItemUpdate().
		instansiation adds to list of.
		*/
		for (int x = 0; x < mainMenuBarItems.size(); x++)
		{
			mainMenuBarItems[x]->debugMenuItemUpdate();
		}
		ImGui::EndMainMenuBar();

	}
}

void DebugMenu::updateLogic()
{
	//Create Object window active
	bool allCreateWindowsClosed = true;
	for (int x = 0; x < objCreateActive.size(); x++)
	{
		if (objCreateActive[x])
		{
			createObjectWindow(objList[x], x);
			allCreateWindowsClosed = false;
		}
	}
	
	for (int i = UIType::TEXT; i < IM_ARRAYSIZE(uiCreateActive); i++)
	{
		UIType uiType = static_cast<UIType>(i);
		if (uiCreateActive[i])
		{
			createUIWindow(uiType, i);
			allCreateWindowsClosed = false;
		}

	}
	if (allCreateWindowsClosed)
	{
		//reload textures when windows are closed
		if (!textureList.empty())
			textureList.clear();
	}
	if (showGameObjects)
	{
		debugGameObjectsMenu();
	}

	if (showSaveAsMenu)
	{
		saveAsMenu();
	}

	if (showLoadMenu)
	{
		loadSpecificLevel();
	}
	else
	{
		//reload levels list when the windows are closed
		if (!levelList.empty())
		{
			levelList.clear();
		}
	}

	
}

void DebugMenu::debugGameObjectsMenu()
{
	if (Engine::g_pEngine->getActiveScreen()->getType() != Screen::type::GAMESCREEN)
	{
		return;
	}

	ImGui::Begin("GameObjects", &showGameObjects);
	shared_ptr<GameScreen> gameScreen = std::static_pointer_cast<GameScreen>(Engine::g_pEngine->getActiveScreen());
	/*
		Every game object from the vector is listed.
	*/
	if (!gameScreen)
		return;
	for (int x = 0; x < gameScreen->getGameObjects().size(); x++)
	{
		//Push and PopID, useful for similarly named objects.
		ImGui::PushID(x);
		static bool showMore = false;
		char goName[50];
		auto model =  gameScreen->getComponentStore()->getComponent<ModelComponent>(gameScreen->getGameObjects()[x]->GetComponentHandle(ComponentType::MODEL), ComponentType::MODEL);
		if (model)
		{
			snprintf(goName, sizeof(goName), "%s_%d", model->getId().c_str(), x);
		}
		else {
			auto ani = gameScreen->getComponentStore()->getComponent<AnimatedModelComponent>(gameScreen->getGameObjects()[x]->GetComponentHandle(ComponentType::ANIMATION), ComponentType::ANIMATION);
			if (ani)
			{
				snprintf(goName, sizeof(goName), "%s_%d", ani->getCurrentAnim().c_str(), x);
			}
			else
				snprintf(goName, sizeof(goName), "%s_%d", "ID_ERROR", x);
		}

		
		//Tree node creates a tree from the game object name
		if (ImGui::TreeNode(goName))
		{
			//savable
			static bool savable = gameScreen->getGameObjects().at(x)->isSavable();
			if (ImGui::Checkbox("Savable", &savable))
			{
				gameScreen->getGameObjects().at(x)->setSavable(savable);
			}
			/*
				Inside the gameobject is a list of the components
			*/
			for (int i = ComponentType::MODEL; i < ComponentType::COMPONENT_TYPE_COUNT; i++)
			{
				ComponentType cType = static_cast<ComponentType>(i);
					
				if (gameScreen->getGameObjects()[x]->HasComponent(cType))
				{
					ImGui::PushID(i);
					char compName[14];
					snprintf(compName, sizeof(compName), "%s_%d", EnumParser<ComponentType>().getString(cType), i);
					//Each component has editable stuff
					if (ImGui::TreeNode(compName))
					{
						switch (cType)
						{
						case ComponentType::MODEL:
							gameObjectsMenuModel(i, model);
							break;
						case ComponentType::ANIMATION:
							gameObjectsMenuAnimation();
							break;
						case ComponentType::RIGID_BODY:
						{
							auto physComp = gameScreen->getComponentStore()->getComponent<PhysicsComponent>(gameScreen->getGameObjects()[x]->GetComponentHandle(ComponentType::RIGID_BODY), ComponentType::RIGID_BODY);
							auto transform = gameScreen->getComponentStore()->getComponent<Transform>(gameScreen->getGameObjects()[x]->GetComponentHandle(ComponentType::TRANSFORM), ComponentType::TRANSFORM);
							if (physComp && transform)
								gameObjectsMenuRigidBody(i, physComp, transform);
							break;
						}
						case ComponentType::LOGIC:
						{
							auto logic = gameScreen->getComponentStore()->getComponent<LogicComponent>(gameScreen->getGameObjects()[x]->GetComponentHandle(ComponentType::LOGIC), ComponentType::LOGIC);
							gameObjectsMenuLogic(i, logic);
							break;
						}
						case ComponentType::TRANSFORM:
						{
							auto transform = gameScreen->getComponentStore()->getComponent<Transform>(gameScreen->getGameObjects()[x]->GetComponentHandle(ComponentType::TRANSFORM), ComponentType::TRANSFORM);
							gameObjectsMenuTransform(i, transform);
							break;
						}
						case ComponentType::TRIGGER:
						{
							auto trigger = gameScreen->getComponentStore()->getComponent<CollisionTrigger>(gameScreen->getGameObjects()[x]->GetComponentHandle(ComponentType::TRIGGER), ComponentType::TRIGGER);
							gameObjectsMenuTrigger(i, trigger);
							break;
						}
						}
						
						ImGui::TreePop();
					}
					ImGui::PopID();
				}
				
			}
			ImGui::TreePop();
			if (!gameScreen->getGameObjects()[x]->HasComponent(ComponentType::RIGID_BODY))
			{
				static bool hasPhysics = false;

				if (ImGui::Checkbox("AddPhysics", &hasPhysics))
				{

				}
				if (hasPhysics)
				{
					ImGui::Indent();
					static shared_ptr<PhysicsComponent> phy = std::make_shared<PhysicsComponent>();
					auto transform = gameScreen->getComponentStore()->getComponent<Transform>(gameScreen->getGameObjects()[x]->GetComponentHandle(ComponentType::TRANSFORM), ComponentType::TRANSFORM);
					if (transform)
					{
						gameObjectsMenuRigidBody(0, phy.get(), transform);
						if (ImGui::Button("Add"))
						{
							phy->init(Engine::g_pEngine->getPhysics(), gameScreen->getGameObjects()[x], *phy->getMass());
							gameScreen->getGameObjects()[x]->AddComponent(phy, ComponentType::RIGID_BODY);
						}
					}
					
				}
			}
			if (!gameScreen->getGameObjects()[x]->HasComponent(ComponentType::LOGIC))
			{
				static bool hasLogic = false;

				if (ImGui::Checkbox("AddLogic", &hasLogic))
				{

				}
				if (hasLogic)
				{
					ImGui::Indent();
					static shared_ptr<LogicComponent> logic = std::make_shared<LogicComponent>(gameScreen->getGameObjects()[x], gameScreen);
					gameObjectsMenuLogic(0, logic.get());
					if (ImGui::Button("Add"))
					{
						logic->registerLuaBindings();
						gameScreen->getGameObjects()[x]->AddComponent(logic, ComponentType::LOGIC);
					}
				}
			}
			if (!gameScreen->getGameObjects()[x]->HasComponent(ComponentType::TRIGGER))
			{
				static bool hasTrigger = false;

				if (ImGui::Checkbox("AddTrigger", &hasTrigger))
				{

				}
				if (hasTrigger)
				{
					ImGui::Indent();
					static shared_ptr<CollisionTrigger> trigger = std::make_shared<CollisionTrigger>();
					gameObjectsMenuTrigger(0, trigger.get());
					if (ImGui::Button("Add"))
					{
						trigger->setOwner(gameScreen->getGameObjects()[x]);
						trigger->init();
						gameScreen->getGameObjects()[x]->AddComponent(trigger, ComponentType::TRIGGER);
						std::shared_ptr<BulletPhysics> physicsPtr = std::dynamic_pointer_cast<BulletPhysics>(Engine::g_pEngine->getPhysics());
						if (physicsPtr != nullptr)
						{
							physicsPtr->addTrigger(trigger);
						}
					}
				}
			}
			
			if (ImGui::Button("Duplicate"))
			{
				LevelLoader::duplicateGameObject(gameScreen, gameScreen->getGameObjects()[x]);
			}
		}
		ImGui::PopID();
	}
	ImGui::End();
}

bool DebugMenu::saveCurrentLevel(string fileName)
{
	int numberOfObjectsInFile = 0;
	if (Engine::g_pEngine->getActiveScreen()->getType() == Screen::type::GAMESCREEN)
	{
		shared_ptr<GameScreen> gameScreen = std::static_pointer_cast<GameScreen>(Engine::g_pEngine->getActiveScreen());
		numberOfObjectsInFile = XMLReader::GetNumberOfGameObjectsInFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument());
		/*
		Every game object from the vector is listed.
		*/
		for (int x = 0; x < gameScreen->getGameObjects().size(); x++)
		{
			FileSaver::UpdateFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument(), fileName, x, gameScreen->getGameObjects()[x], gameScreen);
			//If there is a new object not saved on file
			if (x >= numberOfObjectsInFile)
			{
				//addto
				if (FileSaver::AddObjectToFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument(), x, gameScreen->getGameObjects()[x], gameScreen))
				{
				}
			}
			
		}
		for (int x = 0; x < gameScreen->getGameObjects().size(); x++)
		{
			if (gameScreen->getGameObjects().at(x)->HasComponent(ComponentType::MODEL))
			{
				if (!gameScreen->getGameObjects().at(x)->isSavable())
				{
					FileSaver::DeleteObjectFromFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument(), x, gameScreen->getGameObjects()[x], gameScreen);
				}
			}
		}
	}
	/*
		Every UI object
	*/
	numberOfObjectsInFile = XMLReader::GetNumberOfUIElementsInFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument());
	int unsavableObjectsCount = 0;
	for (int x = 0; x < Engine::g_pEngine->getActiveScreen()->getUIElements().size(); x++)
	{
		if (!Engine::g_pEngine->getActiveScreen()->getUIElements()[x]->isSavable())
		{
			unsavableObjectsCount++;
			continue;
		}
		FileSaver::UpdateFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument(), fileName, x-unsavableObjectsCount, Engine::g_pEngine->getActiveScreen()->getUIElements()[x]);
		//If there is a new object not saved on file
		if (x-unsavableObjectsCount >= numberOfObjectsInFile)
		{
			if (FileSaver::AddObjectToFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument(), x-unsavableObjectsCount, Engine::g_pEngine->getActiveScreen()->getUIElements()[x]))
			{
			}
		}
		if (!Engine::g_pEngine->getActiveScreen()->getUIElements().at(x)->isActive())
		{
			FileSaver::DeleteObjectFromFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument(), x-unsavableObjectsCount, Engine::g_pEngine->getActiveScreen()->getUIElements()[x]);
		}
	}

	/*
		every light
	*/
	numberOfObjectsInFile = XMLReader::GetNumberOfLightElementsInFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument());
	shared_ptr<GameScreen> gameScreen = std::static_pointer_cast<GameScreen>(Engine::g_pEngine->getActiveScreen());
	for (int x = 0; x < gameScreen->getLights()->size(); x++)
	{
		FileSaver::UpdateFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument(), fileName, x, &gameScreen->getLights()->at(x));
		//If there is a new object not saved on file
		if (x >= numberOfObjectsInFile)
		{
			if (FileSaver::AddObjectToFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument(), x, &gameScreen->getLights()->at(x)))
			{
			}
		}
		if (!gameScreen->getLights()->at(x).saved)
		{
			FileSaver::DeleteObjectFromFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument(), x, &gameScreen->getLights()->at(x));
		}
	}

	return FileSaver::SaveFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument(),fileName);
}

void DebugMenu::saveAsMenu()
{
	ImGui::Begin("SaveAs", &showSaveAsMenu);
	static char Buffer[50] = "";
	ImGui::InputText("File Name", Buffer, sizeof(Buffer));
	if (ImGui::Button("Save"))
	{
		if (saveCurrentLevel(string(AssetManager::getInstance()->getRootFolder(AssetManager::ResourceType::LEVEL) + Buffer)))
		{
			popupText = "Saved successfully";
		}
		else
		{
			popupText = "Save FAILED";
		}
		DebugUtils::getInstance()->popup(popupText);
		
	}
	ImGui::End();
}

bool DebugMenu::loadLevel(string fileName)
{
	bool loadResult = LevelLoader::loadLevel(Engine::g_pEngine.get(), Engine::g_pEngine->getRenderer(), Engine::g_pEngine->getInput(), fileName.c_str());
	return loadResult;
}

void DebugMenu::loadSpecificLevel()
{
	ImGui::Begin("Load", &showLoadMenu);
	static std::vector<const char *> levelCStyleArray;
	if (levelList.empty())
	{
		//Gets all levelsin levels dir
		//vector<std::string> tempTextureList;
		std::string path = AssetManager::getInstance()->getRootFolder(AssetManager::ResourceType::LEVEL) + "*.xml";
		levelList = DirectoryReader::getFilesInDirectory(path.c_str());

		//For specific file types

		/*path = AssetManager::getInstance()->getRootFolder(AssetManager::ResourceType::TEXTURE) + "*.jpg";
		tempTextureList = DirectoryReader::getFilesInDirectory(path.c_str());
		textureList.insert(textureList.end(), tempTextureList.begin(), tempTextureList.end());

		path = AssetManager::getInstance()->getRootFolder(AssetManager::ResourceType::TEXTURE) + "*.tga";
		tempTextureList = DirectoryReader::getFilesInDirectory(path.c_str());
		textureList.insert(textureList.end(), tempTextureList.begin(), tempTextureList.end());*/
		if (!levelCStyleArray.empty())
			levelCStyleArray.clear();
		levelCStyleArray.reserve(levelList.size());
		for (int index = 0; index < levelList.size(); ++index)
		{
			levelCStyleArray.push_back(levelList[index].c_str());
		}
	}
	/*
	Create a selectable list for levels
	*/
	static int listbox_item_current = 1;
	ImGui::ListBox("Levels Available", &listbox_item_current, &levelCStyleArray[0], levelCStyleArray.size(), 4);

	if (ImGui::Button("Load"))
	{
		if (loadLevel(string(AssetManager::getInstance()->getRootFolder(AssetManager::ResourceType::LEVEL) + levelCStyleArray[listbox_item_current])))
		{
			//remove .xml from string
			string levelID = levelCStyleArray[listbox_item_current];
			levelID = levelID.substr(0, levelID.size()-4);
			Engine::g_pEngine->replaceScreen(levelID);
			popupText = "Load successfully";
		}
		else
		{
			popupText = "Load FAILED";
		}
		DebugUtils::getInstance()->popup(popupText);

	}
	ImGui::End();
}

void DebugMenu::createCubeMenu()
{
	ImGui::Begin("CreateCube", &showCube);

	static float createPosition[3] = { 0.0f, 0.0f, 0.0f };
	static float createScale[3] = { 1.0f, 1.0f, 1.0f };
	static float createOrientation[3] = { 0.0f, 1.0f, 0.0f };
	ImGui::DragFloat3("Position", createPosition, 0.5f);
	ImGui::DragFloat3("Scale", createScale, 0.5f);
	ImGui::DragFloat3("Orientation", createOrientation, 0.5f);

	if (ImGui::Button("Create"))
	{
		//Load cube, overide
		shared_ptr<GameScreen> gameScreen = std::static_pointer_cast<GameScreen>(Engine::g_pEngine->getActiveScreen());
		shared_ptr<Transform> transform = std::make_shared<Transform>();
		transform->position = glm::vec3(createPosition[0], createPosition[1], createPosition[2]);
		transform->orientation = glm::vec3(createOrientation[0], createOrientation[1], createOrientation[2]);
		transform->scale = glm::vec3(createScale[0], createScale[1], createScale[2]);
		LevelLoader::loadGameObject(Engine::g_pEngine->getRenderer(), gameScreen, std::pair<string,string>("cube.obj",""), transform);

	}
	ImGui::End();

	
}

void DebugMenu::createObjectWindow(std::string objName, int iterator)
{
	/*
		Vector of bools does not store in contiguous memory.
		Hence the use of secondary bool for window.
	*/
	bool bWindowActive = objCreateActive.at(iterator);
	ImGui::Begin(objName.c_str(), &bWindowActive);
	objCreateActive.at(iterator) = bWindowActive;

	ImGui::PushID(iterator);
	//transform
	static shared_ptr<Transform> transform = std::make_shared<Transform>();
	gameObjectsMenuTransform(iterator, transform.get());
	//
	static bool hasLogic = false;
	static shared_ptr<LogicComponent> logic = std::make_shared<LogicComponent>();
	if (ImGui::Checkbox("Logic", &hasLogic))
	{
	}
	if (hasLogic)
	{
		//logic
		gameObjectsMenuLogic(iterator, logic.get());
	}
	//
	static bool hasTexture = false;
	if (ImGui::Checkbox("Texture", &hasTexture))
	{
	}
	if (hasTexture)
	{
		createTextureListBox();
	}
	static bool hasPhysics = false;
	static shared_ptr<PhysicsComponent> phyComp = std::make_shared<PhysicsComponent>();
	if (ImGui::Checkbox("Physics", &hasPhysics))
	{
	}
	if (hasPhysics)
	{
		gameObjectsMenuRigidBody(0, phyComp.get(), transform.get());
	}
	static bool hasTrigger = false;
	static shared_ptr<CollisionTrigger> trigger = std::make_shared<CollisionTrigger>();
	if (ImGui::Checkbox("Trigger", &hasTrigger))
	{
	}
	if (hasTrigger)
	{
		gameObjectsMenuTrigger(0, trigger.get());
	}
	if (ImGui::Button("Create"))
	{
		//Load object, overide
		shared_ptr<GameScreen> gameScreen = std::static_pointer_cast<GameScreen>(Engine::g_pEngine->getActiveScreen());
		std::pair<string, string> objInfo(objName, "");
		if (hasTexture)
		{
			objInfo.second = textureCStyleArray[listbox_item_current];
		}
		
		LevelLoader::loadGameObject(Engine::g_pEngine->getRenderer(), gameScreen, objInfo, transform);

		if (hasPhysics)
		{
			phyComp->init(Engine::g_pEngine->getPhysics(), gameScreen->getGameObjects().back(), *phyComp->getMass());
			gameScreen->getGameObjects().back()->AddComponent(phyComp, ComponentType::RIGID_BODY);
		}
		if (hasLogic)
		{
			logic->setOwner(gameScreen->getGameObjects().back());
			logic->setScreen(gameScreen);
			logic->registerLuaBindings();
			gameScreen->getGameObjects().back()->AddComponent(logic, ComponentType::LOGIC);
		}
		if (hasTrigger)
		{
			trigger->setOwner(gameScreen->getGameObjects().back());
			trigger->init();
			gameScreen->getGameObjects().back()->AddComponent(trigger, ComponentType::TRIGGER);
			std::shared_ptr<BulletPhysics> physicsPtr = std::dynamic_pointer_cast<BulletPhysics>(Engine::g_pEngine->getPhysics());
			if (physicsPtr != nullptr)
			{
				physicsPtr->addTrigger(trigger);
			}
		}
	}
	ImGui::PopID();
	ImGui::End();
}

void DebugMenu::createUIWindow(UIType type, int iterator)
{
	bool uiWindowActive = uiCreateActive[iterator];
	string uiWindowName = EnumParser<UIType>().getString(type).c_str() + std::to_string(iterator);
	ImGui::Begin(uiWindowName.c_str(), &uiWindowActive);
	uiCreateActive[iterator] = uiWindowActive;
	//declarations
	static char uiStringValueBuf[64] = "";
	static glm::vec3 colour;
	/*
		Name of uielement
	*/
	static char objNameBuf[64] = "";
	ImGui::InputText("UIName", objNameBuf, sizeof(objNameBuf));
	/*
		Specfic type tools
	*/
	switch (type)
	{
	case UIType::TEXT:
	{
		//text
		ImGui::InputText("UITextValue", uiStringValueBuf, sizeof(uiStringValueBuf));
		//colour
		static float dragSpeed = 0.0025f;
		ImGui::DragFloat3("Colour", &colour[0], dragSpeed);

		//font
		createFontsListBox();
		break;
	}
	case UIType::TEXTURE:
		//

		createTextureListBox();
		break;
	}

	//Button
	static bool hasButton = false;
	shared_ptr<Button> createdButton;
	//HasButton?
	if (ImGui::Checkbox("HasButton", &hasButton))
	{
	}
	if (hasButton)
	{
		createdButton = createButton();
	}
	else
	{
		createdButton.reset();
	}
	
	
	/*
		transform editor
	*/
	static shared_ptr<Transform> transform = std::make_shared<Transform>();
	gameObjectsMenuTransform(iterator, transform.get());
	if (ImGui::Button("Create"))
	{
		//Load object, overide
		shared_ptr<GameScreen> gameScreen = std::static_pointer_cast<GameScreen>(Engine::g_pEngine->getActiveScreen());
		//No texture init
		std::pair<string, string> objInfo(string(objNameBuf), "");
		switch (type)
		{
		case UIType::TEXT:
		{
			Font font = *AssetManager::getInstance()->getFont(const_cast<char*>(listBoxItemSelected(type).c_str()), Engine::g_pEngine->getRenderer());
			shared_ptr<TextBox> textBox = std::make_shared<TextBox>(uiStringValueBuf, font, transform, Engine::g_pEngine->getRenderer(), colour, objNameBuf);
			if (hasButton)
			{
				Engine::g_pEngine->getInput()->registerMouseListener(createdButton);
				createdButton->init(Engine::g_pEngine->getRenderer(), font, uiStringValueBuf, transform);
				textBox->setButton(createdButton);
			}
			Engine::g_pEngine->getActiveScreen()->addUIElement(textBox);
			break;
		}
		case UIType::TEXTURE:
			objInfo.second = textureCStyleArray[listbox_item_current];
			shared_ptr<UITextureElement> uiTexture = std::make_shared<UITextureElement>(Engine::g_pEngine->getRenderer(), transform, objInfo.first.c_str(), objInfo.second.c_str());
			if (hasButton)
			{
				Engine::g_pEngine->getInput()->registerMouseListener(createdButton);
				createdButton->init(Engine::g_pEngine->getRenderer(), transform);
				uiTexture->setButton(createdButton);
			}
			Engine::g_pEngine->getActiveScreen()->addUIElement(uiTexture);
			break;
		}
		
	}
	ImGui::End();
}

shared_ptr<Button> DebugMenu::createButton()
{
	static shared_ptr<Button> button = std::make_shared<Button>();

	if (ImGui::TreeNode("Button"))
	{

		static char buttonScriptName[64] = "";
		static char buttonFuncName[64] = "";
		static char buttonParamID[64] = "";
		static char buttonParamText[64] = "";

		//inputs
		ImGui::InputText("ScriptName", buttonScriptName, sizeof(buttonScriptName));
		ImGui::InputText("FuncName", buttonFuncName, sizeof(buttonFuncName));
		ImGui::InputText("ParamID", buttonParamID, sizeof(buttonParamID));
		ImGui::InputText("ParamText", buttonParamText, sizeof(buttonParamText));
		button->setScript(buttonScriptName);
		button->setFunc(buttonFuncName);
		if (ImGui::Button("AddParam"))
		{
			button->setParam(pair<string, string>(buttonParamID, buttonParamText));
			LevelLoader::loadButtonFunc(button);
		}
		ImGui::SameLine();
		if (ImGui::Button("ClearButtonParams"))
		{
			button->clearParams();
		}
		ImGui::TreePop();
	}
	return button;
}

void DebugMenu::createTextureListBox()
{
	//static std::vector<const char *> textureCStyleArray;
	if (textureList.empty())
	{
		//Gets all textures in tex dir
		//vector<std::string> tempTextureList;
		std::string path = AssetManager::getInstance()->getRootFolder(AssetManager::ResourceType::TEXTURE) + "*";
		textureList = DirectoryReader::getFilesInDirectory(path.c_str());
		//For specific file types

		/*path = AssetManager::getInstance()->getRootFolder(AssetManager::ResourceType::TEXTURE) + "*.jpg";
		tempTextureList = DirectoryReader::getFilesInDirectory(path.c_str());
		textureList.insert(textureList.end(), tempTextureList.begin(), tempTextureList.end());

		path = AssetManager::getInstance()->getRootFolder(AssetManager::ResourceType::TEXTURE) + "*.tga";
		tempTextureList = DirectoryReader::getFilesInDirectory(path.c_str());
		textureList.insert(textureList.end(), tempTextureList.begin(), tempTextureList.end());*/

		if (!textureCStyleArray.empty())
			textureCStyleArray.clear();
		textureCStyleArray.reserve(textureList.size());
		for (int index = 0; index < textureList.size(); ++index)
		{
			//Safety check, name must be longer than 4 to be valid (.png) etc.
			if (strlen(textureList.at(index).c_str()) > 4)
			{
				textureCStyleArray.push_back(textureList[index].c_str());
			}
			else
			{
				textureList.at(index).erase();
			}
				
		}
	}
	/*
	Create a selectable list for textures
	*/
	ImGui::ListBox("Textures Available", &listbox_item_current, &textureCStyleArray[0], textureCStyleArray.size(), 4);

}

void DebugMenu::createFontsListBox()
{
	//static std::vector<const char *> textureCStyleArray;
	if (fontList.empty())
	{
		//Gets all textures in tex dir
		//vector<std::string> tempTextureList;
		std::string path = AssetManager::getInstance()->getRootFolder(AssetManager::ResourceType::FONT) + "*";
		fontList = DirectoryReader::getFilesInDirectory(path.c_str());
		//For specific file types

		/*path = AssetManager::getInstance()->getRootFolder(AssetManager::ResourceType::TEXTURE) + "*.jpg";
		tempTextureList = DirectoryReader::getFilesInDirectory(path.c_str());
		textureList.insert(textureList.end(), tempTextureList.begin(), tempTextureList.end());

		path = AssetManager::getInstance()->getRootFolder(AssetManager::ResourceType::TEXTURE) + "*.tga";
		tempTextureList = DirectoryReader::getFilesInDirectory(path.c_str());
		textureList.insert(textureList.end(), tempTextureList.begin(), tempTextureList.end());*/

		if (!fontCStyleArray.empty())
			fontCStyleArray.clear();
		fontCStyleArray.reserve(fontList.size());
		for (int index = 0; index < fontList.size(); ++index)
		{
			//Safety check, name must be longer than 4 to be valid (.ttf) etc.
			if (strlen(fontList.at(index).c_str()) > 4)
			{
				fontCStyleArray.push_back(fontList[index].c_str());
			}
			else
			{
				fontList.at(index).erase();
			}

		}
	}
	/*
	Create a selectable list for textures
	*/
	ImGui::ListBox("Textures Available", &listbox_item_current, &fontCStyleArray[0], fontCStyleArray.size(), 4);

}

string DebugMenu::listBoxItemSelected(UIType type)
{
	string returnedString = "";
	switch (type)
	{
	case TEXT:
		//Safety first!
		if (fontCStyleArray.size() > listbox_item_current)
		{
			returnedString = fontCStyleArray[listbox_item_current];
		}
		else
		{
			returnedString = "arial.ttf";
			listbox_item_current = 0;
		}
		break;
	case TEXTURE:
		if (textureCStyleArray[listbox_item_current])
		{
			returnedString = textureCStyleArray[listbox_item_current];
		}
		break;
	default:
		break;
	}
	return returnedString;
	
}

void DebugMenu::render()
{
	ImGui::Render();
}

void DebugMenu::addMenuItem(DebugMenuItem* dmi)
{
	mainMenuBarItems.push_back(dmi);
}

void DebugMenu::refreshMenuItems()
{
	//When we load a scene, the runtime menu items need to be reset
	for (int x = 0; x < mainMenuBarItems.size(); x++)
	{
		if (mainMenuBarItems[x]->canClear())
		{
			mainMenuBarItems.erase(mainMenuBarItems.begin()+x);
		}
	}
}

vector<DebugMenuItem*> DebugMenu::getMenuItems()
{
	return mainMenuBarItems;
}


void DebugMenu::gameObjectsMenuModel(int i, ModelComponent* model)
{
	ImGui::PushID(i);
	
	static char objNameBuf[64] = "";

	if (strlen(objNameBuf) == 0)
	{
		strncpy_s(objNameBuf, model->getId().c_str(), IM_ARRAYSIZE(model->getId().c_str()));
	}
	
	ImGui::InputText("Object Name", objNameBuf, IM_ARRAYSIZE(objNameBuf));
	
	if (ImGui::Button("Toggle"))
	{
		model->toggleDrawing();
	}

	if (ImGui::Button("Update"))
	{
		model->setID(objNameBuf);
	}
	ImGui::PopID();
}

void DebugMenu::gameObjectsMenuAnimation()
{
}

void DebugMenu::gameObjectsMenuRigidBody(int i, PhysicsComponent* phyComp, Transform* tranform)
{
	ImGui::PushID(i);
	ImGui::Indent();
	static float dragSpeed = 0.25f;
	bool hasMeshFile = *phyComp->hasMeshFile();
	bool isConvex = *phyComp->isConvex();
	char meshNameBuf[64] = "";
	if (phyComp->getMeshFileName()->size() > 0)
	{
		strncpy_s(meshNameBuf, phyComp->getMeshFileName()->c_str(), phyComp->getMeshFileName()->size());
	}
	
	static float restitution = phyComp->getRestitution();
	static float friction = phyComp->getFriction();
	if (ImGui::Checkbox("IsConvex", &isConvex))
	{
		phyComp->setConvex(isConvex);
	}
	ImGui::DragFloat("Mass", phyComp->getMass(), dragSpeed);
	if (ImGui::Checkbox("HasMesh", &hasMeshFile))
	{
		phyComp->setHasMesh(hasMeshFile);
	}
	
	if (hasMeshFile)
	{
		ImGui::InputText("MeshName", meshNameBuf, IM_ARRAYSIZE(meshNameBuf));
		phyComp->setMeshFileName(meshNameBuf);
		if (ImGui::Button("LoadMesh"))
		{
			if (isConvex)
			{
				auto mesh = AssetManager::getInstance()->getModelData(meshNameBuf, Engine::g_pEngine->getRenderer());
				phyComp->buildCollisionShape(mesh, tranform->scale);
			}
			else
			{
				auto mesh = AssetManager::getInstance()->getCollisionData(meshNameBuf);
				phyComp->buildCollisionShape(mesh, tranform->scale);
			}
		}
	}
	else
	{
		static ShapeData* shapeData;
		if (phyComp->getShape())
		{
			shapeData = phyComp->getShape();
		}
		else
		{
			shapeData = new ShapeData();
		}
		
		//list of bounding shapes?
		vector<bool> shapeBools;
		if (boundingShapesList.empty())
		{
			//fill
			for (int x = ShapeData::BOX; x < ShapeData::CAPSULE; x++)
			{
				ShapeData::BoundingShape shapeType = (ShapeData::BoundingShape)x;
				string shape = EnumParser<ShapeData::BoundingShape>().getString(shapeType).c_str();
				boundingShapesList.push_back(shape);
			}
			if (!boundingShapesCStyleArray.empty())
				boundingShapesCStyleArray.clear();
			boundingShapesCStyleArray.reserve(boundingShapesList.size());
			for (int index = 0; index < boundingShapesList.size(); ++index)
			{
				boundingShapesCStyleArray.push_back(boundingShapesList[index].c_str());
			}
		}
		while (shapeBools.size() < ShapeData::CAPSULE)
		{
			shapeBools.push_back(false);
		}
		for (int x = ShapeData::BOX; x < ShapeData::CAPSULE; x++)
		{
			ShapeData::BoundingShape shapeType = (ShapeData::BoundingShape)x;
			//if (shapeData->boundingShape)
			{
				switch (shapeData->boundingShape)
				{
				case ShapeData::BOX:
					shapeBools[ShapeData::BOX] = true;
					break;
				case ShapeData::SPHERE:
					shapeBools[ShapeData::SPHERE] = true;
					break;
				case ShapeData::CONE:
					shapeBools[ShapeData::CONE] = true;
					break;
				case ShapeData::CYLINDER:
					shapeBools[ShapeData::CYLINDER] = true;
					break;
				case ShapeData::CAPSULE:
					shapeBools[ShapeData::CAPSULE] = true;
					break;
				default:
					break;
				}
			}
			
			string typeName = EnumParser<ShapeData::BoundingShape>().getString(shapeType);
			
			bool bWindowActive = shapeBools[x];
			
			shapeBools[x] = bWindowActive;

			
			switch (x)
			{
			case ShapeData::BOX:
				if (shapeBools[x])
					ImGui::DragFloat3("Half Extents", &shapeData->halfExtents[0], dragSpeed);
				break;
			case ShapeData::SPHERE:
				if (shapeBools[x])
					ImGui::DragFloat("Radius", &shapeData->radius, dragSpeed);
				break;
			case ShapeData::CONE:
				if (shapeBools[x])
				{
					ImGui::DragFloat("Radius", &shapeData->radius, dragSpeed);
					ImGui::DragFloat("Height", &shapeData->height, dragSpeed);
				}
				break;
			case ShapeData::CYLINDER:
				if (shapeBools[x])
					ImGui::DragFloat3("Half Extents", &shapeData->halfExtents[0], dragSpeed);
				break;
			case ShapeData::CAPSULE:
				if (shapeBools[x])
				{
					ImGui::DragFloat("Radius", &shapeData->radius, dragSpeed);
					ImGui::DragFloat("Height", &shapeData->height, dragSpeed);
				}
				break;
			default:
				break;
			}

		}
		ImGui::ListBox("BoundingShapes Available", &listbox_item_current, &boundingShapesCStyleArray[0], boundingShapesCStyleArray.size(), 4);
		shapeBools[listbox_item_current] = true;
		if (ImGui::Button("UpdateBoundingShape"))
		{
			shapeData->boundingShape = EnumParser<ShapeData::BoundingShape>().parse(boundingShapesList[listbox_item_current]);
			phyComp->buildCollisionShape(*shapeData);
		}
		phyComp->setShape(shapeData);
	
	}
	ImGui::InputFloat("restitution", &restitution, dragSpeed);
	phyComp->setRestitution(restitution);
	ImGui::DragFloat("friction", &friction, dragSpeed);
	phyComp->setFriction(friction);
	float rotationalFriction = *phyComp->getRotationalFriction();
	ImGui::InputFloat("Rotational Friction", &rotationalFriction, dragSpeed);
	phyComp->setRotationalFriction(rotationalFriction);
	ImGui::Checkbox("Is Constant Velocity", phyComp->isConstVelocity());
	glm::vec3 velo = glm::vec3(phyComp->getVelocity()->getX(), phyComp->getVelocity()->getY(), phyComp->getVelocity()->getZ());
	ImGui::DragFloat3("Velocity", &velo[0], dragSpeed);
	phyComp->setVelocity(velo.x, velo.y, velo.z);
	ImGui::PopID();
}

void DebugMenu::gameObjectsMenuLogic(int i, LogicComponent* logic)
{
	ImGui::PushID(i);
	static char scriptNameBuf[64] = "";

	if (strlen(scriptNameBuf) == 0)
	{
		strncpy_s(scriptNameBuf, logic->getScriptName().c_str(), logic->getScriptName().size());
	}

	ImGui::InputText("Script Name", scriptNameBuf, sizeof(scriptNameBuf));
	if (ImGui::Button("SetName"))
	{
		logic->setScriptName(scriptNameBuf);
		static string fullPath;
		fullPath = AssetManager::getInstance()->getScript(scriptNameBuf) + ".lua";
		logic->setScriptFullPath(fullPath.c_str());
	}
	ImGui::SameLine();
	if (ImGui::Button("LoadScript"))
	{
		if (logic->getScriptName() == "")
		{
			logic->setScriptName("default");
			static string fullPath;
			fullPath = AssetManager::getInstance()->getScript("default") + ".lua";
			logic->setScriptFullPath(fullPath.c_str());
		}
		logic->registerLuaBindings();
	}
	ImGui::PopID();
}

void DebugMenu::gameObjectsMenuTrigger(int i, CollisionTrigger * trigger)
{
	ImGui::PushID(i);
	ImGui::Indent();
	//Script
	static char scriptNameBuf[64] = "";

	if (strlen(scriptNameBuf) == 0)
	{
		strncpy_s(scriptNameBuf, trigger->getScriptName().c_str(), trigger->getScriptName().size());
	}

	ImGui::InputText("Script Name", scriptNameBuf, sizeof(scriptNameBuf));
	if (ImGui::Button("SetName"))
	{
		trigger->setScriptName(scriptNameBuf);
		static string fullPath;
		fullPath = AssetManager::getInstance()->getScript(scriptNameBuf) + ".lua";
		trigger->setScriptFullPath(fullPath.c_str());
	}
	ImGui::SameLine();
	if (ImGui::Button("LoadScript"))
	{
		trigger->loadScript();
	}
	//triggeronce
	static bool triggerOnce = trigger->isTriggerOnce();
	if (ImGui::Checkbox("TriggerOnce?", &triggerOnce))
	{
		trigger->setTriggerOnce(triggerOnce);
	}

	//ObjectBoundingShape
	{
		static ShapeData* shapeData;
		if (trigger->getShape())
		{
			shapeData = trigger->getShape();
		}
		else
		{
			shapeData = new ShapeData();
		}

		//list of bounding shapes?
		vector<bool> shapeBools;
		if (boundingShapesList.empty())
		{
			//fill
			for (int x = ShapeData::BOX; x < ShapeData::CAPSULE; x++)
			{
				ShapeData::BoundingShape shapeType = (ShapeData::BoundingShape)x;
				string shape = EnumParser<ShapeData::BoundingShape>().getString(shapeType).c_str();
				boundingShapesList.push_back(shape);
			}
			if (!boundingShapesCStyleArray.empty())
				boundingShapesCStyleArray.clear();
			boundingShapesCStyleArray.reserve(boundingShapesList.size());
			for (int index = 0; index < boundingShapesList.size(); ++index)
			{
				boundingShapesCStyleArray.push_back(boundingShapesList[index].c_str());
			}
		}
		while (shapeBools.size() < ShapeData::CAPSULE)
		{
			shapeBools.push_back(false);
		}
		for (int x = ShapeData::BOX; x < ShapeData::CAPSULE; x++)
		{
			ShapeData::BoundingShape shapeType = (ShapeData::BoundingShape)x;
			//if (shapeData->boundingShape)
			{
				switch (shapeData->boundingShape)
				{
				case ShapeData::BOX:
					shapeBools[ShapeData::BOX] = true;
					break;
				case ShapeData::SPHERE:
					shapeBools[ShapeData::SPHERE] = true;
					break;
				case ShapeData::CONE:
					shapeBools[ShapeData::CONE] = true;
					break;
				case ShapeData::CYLINDER:
					shapeBools[ShapeData::CYLINDER] = true;
					break;
				case ShapeData::CAPSULE:
					shapeBools[ShapeData::CAPSULE] = true;
					break;
				default:
					break;
				}
			}

			string typeName = EnumParser<ShapeData::BoundingShape>().getString(shapeType);

			bool bWindowActive = shapeBools[x];

			shapeBools[x] = bWindowActive;

			static float dragSpeed = 0.25f;
			switch (x)
			{
			case ShapeData::BOX:
				if (shapeBools[x])
					ImGui::DragFloat3("Half Extents", &shapeData->halfExtents[0], dragSpeed);
				break;
			case ShapeData::SPHERE:
				if (shapeBools[x])
					ImGui::DragFloat("Radius", &shapeData->radius, dragSpeed);
				break;
			case ShapeData::CONE:
				if (shapeBools[x])
				{
					ImGui::DragFloat("Radius", &shapeData->radius, dragSpeed);
					ImGui::DragFloat("Height", &shapeData->height, dragSpeed);
				}
				break;
			case ShapeData::CYLINDER:
				if (shapeBools[x])
					ImGui::DragFloat3("Half Extents", &shapeData->halfExtents[0], dragSpeed);
				break;
			case ShapeData::CAPSULE:
				if (shapeBools[x])
				{
					ImGui::DragFloat("Radius", &shapeData->radius, dragSpeed);
					ImGui::DragFloat("Height", &shapeData->height, dragSpeed);
				}
				break;
			default:
				break;
			}

		}
		ImGui::ListBox("BoundingShapes Available", &listbox_item_current, &boundingShapesCStyleArray[0], boundingShapesCStyleArray.size(), 4);
		shapeBools[listbox_item_current] = true;
		if (ImGui::Button("UpdateBoundingShape"))
		{
			shapeData->boundingShape = EnumParser<ShapeData::BoundingShape>().parse(boundingShapesList[listbox_item_current]);
			trigger->setShape(shapeData);
			trigger->buildCollisionShape();
		}
		trigger->setShape(shapeData);
	}
	//
	ImGui::PopID();
}

void DebugMenu::gameObjectsMenuTransform(int i, Transform* transform)
{
	ImGui::PushID(i);
	static float dragSpeed = 0.25f;
	static float quatDragSpeed = 0.0025f;
	static glm::quat orientationVec;
	static glm::vec3 axis;

	float transformAngle = glm::degrees(glm::angle(transform->orientation));
	axis = glm::axis(transform->orientation);

	orientationVec[0] = axis[0];
	orientationVec[1] = axis[1];
	orientationVec[2] = axis[2];
	orientationVec[3] = transformAngle;

	ImGui::DragFloat3("Position", &transform->position[0], dragSpeed);
	ImGui::DragFloat4("Orientation", &orientationVec[0], quatDragSpeed);
	transform->orientation = glm::angleAxis(glm::radians(orientationVec.w), glm::vec3(orientationVec.x, orientationVec.y, orientationVec.z));
	ImGui::DragFloat3("Scale", &transform->scale[0], dragSpeed);
	ImGui::PopID();
}

void DebugMenu::lightsMenu(int i, Light * light)
{
	ImGui::PushID(i);
	static float dragSpeed = 0.25f;
	ImGui::DragFloat3("Position", &light->pos[0], dragSpeed);
	ImGui::DragFloat3("ambient", &light->ambient[0], dragSpeed);
	ImGui::DragFloat3("diffuse", &light->diffuse[0], dragSpeed);
	ImGui::DragFloat3("specular", &light->specular[0], dragSpeed);
	ImGui::Checkbox("Enabled", &light->enabled);
	ImGui::Checkbox("Save?", &light->saved);
	ImGui::PopID();
}

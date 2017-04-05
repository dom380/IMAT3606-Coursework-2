#include "Editor/DebugMenu.h"
#include "utils/levelloader.h"
#include <utils\DirectoryReader.h>
#include <utils\FileSaver.h>

bool DebugMenu::initialised = false;
shared_ptr<DebugMenu> DebugMenu::instance;
#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))


void DebugMenu::popup(string text)
{



}

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
	popupActive = false;
	showSaveAsMenu = false;
}

void DebugMenu::update()
{
	updateMainMenu();
	updateLogic();
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
				popupActive = true;


			}
			if (ImGui::Button("SaveAs..."))
			{
				showSaveAsMenu = true;
			}

			if (ImGui::Button("Reload Scene"))
			{
				if (loadLevel(Engine::g_pEngine->getActiveScreen()->getXMLFilePath()))
				{
					Engine::g_pEngine->replaceScreen(Engine::g_pEngine->getActiveScreen()->getID());
					popupText = "Load successfully";
				}
				else
				{
					popupText = "Load FAILED";
				}
				popupActive = true;
			}

			if (ImGui::Button("Load..."))
			{
				showLoadMenu = true;
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
			if (objList.empty())
			{
				std::string path = AssetManager::getInstance()->getRootFolder(AssetManager::ResourceType::MODEL) + "*.obj";
				objList = DirectoryReader::getFilesInDirectory(path.c_str());

			}
			for (int x = 0; x < objList.size(); x++)
			{
				ImGui::PushID(x);
				if (objCreateActive.size() != objList.size())
				{
					objCreateActive.push_back(false);
				}
				if (ImGui::Button(objList[x].c_str()))
				{
					objCreateActive[x] = true;
					//showCube = true;
				}
				if (objCreateActive[x])
				{
					createObjectWindow(objList[x], x);
				}
				ImGui::PopID();
			}

			ImGui::EndMenu();
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
	if (popupActive)
	{
		ImGui::OpenPopup("Popup");
		popupActive = false;
	}
	if (ImGui::BeginPopup("Popup"))
	{
		ImGui::Text(popupText.c_str());
		ImGui::EndPopup();
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
	ImGui::Begin("GameObjects", &showGameObjects);
	shared_ptr<GameScreen> gameScreen = std::static_pointer_cast<GameScreen>(Engine::g_pEngine->getActiveScreen());
	
	/*
		Every game object from the vector is listed.
	*/
	for (int x = 0; x < gameScreen->getGameObjects().size(); x++)
	{
		//Push and PopID, useful for similarly named objects.
		ImGui::PushID(x);
		static bool showMore = false;
		char goName[50];
		auto model =  gameScreen->getComponentStore()->getComponent<ModelComponent>(gameScreen->getGameObjects()[x]->GetComponentHandle(ComponentType::MODEL), ComponentType::MODEL);
		if (model)
		{
			snprintf(goName, sizeof(goName), "GO_%s_%d", model->getId().c_str(), x);
		}
		else {
			snprintf(goName, sizeof(goName), "GO_%s_%d", "ID_ERROR", x);
		}
		//Tree node creates a tree from the game object name
		if (ImGui::TreeNode(goName))
		{
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
					snprintf(compName, sizeof(compName), "COMP_%d", i);
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
							gameObjectsMenuRigidBody();
							break;
						case ComponentType::LOGIC:
							gameObjectsMenuLogic();
							break;
						case ComponentType::TRANSFORM:
							gameObjectsMenuTransform(i, model);
							break;
						}
						ImGui::TreePop();
					}
					ImGui::PopID();
				}
			}
			ImGui::TreePop();
		}
		ImGui::PopID();
	}
	ImGui::End();
}

bool DebugMenu::saveCurrentLevel(string fileName)
{
	shared_ptr<GameScreen> gameScreen = std::static_pointer_cast<GameScreen>(Engine::g_pEngine->getActiveScreen());
	int numberOfObjectsInFile = XMLReader::GetNumberOfGameObjectsInFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument());
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
		popupActive = true;
		
	}
	ImGui::End();
}

bool DebugMenu::loadLevel(string fileName)
{
	return LevelLoader::loadLevel(Engine::g_pEngine.get(), Engine::g_pEngine->getRenderer(), Engine::g_pEngine->getInput(), fileName.c_str());
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
		popupActive = true;

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
	static float createPosition[3] = { 0.0f, 0.0f, 0.0f };
	static float createScale[3] = { 1.0f, 1.0f, 1.0f };
	static float createOrientation[3] = { 0.0f, 1.0f, 0.0f };
	ImGui::DragFloat3("Position", createPosition, 0.5f);
	ImGui::DragFloat3("Scale", createScale, 0.5f);
	ImGui::DragFloat3("Orientation", createOrientation, 0.5f);

	static std::vector<const char *> textureCStyleArray;
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
			textureCStyleArray.push_back(textureList[index].c_str());
		}
	}
	/*
		Create a selectable list for textures
	*/
	static int listbox_item_current = 1;
	ImGui::ListBox("Textures Available", &listbox_item_current, &textureCStyleArray[0], textureCStyleArray.size(), 4);
	
	if (ImGui::Button("Create"))
	{
		//Load object, overide
		shared_ptr<GameScreen> gameScreen = std::static_pointer_cast<GameScreen>(Engine::g_pEngine->getActiveScreen());
		shared_ptr<Transform> transform = std::make_shared<Transform>();
		transform->position = glm::vec3(createPosition[0], createPosition[1], createPosition[2]);
		transform->orientation = glm::vec3(createOrientation[0], createOrientation[1], createOrientation[2]);
		transform->scale = glm::vec3(createScale[0], createScale[1], createScale[2]);
		std::pair<string, string> objInfo(objName, textureCStyleArray[listbox_item_current]) ;
		LevelLoader::loadGameObject(Engine::g_pEngine->getRenderer(), gameScreen, objInfo, transform);
	}
	ImGui::PopID();
	ImGui::End();
}

void DebugMenu::render()
{
	ImGui::Render();
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

void DebugMenu::gameObjectsMenuRigidBody()
{
}

void DebugMenu::gameObjectsMenuLogic()
{
}

void DebugMenu::gameObjectsMenuTransform(int i, ModelComponent* model)
{
	ImGui::PushID(i);
	float dragSpeed = 0.25f;
	ImGui::DragFloat3("Position", &model->getTransform()->position[0], dragSpeed);
	ImGui::DragFloat3("Orientation", &model->getTransform()->orientation[0], dragSpeed);
	ImGui::DragFloat3("Scale", &model->getTransform()->scale[0], dragSpeed);
	ImGui::PopID();
}

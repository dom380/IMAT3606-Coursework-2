#include "Editor/DebugMenu.h"
#include "utils/levelloader.h"
#include <utils\DirectoryReader.h>
#include <utils\FileSaver.h>

bool DebugMenu::initialised = false;
shared_ptr<DebugMenu> DebugMenu::instance;



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
			if (objModelList.empty())
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
	for (int x = 0; x < objCreateActive.size(); x++)
	{
		if (objCreateActive[x])
		{
			createObjectWindow(objList[x], x);
		}
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
		auto model =  gameScreen->getComponentStore()->getComponent<ModelComponent>(gameScreen->getGameObjects()[x]->GetComponentHandle(ComponentType::ComponentTypes::MODEL), ComponentType::ComponentTypes::MODEL);
		if (model)
		{
			//snprintf(goName, sizeof(goName), "GO_%s", model->getId().c_str());
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
			for (int i = ComponentType::ComponentTypes::MODEL; i < ComponentType::ComponentTypes::COMPONENT_TYPE_COUNT; i++)
			{
				ComponentType::ComponentTypes cType = static_cast<ComponentType::ComponentTypes>(i);
					
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
	//string levelPath = AssetManager::getInstance()->getRootFolder(AssetManager::ResourceType::LEVEL) + "Level1" + ".xml";
	int numberOfObjectsInFile = XMLReader::GetNumberOfGameObjectsInFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument());
	/*
	Every game object from the vector is listed.
	*/
	for (int x = 0; x < gameScreen->getGameObjects().size(); x++)
	{
		FileSaver::UpdateFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument(), x, gameScreen->getGameObjects()[x], gameScreen);
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

bool DebugMenu::saveAsLevel(string fileName)
{
	return false;
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
	
	//Texture Name code
	string objTextureName;
	static char Buffer[50] = "";
	ImGui::InputText("Texture Name", Buffer, sizeof(Buffer));

	if (ImGui::Button("Create"))
	{
		//Load object, overide
		shared_ptr<GameScreen> gameScreen = std::static_pointer_cast<GameScreen>(Engine::g_pEngine->getActiveScreen());
		shared_ptr<Transform> transform = std::make_shared<Transform>();
		transform->position = glm::vec3(createPosition[0], createPosition[1], createPosition[2]);
		transform->orientation = glm::vec3(createOrientation[0], createOrientation[1], createOrientation[2]);
		transform->scale = glm::vec3(createScale[0], createScale[1], createScale[2]);
		objTextureName = Buffer;
		std::pair<string, string> objInfo(objName, objTextureName) ;
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
	if (ImGui::Button("Toggle"))
	{
		model->toggleDrawing();
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
	ImGui::DragFloat3("Position", &model->getTransform()->position[0], 0.25f);
	ImGui::DragFloat3("Orientation", &model->getTransform()->orientation[0], 0.25f);
	ImGui::DragFloat3("Scale", &model->getTransform()->scale[0], 0.25f);
	ImGui::PopID();
}

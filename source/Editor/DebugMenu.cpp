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
				if (saveCurrentLevel())
				{
					
					popupText = "Saved successfully";
				} 
				else
				{
					popupText = "Save FAILED";
				}
				popupActive = true;
	

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
						case MODEL:
							gameObjectsMenuModel(i, model);
							break;
						case ANIMATION:
							gameObjectsMenuAnimation();
							break;
						case RIGID_BODY:
							gameObjectsMenuRigidBody();
							break;
						case LOGIC:
							gameObjectsMenuLogic();
							break;
						case TRANSFORM:
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

bool DebugMenu::saveCurrentLevel()
{
	shared_ptr<GameScreen> gameScreen = std::static_pointer_cast<GameScreen>(Engine::g_pEngine->getActiveScreen());
	//string levelPath = AssetManager::getInstance()->getRootFolder(AssetManager::ResourceType::LEVEL) + "Level1" + ".xml";
	/*
	Every game object from the vector is listed.
	*/
	for (int x = 0; x < gameScreen->getGameObjects().size(); x++)
	{
		auto model = gameScreen->getComponentStore()->getComponent<ModelComponent>(gameScreen->getGameObjects()[x]->GetComponentHandle(ComponentType::MODEL), ComponentType::MODEL);

		/*
		Inside the gameobject is a list of the components
		*/
		for (int i = ComponentType::MODEL; i < ComponentType::COMPONENT_TYPE_COUNT; i++)
		{
			ComponentType cType = static_cast<ComponentType>(i);

			if (gameScreen->getGameObjects()[x]->HasComponent(cType))
			{
				char compName[14];
				snprintf(compName, sizeof(compName), "COMP_%d", i);
				//Each component has editable stuff

				switch (cType)
				{
				case MODEL:

					break;
				case ANIMATION:

					break;
				case RIGID_BODY:

					break;
				case LOGIC:

					break;
				case TRANSFORM:
					string transformInnerElement;
					for (int z = 0; z < 3; z++)
					{
						switch (z)
						{
						case 0:
							transformInnerElement = "position";
							for (int y = 0; y < 3; y++)
							{
								std::ostringstream ss;
								ss << model->getTransform()->position[y];
								switch (y)
								{
								case 0:
									//Save func
									if (!FileSaver::UpdateFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument(), x, model->getId(), transformInnerElement, "x", string(ss.str())))
										return false;
									break;
								case 1:
									//Save func

									if (!FileSaver::UpdateFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument(), x, model->getId(), transformInnerElement, "y", string(ss.str())))
										return false;
									break;
								case 2:
									//Save func

									if (!FileSaver::UpdateFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument(), x, model->getId(), transformInnerElement, "z", string(ss.str())))
										return false;
									break;
								}

							}

							break;
						case 1:
							transformInnerElement = "scale";
							for (int y = 0; y < 3; y++)
							{
								std::ostringstream ss;
								ss << model->getTransform()->scale[y];
								switch (y)
								{
								case 0:
									//Save func

									if (!FileSaver::UpdateFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument(), x, model->getId(), transformInnerElement, "x", string(ss.str())))
										return false;
									break;
								case 1:
									//Save func

									if (!FileSaver::UpdateFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument(), x, model->getId(), transformInnerElement, "y", string(ss.str())))
										return false;
									break;
								case 2:
									//Save func

									if (!FileSaver::UpdateFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument(), x, model->getId(), transformInnerElement, "z", string(ss.str())))
										return false;
									break;
								}

							}
							break;
						case 2:

							transformInnerElement = "orientation";
							for (int y = 0; y < 4; y++)
							{
								std::ostringstream ss;
								ss << model->getTransform()->orientation[y];
								switch (y)
								{
								case 0:
									//Save func

									if (!FileSaver::UpdateFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument(), x, model->getId(), transformInnerElement, "x", string(ss.str())))
										return false;
									break;
								case 1:
									//Save func

									if (!FileSaver::UpdateFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument(), x, model->getId(), transformInnerElement, "y", string(ss.str())))
										return false;
									break;
								case 2:
									//Save func

									if (!FileSaver::UpdateFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument(), x, model->getId(), transformInnerElement, "z", string(ss.str())))
										return false;
									break;
								case 4:
									if (!FileSaver::UpdateFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument(), x, model->getId(), transformInnerElement, "w", string(ss.str())))
										return false;
									break;
								}

							}
							break;
						}
					}

					break;
				}
			}
		}


	}
	return FileSaver::SaveFile(Engine::g_pEngine->getActiveScreen()->getXMLDocument(), Engine::g_pEngine->getActiveScreen()->getXMLFilePath());
}

bool DebugMenu::saveAsLevel()
{
	return false;
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
		/*tinyxml2::XMLDocument doc;
		string filePath = AssetManager::getInstance()->getRootFolder(AssetManager::ResourceType::LEVEL) + "level1" + ".xml";
		tinyxml2::XMLError check = doc.LoadFile(filePath.c_str());
		if (check != tinyxml2::XML_SUCCESS) {
			std::cerr << "Failed to load file" << filePath << std::endl;

		}
		tinyxml2::XMLElement* screenElement = doc.FirstChildElement("screen");
		tinyxml2::XMLElement* gameObjElement = screenElement->FirstChildElement("gameObjects")->FirstChildElement();*/
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

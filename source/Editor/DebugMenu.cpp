#include "Editor/DebugMenu.h"
#include "utils/levelloader.h"

bool DebugMenu::initialised = false;
shared_ptr<DebugMenu> DebugMenu::instance;
float DebugMenu::createPosition[3] { 0.0f, 0.0f, 0.0f };
float DebugMenu::createOrientation[3]{ 0.0f, 1.0f, 0.0f };
float DebugMenu::createScale[3]{ 1.0f, 1.0f, 1.0f };

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
		if (ImGui::BeginMenu("Debug"))
		{
			if (ImGui::Button("Objects"))
			{
				showGameObjects = true;
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::Button("Cube"))
			{
				showCube = true;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void DebugMenu::updateLogic()
{
	if (showCube)
	{
		createCubeMenu();
	}
	if (showGameObjects)
	{


	}
}

void DebugMenu::debugGameObjectsMenu()
{
	ImGui::Begin("GameObjects", &showGameObjects);
	//for (int x = 0;  x < Engine::g_pEngine->getActiveScreen()->)
	ImGui::End();
}

void DebugMenu::createCubeMenu()
{
	ImGui::Begin("CreateCube", &showCube);
	//static float position[3] = { 0.0f, 0.0f, 0.0f };
	ImGui::DragFloat3("Position", createPosition, 0.5f);
	ImGui::DragFloat3("Scale", createScale, 0.5f);
	ImGui::DragFloat3("Orientation", createOrientation, 0.5f);
	if (ImGui::Button("Create"))
	{
		tinyxml2::XMLDocument doc;
		string filePath = AssetManager::getInstance()->getRootFolder(AssetManager::ResourceType::LEVEL) + "level1" + ".xml";
		tinyxml2::XMLError check = doc.LoadFile(filePath.c_str());
		if (check != tinyxml2::XML_SUCCESS) {
			std::cerr << "Failed to load file" << filePath << std::endl;

		}
		tinyxml2::XMLElement* screenElement = doc.FirstChildElement("screen");
		tinyxml2::XMLElement* gameObjElement = screenElement->FirstChildElement("gameObjects")->FirstChildElement();
		//Load cube, overide
		shared_ptr<GameScreen> gameScreen = std::static_pointer_cast<GameScreen>(Engine::g_pEngine->getActiveScreen());
		shared_ptr<Transform> transform = std::make_shared<Transform>();
		transform->position = glm::vec3(createPosition[0], createPosition[1], createPosition[2]);
		transform->orientation = glm::vec3(createOrientation[0], createOrientation[1], createOrientation[2]);
		transform->scale = glm::vec3(createScale[0], createScale[1], createScale[2]);
		LevelLoader::loadGameObject(Engine::g_pEngine->getRenderer(), gameScreen, "cube.obj", transform);

	}
	ImGui::End();
}

void DebugMenu::render()
{
	ImGui::Render();
}



#include "Screens\GameScreen.h"

GameScreen::GameScreen(shared_ptr<Graphics>& renderer, shared_ptr<Input>& input, shared_ptr<Physics>& physics, shared_ptr<Camera> camera) /*:
	robot(std::make_shared<Robot>(AssetManager::getInstance()->getShader(std::pair<string, string>("colour.vert", "colour.frag"))))*/
{
	this->renderer = renderer;
	this->physics = physics;
	componentStore = std::make_shared<ComponentStore>();
	//shared_ptr<EngineCamera> engineCam = std::make_shared<EngineCamera>(renderer->getWidth(), renderer->getHeight(), 45.0f, glm::vec3(58, 42, -68), glm::vec3(0, 1, 0), glm::vec3(-0.8,-0.42,0.42));
	shared_ptr<EngineCamera> engineCam = std::make_shared<EngineCamera>(renderer->getWidth(), renderer->getHeight(), 45.0f, glm::vec3(0.0,3.0,15.0), glm::vec3(0, 1, 0), glm::vec3(0.0, 0.0, -1));
	cameras.push_back(engineCam);
	/*
		Read cameraDistanceToPlayer from file?
		Perhaps a mechanics file, for all things regardless of level.
	*/
	cameraDistanceToPlayer = glm::vec3(0, -6, -15);
	float aspect = 45.0f;
	shared_ptr<FollowCamera> playerCam = std::make_shared<FollowCamera>(renderer->getWidth(), renderer->getHeight(), aspect, cameraDistanceToPlayer);
	playerCam->lookAt(glm::vec3(0.0, 0.0, -1.0));
	cameras.push_back(playerCam);
#ifndef NDEBUG
	glm::quat quat; quat.y = 1.0f; quat.w = 0.0f;
	shared_ptr<Transform> textPos = std::make_shared<Transform>(glm::vec3(30, 30, 0), glm::vec3(0.5, 0.5, 0.5), quat);
	frameTime = std::make_shared<TextBox>("Frame Time: 0", *AssetManager::getInstance()->getFont("arial.ttf", renderer), textPos, renderer);
	uiElements.push_back(frameTime);
#endif
	this->input = input;
	//this->input->registerKeyListener(robot);
	this->input->registerKeyListener(engineCam);
	this->input->registerMouseListener(engineCam);

	activeCamera = 0;
	activeCamera++;
	UIManager::getInstance()->update();

	//Sound stuff may move later only ever need one of these
	//XML??
	listener = listener->Instance();
	sounds = sounds->Instance();
	listener->setPosition(0.0, 0.0, 0.0);
	listener->setDirection(1.0, 0.0, 0.0);
	sounds->GetSound(0)->play();
	sounds->setLooping(true, 0);

}

void GameScreen::show()
{
	Engine::g_pEngine->getEngineState()->switchEngineMode();
	physics->setPaused(false);
	for (auto go : gameObjects)
	{
		if (go->HasComponent(ComponentType::CONTROLLER))
		{
			auto control = go->getComponent<ControllerComponent>(ComponentType::CONTROLLER);
			control->setCamera(cameras.at(1));
		}
	}
}

void GameScreen::update(double dt, double currentTime)
{
	Timer testTimer;
	testTimer.start();
#ifndef NDEBUG
	if(!timer.isRunning())
		timer.start();
#endif
//Message* robotLocMsg = new LocationMessage(robot->getPosition());
	std::vector<std::pair<int, LogicComponent>>* logicComponents = componentStore->getAllComponents<LogicComponent>(ComponentType::LOGIC);
	std::vector<std::pair<int, LogicComponent>>::iterator it;
	for (it = logicComponents->begin(); it != logicComponents->end(); ++it)
	{
		if (it->first != -1)
		{
			it->second.update(dt);
			//it->second.RecieveMessage(robotLocMsg);
		}
	}
//delete robotLocMsg;

	auto physicsComponents = componentStore->getAllComponents<PhysicsComponent>(ComponentType::RIGID_BODY);
	for (auto it = physicsComponents->begin(); it != physicsComponents->end(); ++it)
	{
		if (it->first != -1) //Check if handle is still used.
		{
			it->second.update(dt);
		}
	}
	auto controllers = componentStore->getAllComponents<ControllerComponent>(ComponentType::CONTROLLER);
	for (auto it = controllers->begin(); it != controllers->end(); ++it)
	{
		if (it->first != -1) //Check if handle is still used.
		{
			it->second.update(dt);
		}
	}
	auto ms = testTimer.getElapsedTimeMilliSec();
	testTimer.stop();
//	delete robotLocMsg;
	auto animComponents = componentStore->getAllComponents<AnimatedModelComponent>(ComponentType::ANIMATION);
	for (auto it = animComponents->begin(); it != animComponents->end(); ++it)
	{
		it->second.update(currentTime);
	}
}

void GameScreen::render()
{
	shared_ptr<Camera> camera = cameras.at(activeCamera);
	//robot->DrawRobot(camera->getView(), camera->getProjection());
	Message* renderMsg = new RenderMessage(camera, lightingBufferId, lightingBlockId);
	std::vector<std::pair<int,ModelComponent>>* models = componentStore->getAllComponents<ModelComponent>(ComponentType::MODEL);
	std::vector<std::pair<int, ModelComponent>>::iterator it;
	for (it = models->begin(); it != models->end(); ++it)
	{
		if(it->first != -1)
			it->second.RecieveMessage(renderMsg);
	}

	std::vector<std::pair<int, AnimatedModelComponent>>* animations = componentStore->getAllComponents<AnimatedModelComponent>(ComponentType::ANIMATION);
	std::vector<std::pair<int, AnimatedModelComponent>>::iterator animIt;
	for (animIt = animations->begin(); animIt != animations->end(); ++animIt)
	{
		if (animIt->first != -1)
			//animIt->second.update(0.07f);
			animIt->second.RecieveMessage(renderMsg);
	}

	delete renderMsg;

	for (auto ui : uiElements) {
		ui->render();
	}
	
#ifndef NDEBUG
	double elapsedTime = timer.getElapsedTimeMilliSec();
	string frameText = "Frame Time: " + std::to_string( elapsedTime ) + "ms";
	frameTime->updateText(frameText);
	timer.stop();
#endif
}

void GameScreen::resize(int width, int height)
{
}

shared_ptr<ComponentStore> GameScreen::getComponentStore()
{
	return componentStore;
}

void GameScreen::dispose()
{
	physics->setPaused(true);
	for (shared_ptr<Camera> camera : cameras) {
		input->removeMouseListener(camera);
		input->removeKeyListener(camera);
	}
	input->setKeyFocus(nullptr);
	for (auto element : uiElements)
	{
		input->removeKeyListener(element->getButton());
		input->removeMouseListener(element->getButton());
	}
	gameObjects.clear();
	lights.clear();
	cameras.clear();
	uiElements.clear();
	componentStore.reset();
}

void GameScreen::addLight(Light light)
{
	lights.push_back(light);
}

void GameScreen::addGameObject(shared_ptr<GameObject> gameObj)
{
	gameObjects.push_back(gameObj);
}

void GameScreen::updateLighting()
{
	std::shared_ptr<Shader>shader = AssetManager::getInstance()->getShader(std::pair<string, string>("phong.vert", "phong.frag"));
	renderer->bufferLightingData(lights, shader, lightingBufferId, lightingBlockId);
}

bool GameScreen::handle(MouseEvent& event)
{
	//NOP
	return false;
}

bool GameScreen::handle(KeyEvent& event)
{
	if (event.type == KeyEventType::KEY_PRESSED)
	{
		if (event.key == KeyCodes::C)//c
		{
			activeCamera++;
			if (activeCamera >= cameras.size()) activeCamera = 0;
			return true;
		}
	}
	if (activeCamera == 0)
	{
		cameras.at(0)->handle(event);
	}
	else if (activeCamera == 1)
	{
		//robot->handle(event);
	}
	auto phyPtr = std::dynamic_pointer_cast<BulletPhysics>(physics);
	if(phyPtr != nullptr) phyPtr->handle(event);
	return false;
}

void GameScreen::updateScore(int amountToAdd)
{
	currentScore += amountToAdd;
	for (shared_ptr<UIElement> uiE : uiElements)
	{
		shared_ptr<TextBox> textbox = dynamic_pointer_cast<TextBox>(uiE);
		if (textbox->getID() == string("score_string"))
		{
			textbox->updateText("Gold Collected: " + std::to_string(currentScore));
		}
	}
}

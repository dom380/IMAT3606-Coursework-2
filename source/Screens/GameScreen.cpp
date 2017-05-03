#include "Screens\GameScreen.h"

GameScreen::GameScreen(shared_ptr<Graphics>& renderer, shared_ptr<Input>& input, shared_ptr<Physics>& physics, std::vector<shared_ptr<Camera>> cameras)
{
	screenType = type::GAMESCREEN;
	this->renderer = renderer;
	this->physics = physics;
	componentStore = std::make_shared<ComponentStore>();
	for (auto cam : cameras)
	{
		this->cameras.push_back(cam);
	}

#ifndef NDEBUG
	glm::quat quat; quat.y = 1.0f; quat.w = 0.0f;
	shared_ptr<Transform> textPos = std::make_shared<Transform>(glm::vec3(30, 30, 0), glm::vec3(0.5, 0.5, 0.5), quat);
	frameTime = std::make_shared<TextBox>("Frame Time: 0", *AssetManager::getInstance()->getFont("arial.ttf", renderer), textPos, renderer);
	frameTime->setSavable(false);
	uiElements.push_back(frameTime);
#endif
	this->input = input;

	activeCamera = 0;
	if(this->cameras.size() > 1) activeCamera++;
	UIManager::getInstance()->update();

	//Sound stuff may move later only ever need one of these
	//XML??

	listener = Listener::Instance();
	sounds = SoundComponent::Instance();
	sounds->loadSound("resources/audio/Background Music/menu.wav");

	listener->setPosition(0.0, 0.0, 0.0);
	listener->setDirection(1.0, 0.0, 0.0);
	sounds->GetSound(0)->play();
	sounds->setLooping(true, 0);

}

void GameScreen::show()
{
	Engine::g_pEngine->getEngineState()->setEngineMode(EngineMode::EDITOR);
	physics->setPaused(false);
	std::shared_ptr<FollowCamera> playerCam;
	for (auto camera : cameras)
	{
		if (camera->getCameraType() == Camera::CameraClass::FOLLOW && camera->getId() == "PLAYER_CAM")
		{
			playerCam = std::dynamic_pointer_cast<FollowCamera>(camera);
			break;
		}
	}
	if (playerCam)
	{
		for (auto go : gameObjects)
		{
			if (go->HasComponent(ComponentType::CONTROLLER))
			{
				auto control = go->getComponent<ControllerComponent>(ComponentType::CONTROLLER);
				control->setCamera(playerCam);
			}
		}
	}
}

void GameScreen::update(double dt, double currentTime)
{
#ifndef NDEBUG
	if(!timer.isRunning())
		timer.start();
#endif
	std::vector<std::pair<int, LogicComponent>>* logicComponents = componentStore->getAllComponents<LogicComponent>(ComponentType::LOGIC);
	std::vector<std::pair<int, LogicComponent>>::iterator it;
	for (it = logicComponents->begin(); it != logicComponents->end(); ++it)
	{
		if (it->first != -1)
		{
			it->second.update(dt);
		}
	}

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
	auto animComponents = componentStore->getAllComponents<AnimatedModelComponent>(ComponentType::ANIMATION);
	for (auto it = animComponents->begin(); it != animComponents->end(); ++it)
	{
		it->second.update(currentTime);
	}
}

void GameScreen::render()
{
	shared_ptr<Camera> camera = cameras.at(activeCamera);
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
		element->dispose();
	}
	componentStore->dispose();
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
		else if (event.key == KeyCodes::P)
		{
			auto phyPtr = std::dynamic_pointer_cast<BulletPhysics>(physics);
			if (phyPtr != nullptr) phyPtr->handle(event);
		}
	}
	if (activeCamera == 0) //Redirect event to Engine camera.
	{
		cameras.at(0)->handle(event);
	}
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

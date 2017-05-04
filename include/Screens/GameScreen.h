#pragma once
#ifndef GAMESCREEN_H
#define GAMESCREEN_H
#include <Screens\Screen.h>
#include <memory>
using std::shared_ptr;
#include <Camera\Camera.h>
#include <Camera\PerspectiveCamera.h>
#include <Camera\FollowCamera.h>
#include <Camera\EngineCamera.h>
#include <Input\Input.h>
#include <vector>
using std::vector;
#include <ComponentStore.h>
#include <Robot.h>
#include <GUI\UIManager.h>
#include <GUI\TextBox.h>
#include <GameObject.h>
#include <Components\Message.h>
#include <Components\LocationMessage.h>
#include <Components\RenderMessage.h>
#include <AssetManager.h>
#include <Physics\Physics.h>
#include "Components/AnimatedModelComponent.h"
#include "Audio\Listener.h"
#include "Audio\SoundComponent.h"

#ifndef NDEBUG
#include <utils\Timer.h>
#endif
class Screen;
class GameObject;
class ComponentStore;
class LogicComponent; //Forward declare a number of classes.

/*
	The gameplay implementaion of the Screen Class.
	Implements EventListener.
*/
class GameScreen : public Screen, public EventListener
{
public:
	/*
		Constructor
		shared_ptr<Graphics>& renderer, Pointer to the graphics system.
		shared_ptr<Input>& input, Pointer to the input system.
		std::vector<shared_ptr<Camera>> cameras, Vector of pointers to camera.
	*/
	GameScreen(shared_ptr<Graphics>& renderer, shared_ptr<Input>& input, shared_ptr<Physics>& physics, std::vector<shared_ptr<Camera>> cameras);
	~GameScreen() {};

	/*
		Unpauses the physics simulation.
	*/
	void show();
	/*
		Update this screen's objects.
		double dt, Current time step.
	*/
	void update(double dt, double currentTime);
	/*
		Render this screen.
	*/
	void render();
	/*
		Resize this screen.
		int width, The new width.
		int height, The new height.
	*/
	void resize(int width, int height);

	/*
		Returns this screens component store.
	*/
	shared_ptr<ComponentStore> getComponentStore();

	/*
		Clean up resources.
	*/
	void dispose();
	/*
		Add a light.
	*/
	void addLight(Light light);
	/*
		Add a GameObject
	*/
	void addGameObject(shared_ptr<GameObject> gameObj);
	/*
		Gets the vector of gameobjects.
	*/
	vector<shared_ptr<GameObject>> getGameObjects()
	{
		return gameObjects;
	}
	/*
		Calls to the graphics system to update the lighting buffer.
		This method must be called after any change to the lighting.
	*/
	void updateLighting();
	/*
		Increments the score and updates the score text by the specified amount.
		int amountToAdd, The number of points to add to the current score.
	*/
	void updateScore(int amountToAdd, string idToUpdate);

	/*
		Empty implementation of Mouse Event handeling
	*/
	bool  handle(MouseEvent& event);
	/*
		Switches the camera if 'C' pressed.
	*/
	bool  handle(KeyEvent& event);
private:
	shared_ptr<ComponentStore> componentStore;
	vector<shared_ptr<GameObject>> gameObjects;
	shared_ptr<Input> input;
	shared_ptr<Graphics> renderer;
	shared_ptr<Physics> physics;
	int activeCamera = 0;
	vector<shared_ptr<Camera>> cameras;
	vector<Light> lights;
	unsigned int lightingBlockId = -1;
	unsigned int lightingBufferId = -1;
	int currentScore = 0;
	glm::vec3 cameraDistanceToPlayer;
	//Sound Stuff May need to move later
	Listener* listener;
	SoundComponent* sounds;
#ifndef NDEBUG //If debugging display how long the frame took in ms.
	Timer timer;
	shared_ptr<TextBox> frameTime;
#endif
};

#endif // !GAMESCREEN_H


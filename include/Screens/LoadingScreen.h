#pragma once
#ifndef LOADINGSCREEN_H
#define LOADINGSCREEN_H
#include <Engine.h>
#include <Screens\Screen.h>
#include <GUI\TextBox.h>
#include <utils\LevelLoader.h>
#include <AssetManager.h>
#include <Graphics\Transform.h>
#include <memory>
#include <string>
#include <atomic>
#include <thread>

/*
	Implementation of the Screen class that loads another screen in the background.
*/
class LoadingScreen : public Screen
{
private:
	//private memebers
	std::atomic<bool> loading = true;
	std::atomic<bool> succeded = false;
	std::thread backgroundThread;
	TextBox loadingText;
	Engine* gameEngine;
	std::string levelId;
	double time = 0.0;
public:
	/*
		Constructor.
		shared_ptr<Window> window, Pointer to the window system.
		Engine* engine, Pointer to the core engine.
		std::shared_ptr<Graphics> renderer, Pointer to the graphics system.
		shared_ptr<Input> input, Pointer to the input system.
		std::string levelToLoad, Name of the level to load. Must be the same as a level descriptor XML file.
		Creates a background thread that begins to load the assets of the requested level.
	*/
	LoadingScreen(shared_ptr<Window> window, Engine* engine, std::shared_ptr<Graphics> renderer, shared_ptr<Input> input, std::string levelToLoad)
	{
		shared_ptr<Transform> transform = std::make_shared<Transform>();
		transform->orientation.y = 1.0f; transform->orientation.w = 0.0f;
		transform->position.x = 30; transform->position.y = 500;
		loadingText = TextBox("Loading", *AssetManager::getInstance()->getFont("arial.ttf", renderer), transform, renderer);
		levelId = levelToLoad;
		gameEngine = engine;
		backgroundThread = std::thread([this, window, engine, renderer, input, levelToLoad] {
			window->switchBackgroundContext();
			string path = AssetManager::getInstance()->getRootFolder(AssetManager::ResourceType::LEVEL) + levelToLoad + ".xml";
			this->succeded = LevelLoader::loadLevel(engine, renderer, input, path.c_str());
			this->loading = false;
		});
	}
	/*
		Switches to the new Screen if background thread has finished.
		If not, update the Loading text every second.
		double dt, Time passed since last update.
	*/
	void update(double dt)
	{
		if (!loading)
		{
			backgroundThread.join();
			if (succeded) {
				gameEngine->replaceScreen(levelId);
			} 
			else {
				gameEngine->replaceScreen(gameEngine->getInitialScreenId());
			}
		}
		time += dt;
		if (time > 1.0 && time < 2.0)
		{
			loadingText.updateText("Loading.");
		}
		else if (time > 2.0 && time < 3.0)
		{
			loadingText.updateText("Loading..");
		}
		else if (time > 3.0 && time < 4.0)
		{
			loadingText.updateText("Loading...");
		}
		else if (time > 4.0)
		{
			loadingText.updateText("Loading");
			time = 0;
		}
	}

	/*
		Render the loading text.
	*/
	void render()
	{
		loadingText.render();
	}
	/*
		Resize the window
	*/
	void resize(int width, int height) {};
	/*
		Clean up any resources.
	*/
	void dispose() {};
	/*
		Empty implementation.
	*/
	void addTextBox(shared_ptr<TextBox> textbox) {};

};

#endif // !LOADINGSCREEN_H
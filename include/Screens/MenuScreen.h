#pragma once
#ifndef MENUSCREEN_H
#define MENUSCREEN_H
#include <Screens\Screen.h>
#include <GUI\Button.h>
#include <GUI\TextBox.h>
#include <AssetManager.h>
#include <Engine.h>
#include <memory>
using std::shared_ptr;
#include <vector>
using std::vector;
class Engine;

/*
	A GUI/Menu system implementation of the Screen class.
	TODO - Refactor to be component based.
*/
class MenuScreen : public Screen {
public:
	/*
		Constructor.
	*/
	MenuScreen() {};
	~MenuScreen() {};
	/*
		Constructor.
		shared_ptr<Graphics>& graphics, Pointer to the graphics system.
		Engine* engine, Pointer to the core engine.
	*/
	MenuScreen(shared_ptr<Graphics>& graphics, Engine* engine);
	/*
		Empty implementation of show.
	*/
	void show();

	/*
		Empty implementation of update.
	*/
	void update(double dt);
	/*
		Calls to the graphics system to render the Text and Buttons.
	*/
	void render();
	/*
		Resizes the screen.
		int width, The new screen width.
		int height The new screen height.
	*/
	void resize(int width, int height);
	/*
		Clean up resources.
	*/
	void dispose();
	/*
		Adds a new Button to the screen.
		shared_ptr<Button> button, Button to add.
	*/
	void addButton(shared_ptr<Button> button);
	/*
		Adds a new TextBox to the screen.
		shared_ptr<TextBox> textbox, TextBox to add.
	*/
	void addTextBox(shared_ptr<TextBox> textbox);

private:
	//private memebers.
	shared_ptr<Graphics> graphics;
	Engine* engine;
	vector<shared_ptr<Button>> buttons;
	vector<shared_ptr<TextBox>> textBoxes;

};

#endif // !SCREENMENU_H


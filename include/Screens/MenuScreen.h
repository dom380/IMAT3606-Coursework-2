#pragma once
#ifndef MENUSCREEN_H
#define MENUSCREEN_H
#include <Screens\Screen.h>
#include <GUI\UIManager.h>
#include <GUI\Button.h>
#include <GUI\TextBox.h>
#include <AssetManager.h>
#include <memory>
using std::shared_ptr;
#include <vector>
using std::vector;

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
	MenuScreen(shared_ptr<Graphics>& graphics);
	/*
		Empty implementation of show.
	*/
	void show();

	/*
		Empty implementation of update.
	*/
	void update(double dt, double currentTime);
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
	
private:
	//private memebers.
	shared_ptr<Graphics> graphics;
};

#endif // !SCREENMENU_H


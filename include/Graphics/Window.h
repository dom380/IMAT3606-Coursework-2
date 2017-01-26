#pragma once
#ifndef WINDOW_H
#define WINDOW

/*
	Abstract class representing the game window.
*/
class Window
{
public:
	/*
		Constructor.
		int width, The width of the screen in pixels.
		int height, The height of hte screen in pixels.
	*/
	Window(int width, int height)
	{
		this->width = width;
		this->height = height;
	}
	/*
		Initialisation method to be called before use.
		Implementations should perform one off and time consuming initalisation here
		rather than the constructor.
	*/
	virtual bool inititalise() = 0;
	/*
		Display the current framebuffer on screen.
		This may swap buffers or flush the buffer depending on the implementation.
		Called at the end of each loop by the Engine.
	*/
	virtual void display() = 0;
	/*
		Poll system events.
		Called at the end of each loop by the Engine.
	*/
	virtual void pollEvents() = 0;
	/*
		Check if the window has signalled to exit.
		Returns true if program should exit.
	*/
	virtual bool shouldExit() = 0;
	/*
		Switch the underlying graphics context to the background context if supported by implementation.
		Allows for loading of graphical data on a background thread.
	*/
	virtual void switchBackgroundContext() = 0;
	/*
		Called on program exit.
		Implementations should clear up any resources and destroy graphics contexts here.
	*/
	virtual void close() = 0;
protected:
	int width;
	int height;
};

#endif // !WINDOW_H

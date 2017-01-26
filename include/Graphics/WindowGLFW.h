#pragma once
#ifndef WINDOWGLFW_H
#define WINDOWGLFW_H
#include "Window.h"
#include "GL/glfw3.h"
#include <InputGLFW.h>

/*
	GLFW implementation of the Window class.
	Should be used with the OpenGL Renderer implementation.
*/
class WindowGLFW : public Window
{
public:
	/*
		Constructor.
		int width, The width of the screen in pixels.
		int height, The height of hte screen in pixels.
		shared_ptr<InputGLFW> input, Pointer to the input handler containing key and mouse callbacks.
	*/
	WindowGLFW(int width, int height, shared_ptr<InputGLFW> input);
	/*
		Initialise GLFW and creates the main window and OpenGL context.
		It also creates a second, background OpenGL context that shares data
		with the main to allow background loading.
	*/
	bool inititalise();
	/*
		Checks if the window should close.
	*/
	bool shouldExit();
	/*
		Signals GLFW to swap the frame buffers.
	*/
	void display();
	/*
		Signa;s GLFW to poll events.
	*/
	void pollEvents();
	/*
		Sets the background OpenGL context to the currently active context.
		Should only be called on a separete worker thread.
	*/
	void switchBackgroundContext();
	/*
		Destroys the main and background OpenGL Contexts.
	*/
	void close();
private:
	GLFWwindow* window;
	GLFWwindow* offscreen_context;
	shared_ptr<InputGLFW> input;
};

#endif // !WINDOWGLFW_H

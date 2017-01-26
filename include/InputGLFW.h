#pragma once
#ifndef INPUTGLFW_H
#define INPUTGLFW_H
#include "Input.h"
#include <gl\glfw3.h>

/*
	GLFW implementation of the Input system.
*/
class InputGLFW : public Input
{
public:
	/*
		Default constructor
	*/
	InputGLFW();
	/*
		GLFW Mouse movement callback function. 
		Constructs and passes a MouseEvent to all listeners
	*/
	void mouseMovementCallback(GLFWwindow* window, double xpos, double ypos);
	/*
		GLFW Mouse movement callback function.
		Constructs and passes a MouseEvent to all listeners
	*/
	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	/*
		GLFW keyboard callback function.
		Constructs and passes a KeyEvent to all listeners
	*/
	void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

};

#endif // !INPUTGLFW_H

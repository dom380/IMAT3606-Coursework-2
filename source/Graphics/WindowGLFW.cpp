#include "Graphics\WindowGLFW.h"

#include <Editor\imgui\imgui.h>

WindowGLFW::WindowGLFW(int width, int height, shared_ptr<InputGLFW> input) : Window(width,height)
{
	this->input = input;
}

bool WindowGLFW::inititalise()
{
	// Initialize GLFW
	if (!glfwInit()) return false;

	//	 Select OpenGL 4.0 with a forward compatible core profile.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, false);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	glfwWindowHint(GLFW_DOUBLEBUFFER, true);

	// Open the window
	window = glfwCreateWindow(width, height, "IMAT3606-CourseWork", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, input.get()); //Force GLFW to store a pointer to input handler. This allows use to use c++ member functions as callbacks in a round about way.
	auto keyCallback = [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		static_cast<InputGLFW*>(glfwGetWindowUserPointer(window))->keyboardCallback(window, key, scancode, action, mods);
	};
	auto mouseMoveCallback = [](GLFWwindow * window, double xpos, double ypos)
	{
		static_cast<InputGLFW*>(glfwGetWindowUserPointer(window))->mouseMovementCallback(window, xpos, ypos);
	};
	auto mouseCickCallback = [](GLFWwindow * window, int button, int action, int mods)
	{
		static_cast<InputGLFW*>(glfwGetWindowUserPointer(window))->mouseButtonCallback(window, button, action, mods);
	};

	auto charCallback = [](GLFWwindow * window, unsigned int c)
	{
		static_cast<InputGLFW*>(glfwGetWindowUserPointer(window))->charCallback(window, c);
	};
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCharCallback(window, charCallback);
	glfwSetMouseButtonCallback(window, mouseCickCallback);
	glfwSetCursorPosCallback(window, mouseMoveCallback);

	//Create an invisible separate context to allow background loading.
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, false);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, false);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	offscreen_context = glfwCreateWindow(640, 480, "", NULL, window);

	ImGuiGLFWHandler = std::make_shared<ImguiGLFWHandler>();
	if (!ImGuiGLFWHandler->init(this))
	{
		return false;
	}
	

	return true;
}

bool WindowGLFW::shouldExit()
{
	return glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE);
}

void WindowGLFW::display()
{
	glfwSwapBuffers(window);
}

void WindowGLFW::update()
{
	ImGuiGLFWHandler->newFrame();
}

void WindowGLFW::pollEvents()
{
	glfwPollEvents();
}

void WindowGLFW::switchBackgroundContext()
{
	glfwMakeContextCurrent(offscreen_context);
}

void WindowGLFW::close()
{
	ImGuiGLFWHandler->shutdown();
	// Close window and terminate GLFW
	glfwTerminate();
}

GLFWwindow * WindowGLFW::getWindow()
{
	return window;
}
#ifndef NDEBUG
shared_ptr<ImguiGLFWHandler> WindowGLFW::getImGuiHandler()
{
	return ImGuiGLFWHandler;
}
#endif
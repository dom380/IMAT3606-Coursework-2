#include "..\include\InputGLFW.h"
#include "Graphics\WindowGLFW.h"
#ifndef NDEBUG
#include <Editor\imgui\ImguiGLFWHandler.h>
#include <Editor\imgui\imgui.h>
#endif

InputGLFW::InputGLFW()
{
}

void InputGLFW::mouseMovementCallback(GLFWwindow * window, double xpos, double ypos)
{
	MouseEvent e(MouseEventType::MOUSE_MOVE, xpos, ypos);
	for (shared_ptr<EventListener> listener : mouseSubs) {
		listener->handle(e);
	}
}

void InputGLFW::mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
#ifndef NDEBUG
	WindowGLFW* gw = (WindowGLFW*) Engine::g_pEngine->GetWindow().get();
	gw->getImGuiHandler()->imGuiMouseButtonCallback(Engine::g_pEngine->GetWindow(), button, action, mods);
#endif
	double cursorPosX, cursorPosY;
	glfwGetCursorPos(window, &cursorPosX, &cursorPosY);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		MouseEvent e(MouseEventType::LEFT_CLICK, cursorPosX, cursorPosY);
		for (shared_ptr<EventListener> listener : mouseSubs) {
			listener->handle(e);
		}
	}
	
}

void InputGLFW::keyboardCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
#ifndef NDEBUG
	WindowGLFW* gw = (WindowGLFW*)Engine::g_pEngine->GetWindow().get(); 
	gw->getImGuiHandler()->imGuiKeyCallback(Engine::g_pEngine->GetWindow(), key, scancode, action, mods);
#endif
	KeyEventType type;
	switch (action)
	{
	case GLFW_PRESS:
		type = KeyEventType::KEY_PRESSED;
		break;
	case GLFW_REPEAT:
		type = KeyEventType::KEY_REPEATED;
		break;
	case GLFW_RELEASE:
		type = KeyEventType::KEY_RELEASED;
		break;
	default:
		type = KeyEventType::KEY_PRESSED;
		break;
	}
	KeyEvent e(type, key, mods);
	for (shared_ptr<EventListener> listener : keySubs) {
		listener->handle(e);
	}
}

void InputGLFW::charCallback(GLFWwindow * window, unsigned int c)
{
#ifndef NDEBUG
	WindowGLFW* gw = (WindowGLFW*)Engine::g_pEngine->GetWindow().get(); 
	gw->getImGuiHandler()->imGuiCharCallback(Engine::g_pEngine->GetWindow(), c);
#endif
}

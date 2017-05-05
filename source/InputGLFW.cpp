#include "..\include\Input\InputGLFW.h"
#include "Graphics\WindowGLFW.h"

#include <Editor\imgui\ImguiGLFWHandler.h>
#include <Editor\imgui\imgui.h>


InputGLFW::InputGLFW()
{
}

void InputGLFW::mouseMovementCallback(GLFWwindow * window, double xpos, double ypos)
{
	MouseEvent e(MouseEventType::MOUSE_MOVE, xpos, ypos);
	if (mouseFocus)
	{
		mouseFocus->handle(e);
	}
	else
	{
		for (shared_ptr<EventListener> listener : mouseSubs)
		{
			listener->handle(e);
		}
	}
}

void InputGLFW::mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{

	WindowGLFW* gw = (WindowGLFW*) Engine::g_pEngine->GetWindow().get();
	gw->getImGuiHandler()->imGuiMouseButtonCallback(Engine::g_pEngine->GetWindow(), button, action, mods);
	if (gw->getImGuiHandler()->imGuiHasInputFocus())
		return;

	double cursorPosX, cursorPosY;
	glfwGetCursorPos(window, &cursorPosX, &cursorPosY);
	//Create Mouse Event
	MouseEvent e(MouseEventType::LEFT_CLICK, (MouseActionType)action, cursorPosX, cursorPosY);
	if (button == GLFW_MOUSE_BUTTON_LEFT) 
	{
		e = MouseEvent(MouseEventType::LEFT_CLICK, (MouseActionType) action, cursorPosX, cursorPosY);
		
	} 
	else if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		e = MouseEvent(MouseEventType::RIGHT_CLICK, (MouseActionType)action, cursorPosX, cursorPosY);
	}
	//Notify listeners
	if (mouseFocus)
	{
		mouseFocus->handle(e);
	} 
	else
	{
		for (shared_ptr<EventListener> listener : mouseSubs)
		{
			if (listener->handle(e)) return;
		}
	}
}

void InputGLFW::keyboardCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{

	WindowGLFW* gw = (WindowGLFW*)Engine::g_pEngine->GetWindow().get(); 
	gw->getImGuiHandler()->imGuiKeyCallback(Engine::g_pEngine->GetWindow(), key, scancode, action, mods);
	if (gw->getImGuiHandler()->imGuiHasInputFocus())
		return;

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
	KeyEvent e(type, (KeyCodes)key, mods);
	if (keyFocus)
	{
		keyFocus->handle(e);
	}
	else
	{
		for (shared_ptr<EventListener> listener : keySubs) {
			if (listener->handle(e)) return;
		}
	}
}

void InputGLFW::charCallback(GLFWwindow * window, unsigned int c)
{
	WindowGLFW* gw = (WindowGLFW*)Engine::g_pEngine->GetWindow().get(); 
	gw->getImGuiHandler()->imGuiCharCallback(Engine::g_pEngine->GetWindow(), c);
	if (gw->getImGuiHandler()->imGuiHasInputFocus())
		return;
}

KeyEventType InputGLFW::getKeyState(KeyCodes key)
{
	WindowGLFW* gw = (WindowGLFW*)Engine::g_pEngine->GetWindow().get();
	auto state = glfwGetKey(gw->getWindow(), key);
	if (state == GLFW_PRESS)
		return KeyEventType::KEY_PRESSED;
	else if (state == GLFW_REPEAT)
		return KeyEventType::KEY_REPEATED;
	else 
		return KeyEventType::KEY_RELEASED;
}

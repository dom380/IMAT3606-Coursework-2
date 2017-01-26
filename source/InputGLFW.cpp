#include "..\include\InputGLFW.h"

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

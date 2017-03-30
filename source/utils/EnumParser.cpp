#include <utils\EnumParser.h>
#include <Engine.h>
#include <utils\OnClickFunctions.h>
#include <Camera\Camera.h>
#include <Components\ComponentType.h>
#include <Input.h>

EnumParser<Engine::GraphicsContext>::EnumParser()
{
	map["OPEN_GL"] = Engine::GraphicsContext::OPEN_GL;
	map["open_gl"] = Engine::GraphicsContext::OPEN_GL;
}

EnumParser<MouseEventType>::EnumParser()
{
	map["LEFT_CLICK"] = MouseEventType::LEFT_CLICK;
	map["left_click"] = MouseEventType::LEFT_CLICK;
	map["MOUSE_MOVE"] = MouseEventType::MOUSE_MOVE;
	map["mouse_move"] = MouseEventType::MOUSE_MOVE;
	map["RIGHT_CLICK"] = MouseEventType::RIGHT_CLICK;
	map["right_click"] = MouseEventType::RIGHT_CLICK;
}

EnumParser<KeyEventType>::EnumParser()
{
	map["KEY_PRESSED"] = KeyEventType::KEY_PRESSED;
	map["key_pressed"] = KeyEventType::KEY_PRESSED;
	map["KEY_RELEASED"] = KeyEventType::KEY_RELEASED;
	map["key_released"] = KeyEventType::KEY_RELEASED;
	map["KEY_REPEATED"] = KeyEventType::KEY_REPEATED;
	map["key_repeated"] = KeyEventType::KEY_REPEATED;
}

EnumParser<OnClickFunctions::FunctionType>::EnumParser()
{
	map["SWITCH_SCREEN"] = OnClickFunctions::FunctionType::SWITCH_SCREEN;
	map["switch_screen"] = OnClickFunctions::FunctionType::SWITCH_SCREEN;
	map["REPLACE_SCREEN"] = OnClickFunctions::FunctionType::REPLACE_SCREEN;
	map["replace_screen"] = OnClickFunctions::FunctionType::REPLACE_SCREEN;
	map["EXIT"] = OnClickFunctions::FunctionType::EXIT;
	map["exit"] = OnClickFunctions::FunctionType::EXIT;
	map["DO_NOTHING"] = OnClickFunctions::FunctionType::DO_NOTHING;
	map["do_nothing"] = OnClickFunctions::FunctionType::DO_NOTHING;
}

EnumParser<Camera::CameraClass>::EnumParser()
{
	map["PERSPECTIVE"] = Camera::CameraClass::PERSPECTIVE;
	map["perspective"] = Camera::CameraClass::PERSPECTIVE;
	map["ORTHOGRAPHIC"] = Camera::CameraClass::ORTHOGRAPHIC;
	map["orthographic"] = Camera::CameraClass::ORTHOGRAPHIC;
}

EnumParser<ComponentType>::EnumParser()
{
	map["MODEL"] = ComponentType::MODEL;
	map["model"] = ComponentType::MODEL;
	map["ANIMATION"] = ComponentType::ANIMATION;
	map["animation"] = ComponentType::ANIMATION;
	map["RIGID_BODY"] = ComponentType::RIGID_BODY;
	map["rigid_body"] = ComponentType::RIGID_BODY;
	map["LOGIC"] = ComponentType::LOGIC;
	map["logic"] = ComponentType::LOGIC;
	map["TRANSFORM"] = ComponentType::TRANSFORM;
	map["transform"] = ComponentType::TRANSFORM;

	strmap[ComponentType::MODEL] = "model";
	strmap[ComponentType::ANIMATION] = "animation";
	strmap[ComponentType::RIGID_BODY] = "rigid_body";
	strmap[ComponentType::LOGIC] = "logic";
	strmap[ComponentType::TRANSFORM] = "transform";
}

EnumParser<Input::InputImpl>::EnumParser()
{
	map["GLFW"] = Input::InputImpl::GLFW;
	map["glfw"] = Input::InputImpl::GLFW;
	map["GLUT"] = Input::InputImpl::GLUT;
	map["glut"] = Input::InputImpl::GLUT;
}
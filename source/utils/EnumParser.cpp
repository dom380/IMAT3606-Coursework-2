#include <utils\EnumParser.h>
#include <Engine.h>
#include <Camera\Camera.h>
#include <Components\ComponentType.h>
#include <GUI\UIType.h>
#include <Input\Input.h>
#include <Physics\Physics.h>
#include <Physics\ShapeData.h>
#include <GameObjectTag.h>

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
	map["TRIGGER"] = ComponentType::TRIGGER;
	map["trigger"] = ComponentType::TRIGGER;

	strmap[ComponentType::MODEL] = "model";
	strmap[ComponentType::ANIMATION] = "animation";
	strmap[ComponentType::RIGID_BODY] = "rigid_body";
	strmap[ComponentType::LOGIC] = "logic";
	strmap[ComponentType::TRANSFORM] = "transform";
	strmap[ComponentType::TRIGGER] = "trigger";
}

EnumParser<UIType>::EnumParser()
{
	map["text"] = UIType::TEXT;
	map["texture"] = UIType::TEXTURE;

	strmap[UIType::TEXT] = "text";
	strmap[UIType::TEXTURE] = "texture";
}

EnumParser<Input::InputImpl>::EnumParser()
{
	map["GLFW"] = Input::InputImpl::GLFW;
	map["glfw"] = Input::InputImpl::GLFW;
	map["GLUT"] = Input::InputImpl::GLUT;
	map["glut"] = Input::InputImpl::GLUT;
}

EnumParser<Physics::PhysicsImpl>::EnumParser()
{
	map["BULLET"] = Physics::PhysicsImpl::BULLET;
	map["bullet"] = Physics::PhysicsImpl::BULLET;
}

EnumParser<ShapeData::BoundingShape>::EnumParser()
{
	map["BOX"] = ShapeData::BoundingShape::BOX;
	map["box"] = ShapeData::BoundingShape::BOX;
	map["SPHERE"] = ShapeData::BoundingShape::SPHERE;
	map["sphere"] = ShapeData::BoundingShape::SPHERE;
	map["CONE"] = ShapeData::BoundingShape::CONE;
	map["cone"] = ShapeData::BoundingShape::CONE;
	map["CYLINDER"] = ShapeData::BoundingShape::CYLINDER;
	map["cylinder"] = ShapeData::BoundingShape::CYLINDER;
}

EnumParser<GameObjectTag>::EnumParser()
{
	map["PLAYER"] = GameObjectTag::PLAYER;
	map["player"] = GameObjectTag::PLAYER;
	map["ENEMY"] = GameObjectTag::ENEMY;
	map["enemy"] = GameObjectTag::ENEMY;
	map["PHY_OBJ"] = GameObjectTag::PHY_OBJ;
	map["phy_obj"] = GameObjectTag::PHY_OBJ;
	map["UNKNOWN"] = GameObjectTag::UNKNOWN;
	map["unknown"] = GameObjectTag::UNKNOWN;
}
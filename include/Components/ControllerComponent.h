#pragma once
#ifndef CONTROLLERCOMPONENT_H
#define CONTROLLERCOMPONENT_H
#include <GameObject.h>
#include "Component.h"
#include <Physics\Physics.h>
#include <Physics\BulletActerController.h>
#include <Input\Input.h>
#include <memory>
class ControllerComponent : public Component
{
public:
	ControllerComponent(std::shared_ptr<Physics> physics, std::weak_ptr<GameObject> owner, ShapeData shape);
	void update(double dt);
	void RecieveMessage(Message* msg);
private:
	std::weak_ptr<GameObject> owner;
	std::shared_ptr<Physics> physics;
	std::shared_ptr<BulletActerController> controller;
	std::shared_ptr<Input> input;
	btVector3 upDir;
	void pollInput();
	void updateTransform(Transform* transformPtr);
};

#endif // !CONTROLLERCOMPONENT_H

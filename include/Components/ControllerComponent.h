#pragma once
#ifndef CONTROLLERCOMPONENT_H
#define CONTROLLERCOMPONENT_H
#include <GameObject.h>
#include "Component.h"
#include <Physics\Physics.h>
#include <Physics\BulletActerController.h>
#include <Input\Input.h>
#include <memory>
#include <Camera\Camera.h>
class ControllerComponent : public Component
{
public:
	ControllerComponent(std::shared_ptr<Physics> physics, std::weak_ptr<GameObject> owner, ShapeData shape);
	void update(double dt);
	void RecieveMessage(Message* msg);
	void setCamera(std::shared_ptr<Camera> camera);
	void setMovementSpeed(float speed);
	void setGravity(float value);
	void setGravity(float x, float y, float z);
private:
	std::weak_ptr<GameObject> owner;
	std::shared_ptr<Physics> physics;
	std::shared_ptr<BulletActerController> controller;
	std::shared_ptr<Input> input;
	std::shared_ptr<Camera> camera;
	btVector3 upDir;
	btQuaternion frontDir = btQuaternion::getIdentity();
	float movementSpeed = 0.1f;
	void pollInput();
	void updateTransform(Transform* transformPtr);
	void calcDirection(const btVector3& walkDir);
};

#endif // !CONTROLLERCOMPONENT_H

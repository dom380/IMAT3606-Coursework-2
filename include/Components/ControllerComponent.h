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
	ControllerComponent(std::shared_ptr<Physics> physics, std::weak_ptr<GameObject> owner, ShapeData shape, float yOffset, float jumpRayVal = 0.13, bool flip = false);
	void update(double dt);
	void RecieveMessage(Message* msg);
	void setCamera(std::shared_ptr<Camera> camera);
	void setMovementSpeed(float speed);
	void setGravity(float value);
	void setGravity(float x, float y, float z);
	void setTransform(Transform* transform);
	void setJumpForce(float force);
	void setWorldFront(float x, float y, float z);
	void dispose();
private:
	std::weak_ptr<GameObject> owner;
	std::shared_ptr<Physics> physics;
	std::shared_ptr<BulletActerController> controller;
	std::shared_ptr<Input> input;
	std::shared_ptr<Camera> camera;
	btVector3 upDir, worldFront, interalFront = btVector3(0.0, 0.0, -1.0);
	btQuaternion frontDir = btQuaternion::getIdentity();
	float movementSpeed = 0.1f;
	float offset = 0.0f;
	float angleOffset = 0.0f;
	float jumpForce = 10.0f;
	bool flip = true;
	void pollInput();
	void calcDirection(const btVector3& walkDir);
	void updateTransform(Transform* transformPtr);
};

#endif // !CONTROLLERCOMPONENT_H

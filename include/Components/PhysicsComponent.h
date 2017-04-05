#pragma once
#ifndef PHYSICSCOMPONENT_H
#define PHYSICSCOMPONENT_H
#include "Component.h"
#include <GameObject.h>
#include "btBulletDynamicsCommon.h"
#include <Graphics\ModelData.h>
#include <memory>

class PhysicsComponent : public Component
{
public:
	PhysicsComponent();
	PhysicsComponent(std::weak_ptr<GameObject> owner, std::shared_ptr<ModelData> mesh, float mass, bool convex);
	btRigidBody* getBody();
private:
	std::weak_ptr<GameObject> owner;
	float mass;
	btRigidBody* body;
	btCollisionShape* shape;
	btVector3 localInertia = btVector3(0.0, 0.0, 0.0);

	void buildCollisionShape(std::shared_ptr<ModelData> mesh, bool convex);
};

#endif // !PHYSICSCOMPONENT_H


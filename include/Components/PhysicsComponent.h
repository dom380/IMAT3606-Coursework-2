#pragma once
#ifndef PHYSICSCOMPONENT_H
#define PHYSICSCOMPONENT_H
#include "Component.h"
#include <GameObject.h>
#include "btBulletDynamicsCommon.h"
#include <Graphics\ModelData.h>
#include <memory>
#include <Physics\BulletPhysics.h>
class Physics;

class PhysicsComponent : public Component
{
public:
	PhysicsComponent(std::shared_ptr<Physics> &physics, std::weak_ptr<GameObject> owner, std::shared_ptr<ModelData> mesh, float mass, bool convex);
	void update(double dt);
	void RecieveMessage(Message* msg);
	btRigidBody* getBody();
private:
	std::weak_ptr<GameObject> owner;
	float mass;
	btRigidBody* body;
	btCollisionShape* shape;
	btVector3 localInertia = btVector3(0.0, 0.0, 0.0);

	void buildCollisionShape(std::shared_ptr<ModelData>& mesh, bool& convex, glm::vec3& scale);
};

#endif // !PHYSICSCOMPONENT_H


#pragma once
#ifndef BULLETPHYSICS_H
#define BULLETPHYSICS_H
#include "Physics.h"
#include <Components\PhysicsComponent.h>
#include "btBulletDynamicsCommon.h"
#include <GUI\EventListener.h>

class BulletPhysics : public Physics, public EventListener
{
public:
	BulletPhysics() {};
	~BulletPhysics();
	void init();
	void update(double dt);
	void setGravity(double x, double y, double z);
	void addBody(btRigidBody* body);
	void addBody(PhysicsComponent comp);

	//Event Listener methods
	void handle(MouseEvent event);
	void handle(KeyEvent event);

private:
	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* world;
};

#endif // !BULLETPHYSICS_H

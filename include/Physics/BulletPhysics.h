#pragma once
#ifndef BULLETPHYSICS_H
#define BULLETPHYSICS_H
#include "Physics.h"
#include "btBulletDynamicsCommon.h"

class BulletPhysics : public Physics
{
public:
	BulletPhysics() {};
	~BulletPhysics();
	void init();
private:
	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* world;
};

#endif // !BULLETPHYSICS_H

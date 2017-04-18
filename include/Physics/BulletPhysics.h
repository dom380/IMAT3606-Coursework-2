#pragma once
#ifndef BULLETPHYSICS_H
#define BULLETPHYSICS_H
#include <Engine.h>
#include "Physics.h"
#include <Components\PhysicsComponent.h>
#include <Components\CollisionTrigger.h>
#include "btBulletDynamicsCommon.h"
#include <GUI\EventListener.h>
#include <utils\UUID\guid.h>
#include <map>
class CollisionTrigger;
/*
	The bullet 2.8x implementation of the Physics system
*/
class BulletPhysics : public Physics, public EventListener
{
public:
	/*
		Default constructor
	*/
	BulletPhysics() {};
	/*
		Deconstructor. Deletes dynamic world and collision handler.
	*/
	~BulletPhysics();
	/*
		Initialises the dynamic world and collision detection.
	*/
	void init();
	/*
		Updates the physics simulation.
		double dt - The current time step.
	*/
	void update(double dt);
	/*
		Set's the worlds gravity vector.
		double x - The X component of the gravity vector.
		double y - The Y component of the gravity vector.
		double z - The Z component of the gravity vector.
	*/
	void setGravity(double x, double y, double z);
	/*
		Add the specified rigid body to the world.
	*/
	void addBody(btRigidBody* body);
	/*
		Add the specified PhysicsComponent to the dynamic world.
	*/
	void addBody(PhysicsComponent comp);

	/*
		Add the specified CollisionTrigger to the dymaic world.
	*/
	void addTrigger(std::shared_ptr<CollisionTrigger> trigger);

	std::vector<std::shared_ptr<CollisionTrigger>> getCollisionTriggers();

	/*
		Per step callback function for the simulation.
	*/
	static void tickCallback(btDynamicsWorld *world, btScalar timeStep);

	//Event Listener methods
	/*
		stub method
	*/
	void handle(MouseEvent event);
	/*
		Pauses and unpauses the physics simulation on 'P' key press.
	*/
	void handle(KeyEvent event);

private:
	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* world;
	std::vector<std::shared_ptr<CollisionTrigger>> collisionTriggers;
};

#endif // !BULLETPHYSICS_H

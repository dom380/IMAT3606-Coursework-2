#include "..\..\include\Physics\BulletPhysics.h"

BulletPhysics::~BulletPhysics()
{
	delete world;
	delete solver;
	delete broadphase;
	delete dispatcher;
	delete collisionConfiguration;
}

void BulletPhysics::init()
{
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	broadphase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	world->setGravity(btVector3(0.0f, -20.0f, 0.0f));
}

void BulletPhysics::update(double dt)
{
	if (!paused) 
	{
		world->stepSimulation(dt);
	}
}

void BulletPhysics::setGravity(double x, double y, double z)
{
	world->setGravity(btVector3(x, y, z));
}

void BulletPhysics::addBody(btRigidBody* body)
{
	world->addRigidBody(body);
}

void BulletPhysics::addBody(PhysicsComponent comp)
{
	world->addRigidBody(comp.getBody());
}

void BulletPhysics::handle(MouseEvent event)
{
	//no op
}

void BulletPhysics::handle(KeyEvent event)
{
	if (event.type == KeyEventType::KEY_RELEASED && event.key == 80) //p
	{
		paused = !paused;
	}
}

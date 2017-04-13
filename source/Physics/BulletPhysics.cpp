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
	broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	world->setInternalTickCallback(BulletPhysics::tickCallback, this, true);
}

void BulletPhysics::update(double dt)
{
	if (!paused) 
	{
		world->stepSimulation(btScalar(dt));
	}
}

void BulletPhysics::setGravity(double x, double y, double z)
{
	world->setGravity(btVector3(btScalar(x), btScalar(y), btScalar(z)));
}

void BulletPhysics::addBody(btRigidBody* body)
{
	world->addRigidBody(body);
}

void BulletPhysics::addBody(PhysicsComponent comp)
{
	world->addRigidBody(comp.getBody());
}

void BulletPhysics::addTrigger(std::shared_ptr<CollisionTrigger> trigger)
{
	world->addCollisionObject(trigger->getBody());
	collisionTriggers.push_back(trigger);
}

std::vector<std::shared_ptr<CollisionTrigger>> BulletPhysics::getCollisionTriggers()
{
	return collisionTriggers;
}

void BulletPhysics::tickCallback(btDynamicsWorld * world, btScalar timeStep)
{
	/*
		Exploiting the global engine ptr to get back to the actual BulletPhysics instance from this static method.
		Cast should be safe as the callback is only registered if a BulletPhysics instance was initialised in the first place.
	*/
	std::shared_ptr<BulletPhysics> physicsPtr = std::dynamic_pointer_cast<BulletPhysics>(Engine::g_pEngine->getPhysics());
	if (physicsPtr != nullptr) //Can't hurt to check though.
	{
		auto collisionTriggers = physicsPtr->getCollisionTriggers();
		for (std::shared_ptr<CollisionTrigger> trigger : collisionTriggers)
		{
			if (trigger == nullptr)
				continue;
			if (trigger->isTriggerOnce() && trigger->isTriggered())
			{
				continue;
			}
			else 
			{
				int numOfOverlaps = trigger->getBody()->getNumOverlappingObjects();
				for (int i = 0; i < numOfOverlaps; ++i)
				{
					auto object = trigger->getBody()->getOverlappingPairs().at(i);
					//Pretty unsafe but bullet forces this. 
					auto owner = static_cast<GameObject*>(object->getUserPointer());
					if (owner != nullptr)
					{
						trigger->trigger(owner);
					}
				}
			}		
		}
	}
	//Now iterate over the contact manifolds and notify any colliding objects
	auto dispatcher = world->getDispatcher();
	int numOfManifolds = dispatcher->getNumManifolds();
	for (int i = 0; i < numOfManifolds; ++i)
	{
		auto contactManifold = dispatcher->getManifoldByIndexInternal(i);
		if (contactManifold->getNumContacts() > 0)
		{
			//Get the colliding objects
			const btCollisionObject* objA = contactManifold->getBody0();
			const btCollisionObject* objB = contactManifold->getBody1();
			//We only care about dynamic objects colliding. Use a collision trigger if that is needed.
			if (objA->isStaticObject() || objB->isStaticObject()) 
			{
				continue;
			}
			else
			{
				/*TODO - Get both GameObjects from user ptr and send a contact message. 
				Again, need some kind of tagging system so we know what the objects are meant to be
				Anything I do here may get called multiple times every frame, need some way of limiting*/
			}
		}
	}
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


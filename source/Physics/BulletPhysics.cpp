#include "..\..\include\Physics\BulletPhysics.h"

BulletPhysics::~BulletPhysics()
{
	world->getCollisionObjectArray().clear();
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
#ifndef NDEBUG
	Timer timer;
	timer.start();
#endif
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
				//Check if they're actually colliding and not just close
				bool collided = false;
				for (int j = 0; j < contactManifold->getNumContacts(); ++j)
				{
					auto cp = contactManifold->getContactPoint(j);
					if (cp.getDistance() <= 0.f)
					{
						collided = true;
						break;
					}
				}
				if (!collided) continue;
				
				auto gameObjA = static_cast<GameObject*>(objA->getUserPointer());
				auto gameObjB = static_cast<GameObject*>(objB->getUserPointer());

				Message* collisionMessageA = new CollisionMessage(gameObjB, timeStep);
				Message* collisionMessageB = new CollisionMessage(gameObjA, timeStep);
				auto logicComp = gameObjA->getLogic(); //Send collision message to ObjectA 
				if (logicComp != nullptr)
					logicComp->RecieveMessage(collisionMessageA);
				logicComp = gameObjB->getLogic();  //Send collision message to ObjectB
				if (logicComp != nullptr)
					logicComp->RecieveMessage(collisionMessageB);
				delete collisionMessageA;
				delete collisionMessageB;

			}
		}
	}
#ifndef NDEBUG
	auto updateTime = timer.getElapsedTime();
	timer.stop();
#endif
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


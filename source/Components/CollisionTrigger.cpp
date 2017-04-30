#include "..\..\include\Components\CollisionTrigger.h"
#include <utils\Utilities.h>

CollisionTrigger::CollisionTrigger(std::shared_ptr<Physics> &physics, std::weak_ptr<GameObject>& owner, ShapeData& boundingShape, const char* scriptFile, bool triggerOnce) : Component(ComponentType::TRIGGER), triggerFunc(luaState)
{
	this->owner = owner;
	this->triggerOnce = triggerOnce;
	this->triggered = false;
	//Retrieve the transform of the game object
	btTransform transform;
	auto sp = owner.lock();
	auto tp = sp != nullptr ? sp->getComponent<Transform>(ComponentType::TRANSFORM) : nullptr;
	transform.setOrigin(btVector3(tp->position.x, tp->position.y, tp->position.z));
	transform.setRotation(btQuaternion(tp->orientation.x, tp->orientation.y, tp->orientation.z, tp->orientation.w));

	//Build Collision shape
	switch (boundingShape.boundingShape)
	{
	case ShapeData::BoundingShape::BOX:
		shape = new btBoxShape(btVector3(boundingShape.halfExtents.x, boundingShape.halfExtents.y, boundingShape.halfExtents.z));
		break;
	case ShapeData::BoundingShape::SPHERE:
		shape = new btSphereShape(boundingShape.radius);
		break;
	case ShapeData::BoundingShape::CONE:
		shape = new btConeShape(boundingShape.radius, boundingShape.height);
		break;
	case ShapeData::BoundingShape::CYLINDER:
		shape = new btCylinderShape(btVector3(boundingShape.halfExtents.x, boundingShape.halfExtents.y, boundingShape.halfExtents.z));
		break;
	}

	body = new btGhostObject();
	body->setCollisionShape(shape);
	body->setWorldTransform(transform);
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	script = scriptFile;
	auto splitPath = Utilities::splitFilePath(scriptFile);
	scriptName = Utilities::removeExtension(splitPath.at(splitPath.size() - 1));
	auto scriptEngine = ScriptEngine::getInstance();
	scriptEngine->loadScript(script, scriptName);
	triggerFunc = scriptEngine->getFunction(scriptName, "trigger");
}

CollisionTrigger::CollisionTrigger(std::shared_ptr<Physics>& physics, std::weak_ptr<GameObject>& owner, ShapeData & boundingShape, std::string scriptFile, bool triggerOnce)
	: CollisionTrigger(physics, owner, boundingShape, scriptFile.c_str(), triggerOnce)
{
	//Delegation constructor
}

void CollisionTrigger::update(double dt)
{
}

void CollisionTrigger::RecieveMessage(Message * msg)
{
}

btGhostObject * CollisionTrigger::getBody()
{
	return body;
}

void CollisionTrigger::trigger(std::shared_ptr<GameObject> collider)
{
	try
	{
		if (triggerFunc.isFunction()) //If the script contains a valid function
		{
			triggerFunc();
			if (triggerOnce && !triggered)
				triggered = true;
		}
	}
	catch (luabridge::LuaException e)
	{
		//Todo - better error handling here
		std::cout << e.what() << std::endl;
		throw std::runtime_error(e.what());
	}
}

void CollisionTrigger::trigger(GameObject * collider)
{
	try
	{
		if (triggerFunc.isFunction()) //If the script contains a valid function
		{
			if (triggerOnce && !triggered)
			{
				this->triggered = true;
			}
			else if (triggerOnce && triggered)
			{
				return;
			}
			triggerFunc(*collider);
		}
	}
	catch (luabridge::LuaException e)
	{
		//Todo - better error handling here
		std::cout << e.what() << std::endl;
		throw std::runtime_error(e.what());
	}
}

void CollisionTrigger::setTransform(Transform transform)
{
	btTransform btTran;
	btTran.setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
	btTran.setRotation(btQuaternion(transform.orientation.x, transform.orientation.y, transform.orientation.z, transform.orientation.w));
	body->setWorldTransform(btTran);
}

bool CollisionTrigger::isTriggerOnce()
{
	return triggerOnce;
}

bool CollisionTrigger::isTriggered()
{
	return triggered;
}

void CollisionTrigger::dispose()
{
	std::shared_ptr<BulletPhysics> bullet = std::dynamic_pointer_cast<BulletPhysics>(Engine::g_pEngine->getPhysics());
	if (bullet)
	{
		bullet->removeTrigger(this);
	}
}

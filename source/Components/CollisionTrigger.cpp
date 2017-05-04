#include "..\..\include\Components\CollisionTrigger.h"
#include <utils\Utilities.h>

CollisionTrigger::CollisionTrigger() : Component(ComponentType::TRIGGER), triggerFunc(luaState), params(luaState)
{
	triggerOnce = true;
	triggered = false;
	shapeData = nullptr;
	body = nullptr;
}

CollisionTrigger::CollisionTrigger(std::shared_ptr<Physics> &physics, std::weak_ptr<GameObject>& owner, ShapeData& boundingShape, const char* scriptFile, bool triggerOnce, luabridge::LuaRef params) : Component(ComponentType::TRIGGER), triggerFunc(luaState), params(luaState)
{
	this->owner = owner;
	this->triggerOnce = triggerOnce;
	this->triggered = false;
	this->params = params;
	shapeData = new ShapeData(boundingShape);

	script = scriptFile;
	auto splitPath = Utilities::splitFilePath(scriptFile);
	scriptName = Utilities::removeExtension(splitPath.at(splitPath.size() - 1));
	
	init();
}

CollisionTrigger::CollisionTrigger(std::shared_ptr<Physics>& physics, std::weak_ptr<GameObject>& owner, ShapeData & boundingShape, std::string scriptFile, bool triggerOnce, luabridge::LuaRef params)
	: CollisionTrigger(physics, owner, boundingShape, scriptFile.c_str(), triggerOnce, params)
{
	//Delegation constructor
}

void CollisionTrigger::init()
{
	btTransform transform;
	auto sp = owner.lock();
	auto tp = sp != nullptr ? sp->getComponent<Transform>(ComponentType::TRANSFORM) : nullptr;
	transform.setOrigin(btVector3(tp->position.x, tp->position.y, tp->position.z));
	transform.setRotation(btQuaternion(tp->orientation.x, tp->orientation.y, tp->orientation.z, tp->orientation.w));
	
	buildCollisionShape();

	body = new btGhostObject();
	body->setCollisionShape(shape);
	body->setWorldTransform(transform);
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	loadScript();
}

void CollisionTrigger::setOwner(std::weak_ptr<GameObject> passedOwner)
{
	owner = passedOwner;
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
			if (triggerOnce && !triggered)
			{
				this->triggered = true;
			}
			else if (triggerOnce && triggered)
			{
				return;
			}
			triggerFunc(*collider, Engine::g_pEngine.get(), params);
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
			auto sp = owner.lock();
			LogicComponent* logic = nullptr;
			if (sp)
			{
				logic = sp->getLogic();
			}
			triggerFunc(*collider, Engine::g_pEngine.get(), logic, params);
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

void CollisionTrigger::setTriggerOnce(bool isTriggerOnce)
{
	triggerOnce = isTriggerOnce;
}

bool CollisionTrigger::isTriggerOnce()
{
	return triggerOnce;
}

bool CollisionTrigger::isTriggered()
{
	return triggered;
}

void CollisionTrigger::loadScript()
{
	auto scriptEngine = ScriptEngine::getInstance();
	scriptEngine->loadScript(script, scriptName);
	triggerFunc = scriptEngine->getFunction(scriptName, "trigger");
}


string CollisionTrigger::getScriptName()
{
	return scriptName;
}

void CollisionTrigger::setScriptName(string passedName)
{
	scriptName = passedName;
}

void CollisionTrigger::setScriptFullPath(const char* scriptFullName)
{
	script = scriptFullName;
}

ShapeData * CollisionTrigger::getShape()
{
	return shapeData;
}

void CollisionTrigger::setShape(ShapeData * passedShapeData)
{
	shapeData = passedShapeData;
}

void CollisionTrigger::buildCollisionShape()
{
	//Build Collision shape
	switch (shapeData->boundingShape)
	{
	case ShapeData::BoundingShape::BOX:
		shape = new btBoxShape(btVector3(shapeData->halfExtents.x, shapeData->halfExtents.y, shapeData->halfExtents.z));
		break;
	case ShapeData::BoundingShape::SPHERE:
		shape = new btSphereShape(shapeData->radius);
		break;
	case ShapeData::BoundingShape::CONE:
		shape = new btConeShape(shapeData->radius, shapeData->height);
		break;
	case ShapeData::BoundingShape::CYLINDER:
		shape = new btCylinderShape(btVector3(shapeData->halfExtents.x, shapeData->halfExtents.y, shapeData->halfExtents.z));
		break;
	}
}

luabridge::LuaRef CollisionTrigger::getParams()
{
	return params;
}

void CollisionTrigger::dispose()
{
	std::shared_ptr<BulletPhysics> bullet = std::dynamic_pointer_cast<BulletPhysics>(Engine::g_pEngine->getPhysics());
	if (bullet)
	{
		bullet->removeTrigger(this);
	}
}

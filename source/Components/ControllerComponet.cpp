#include <Components\ControllerComponent.h>


ControllerComponent::ControllerComponent(std::shared_ptr<Physics> physics, std::weak_ptr<GameObject> owner, ShapeData shape) : Component(ComponentType::CONTROLLER)
{
	this->owner = owner;
	this->physics = physics;
	this->input = Engine::g_pEngine->getInput();
	upDir = btVector3(0.0, 1.0, 0.0);
	btTransform transform;
	transform.setIdentity();

	auto sp = owner.lock();
	auto tp = sp != nullptr ? sp->getComponent<Transform>(ComponentType::TRANSFORM) : nullptr;
	transform.setOrigin(btVector3(tp->position.x, tp->position.y, tp->position.z));

	btCapsuleShape* collisionShape = new btCapsuleShape(shape.radius, shape.height);

	auto actorGhost = new btPairCachingGhostObject();
	actorGhost->setWorldTransform(transform);
	actorGhost->setCollisionShape(collisionShape);
	actorGhost->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
	
	
	this->controller = std::make_shared<BulletActerController>(actorGhost, static_cast<btConvexShape*>(collisionShape), btScalar(0.5), upDir);
	//this->controller->setFallSpeed(btScalar(10.0));
	this->controller->setGravity(btVector3(0.0f, -10.0f, 0.0f));
	this->controller->setAngularDamping(1.0);
	
	auto ptr = dynamic_pointer_cast<BulletPhysics, Physics>(physics);
	ptr->addController(this->controller);
}

void ControllerComponent::update(double dt)
{
	pollInput();
	auto ownerPtr = owner.lock();
	auto transformPtr = ownerPtr != nullptr ? ownerPtr->getComponent<Transform>(ComponentType::TRANSFORM) : nullptr;
	if (transformPtr == nullptr) //This shouldn't happen. A game object with physics but no transform doesn't make much sense. Check for it anyway just to be safe.
	{
		return;
	}
	else
	{
		updateTransform(transformPtr);
	}
}

void ControllerComponent::RecieveMessage(Message * msg)
{
}

void ControllerComponent::pollInput()
{
	KeyEventType left, right, up, down, space;
	left = input->getKeyState(KeyCodes::LEFT);
	right = input->getKeyState(KeyCodes::RIGHT);
	up = input->getKeyState(KeyCodes::UP);
	down = input->getKeyState(KeyCodes::DOWN);
	space = input->getKeyState(KeyCodes::SPACE);
	btVector3 walkDir(0.0, 0.0, 0.0);
	if (left == KeyEventType::KEY_PRESSED) walkDir.setX(btScalar(-0.1));
	else if (right == KeyEventType::KEY_PRESSED) walkDir.setX(btScalar(0.1));
	if (up == KeyEventType::KEY_PRESSED) walkDir.setZ(btScalar(-0.1));
	else if (down == KeyEventType::KEY_PRESSED) walkDir.setZ(btScalar(0.1));
	controller->setWalkDirection(walkDir);
	if (space == KeyEventType::KEY_PRESSED && controller->canJump()) controller->jump(upDir * 5);
}

void ControllerComponent::updateTransform(Transform* transformPtr)
{
	btTransform transform = controller->getGhostObject()->getWorldTransform();

	auto pos = transform.getOrigin();
	transformPtr->position = glm::vec3(pos.x(), pos.y(), pos.z());
	//transformPtr->orientation = glm::quat();
	auto q = transform.getRotation();
	transformPtr->orientation.w = q.getW();
	transformPtr->orientation.x = q.getX();
	transformPtr->orientation.y = q.getY();
	transformPtr->orientation.z = q.getZ();
}

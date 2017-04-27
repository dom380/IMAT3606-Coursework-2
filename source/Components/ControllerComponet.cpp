#include <Components\ControllerComponent.h>


ControllerComponent::ControllerComponent(std::shared_ptr<Physics> physics, std::weak_ptr<GameObject> owner, ShapeData shape, float yOffset, bool flip) : Component(ComponentType::CONTROLLER)
{
	this->owner = owner;
	this->physics = physics;
	this->input = Engine::g_pEngine->getInput();
	this->flip = flip;
	this->offset = yOffset;
	upDir = btVector3(0.0, 1.0, 0.0);
	btTransform transform;
	transform.setIdentity();
	
	auto sp = owner.lock();
	auto tp = sp != nullptr ? sp->getComponent<Transform>(ComponentType::TRANSFORM) : nullptr;
	if (tp)
	{
		if(this->flip)	frontDir.setRotation(upDir, btRadians(180));
		transform.setOrigin(btVector3(tp->position.x, tp->position.y, tp->position.z));
	}
	

	btCapsuleShape* collisionShape = new btCapsuleShape(shape.radius, shape.height);

	auto actorGhost = new btPairCachingGhostObject();
	actorGhost->setWorldTransform(transform);
	actorGhost->setCollisionShape(collisionShape);
	actorGhost->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
	
	
	this->controller = std::make_shared<BulletActerController>(actorGhost, static_cast<btConvexShape*>(collisionShape), btScalar(0.5), upDir);
	this->controller->setFallSpeed(btScalar(55.0));
	this->controller->setGravity(btVector3(0.0f, -30.0f, 0.0f));
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

void ControllerComponent::setCamera(std::shared_ptr<Camera> camera)
{
	this->camera = camera;
}

void ControllerComponent::setMovementSpeed(float speed)
{
	movementSpeed = speed;
}

void ControllerComponent::setGravity(float value)
{
	controller->setGravity(btVector3(0.0, value, 0.0));
}

void ControllerComponent::setGravity(float x, float y, float z)
{
	controller->setGravity(btVector3(x, y, z));
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
	btScalar vel = btScalar(movementSpeed);
	if (left == KeyEventType::KEY_PRESSED) walkDir.setX(-vel);
	else if (right == KeyEventType::KEY_PRESSED) walkDir.setX(vel);
	if (up == KeyEventType::KEY_PRESSED) walkDir.setZ(-vel);
	else if (down == KeyEventType::KEY_PRESSED) walkDir.setZ(vel);
	controller->setWalkDirection(walkDir);
	if (!walkDir.fuzzyZero())
	{
		calcDirection(walkDir);
	} 

	if (space == KeyEventType::KEY_PRESSED && controller->canJump())
	{
		//auto vertVel = controller->m_verticalVelocity;
		//std::cout << "Vertical Velocity: " << vertVel << std::endl;
		//auto vertOffset = controller->m_verticalOffset;
		//std::cout << "Vertical Offset: " << vertOffset << std::endl;
		controller->jump(upDir*10);
	}
}

void ControllerComponent::updateTransform(Transform* transformPtr)
{
	btTransform transform = controller->getGhostObject()->getWorldTransform();

	auto pos = transform.getOrigin();
	transformPtr->position = glm::vec3(pos.x(), pos.y()-offset, pos.z());
	//todo slerp to rotation across a few frames so model doesn't just jerk to new direction
	transformPtr->orientation.w = frontDir.getW();
	transformPtr->orientation.x = frontDir.getX();
	transformPtr->orientation.y = frontDir.getY();
	transformPtr->orientation.z = frontDir.getZ();
	
	if (camera) camera->move(transformPtr->position);
}

void ControllerComponent::calcDirection(const btVector3& walkDir)
{
	if (flip)
	{
		if (walkDir.getX() < 0 && walkDir.getZ() < 0) //NW
		{
			frontDir.setRotation(upDir, btRadians(-135));
		}
		else if (walkDir.getX() > 0 && walkDir.getZ() < 0) //NE
		{
			frontDir.setRotation(upDir, btRadians(135));
		}
		else if (walkDir.getX() > 0 && walkDir.getZ() > 0) //SE
		{
			
			frontDir.setRotation(upDir, btRadians(45));
		}
		else if (walkDir.getX() < 0 && walkDir.getZ() > 0) //SW
		{
			frontDir.setRotation(upDir, btRadians(-45));
			
		}
		else if (walkDir.getZ() < 0) //N
		{
			frontDir.setRotation(upDir, btRadians(180));
		}
		else if (walkDir.getZ() > 0) //S
		{
			frontDir.setRotation(upDir, btRadians(0));
		}
		else if (walkDir.getX() > 0) //E
		{
			frontDir.setRotation(upDir, btRadians(90));
		}
		else if (walkDir.getX() < 0) //W
		{
			frontDir.setRotation(upDir, btRadians(-90));
		}
	}
	else 
	{
		if (walkDir.getX() < 0 && walkDir.getZ() < 0) //NW
		{
			frontDir.setRotation(upDir, btRadians(45));
		}
		else if (walkDir.getX() > 0 && walkDir.getZ() < 0) //NE
		{
			frontDir.setRotation(upDir, btRadians(-45));
		}
		else if (walkDir.getX() > 0 && walkDir.getZ() > 0) //SE
		{
			frontDir.setRotation(upDir, btRadians(-135));
		}
		else if (walkDir.getX() < 0 && walkDir.getZ() > 0) //SW
		{
			frontDir.setRotation(upDir, btRadians(135));
		}
		else if (walkDir.getZ() < 0) //N
		{
			frontDir.setRotation(upDir, btRadians(0));
		}
		else if (walkDir.getZ() > 0) //S
		{
			frontDir.setRotation(upDir, btRadians(180));
		}
		else if (walkDir.getX() > 0) //E
		{
			frontDir.setRotation(upDir, btRadians(-90));
		}
		else if (walkDir.getX() < 0) //W
		{
			frontDir.setRotation(upDir, btRadians(90));
		}
	}
}

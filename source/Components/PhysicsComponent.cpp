#include "..\..\include\Components\PhysicsComponent.h"
#include "BulletCollision\CollisionShapes\btShapeHull.h"
#include "GL\glm\glm\gtc\matrix_transform.hpp"
#include "GL\glm\glm\gtx\transform.hpp"

PhysicsComponent::PhysicsComponent(std::shared_ptr<Physics> &physics, std::weak_ptr<GameObject> owner, std::shared_ptr<ModelData> mesh, float mass, bool isConvex) : Component(ComponentType::RIGID_BODY)
{
	convex = isConvex;
	//Retrieve the transform of the mesh
	btTransform transform;
	auto sp = owner.lock();
	auto tp = sp != nullptr ? sp->getComponent<Transform>(ComponentType::TRANSFORM) : nullptr;

	//Build collision shape
	buildCollisionShape(mesh, tp->scale);
	hasMesh = true;
	init(physics, owner, mass);
}

PhysicsComponent::PhysicsComponent(std::shared_ptr<Physics> &physics, std::weak_ptr<GameObject> owner, std::shared_ptr<std::vector<ConvexHull>> mesh, float mass, bool isConvex) : Component(ComponentType::RIGID_BODY)
{
	convex = isConvex;
	//Retrieve the transform of the mesh
	btTransform transform;
	auto sp = owner.lock();
	auto tp = sp != nullptr ? sp->getComponent<Transform>(ComponentType::TRANSFORM) : nullptr;

	//Build collision shape
	auto scale = tp->scale;
	buildCollisionShape(mesh, tp->scale);
	hasMesh = true;
	init(physics, owner, mass);
}

PhysicsComponent::PhysicsComponent(std::shared_ptr<Physics>& physics, std::weak_ptr<GameObject>& owner, ShapeData& boundingShape, float mass) : Component(ComponentType::RIGID_BODY)
{
	auto sp = owner.lock();
	auto tp = sp != nullptr ? sp->getComponent<Transform>(ComponentType::TRANSFORM) : nullptr;

	auto scale = tp->scale;

	buildCollisionShape(boundingShape);
	hasMesh = false;
	init(physics, owner, mass);
}

void PhysicsComponent::update(double dt)
{
	if (constVelocity) 
	{
		btVector3 currentVelDir = body->getLinearVelocity();
		btScalar currentVel = currentVelDir.length();
		btScalar desiredVel = velocity.length();
		if (currentVel < desiredVel) //If velocity has slowed, set back to the desired value
		{
			currentVelDir *= desiredVel / currentVel;
			body->setLinearVelocity(currentVelDir);
		}
	}

	if (mass == 0.0f) return; //If mass is 0, it's not moving so no point updating the transform
	
	//Sync Transform with rigid body position
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

void PhysicsComponent::RecieveMessage(Message * msg)
{
	//Not listening for any messages.
}

btRigidBody * PhysicsComponent::getBody()
{
	return body;
}

float* PhysicsComponent::getMass()
{
	return &mass;
}

double PhysicsComponent::getRestitution()
{
	if (body)
		return body->getRestitution();
	else
		return restitution;
}

double PhysicsComponent::getFriction()
{
	if (body)
		return body->getFriction();
	else
		return friction;
}

double* PhysicsComponent::getRotationalFriction()
{
	if (body)
	{
		rotationalFriction = body->getRollingFriction();
	}
	return &rotationalFriction;
}

btVector3* PhysicsComponent::getVelocity()
{
	return &velocity;
}

bool* PhysicsComponent::isConstVelocity()
{
	return &constVelocity;
}

bool* PhysicsComponent::isConvex()
{
	return &convex;
}

bool * PhysicsComponent::hasMeshFile()
{
	return &hasMesh;
}

string* PhysicsComponent::getMeshFileName()
{
	return &meshFileName;
}

void PhysicsComponent::setConvex(bool passedConvex)
{
	convex = passedConvex;
}

void PhysicsComponent::setHasMesh(bool passedHasMesh)
{
	hasMesh = passedHasMesh;
}

ShapeData * PhysicsComponent::getShape()
{
	return shapeData;
}

void PhysicsComponent::setShape(ShapeData * passedShapeData)
{
	shapeData = passedShapeData;
}

void PhysicsComponent::setMeshFileName(string fileName)
{
	meshFileName = fileName;
}

void PhysicsComponent::setRestitution(double passedRestitution)
{
	restitution = passedRestitution;
	if (body)
		body->setRestitution(btScalar(passedRestitution));
}

void PhysicsComponent::setRestitution()
{
	if (body)
		body->setRestitution(btScalar(restitution));
}

void PhysicsComponent::setFriction(double passedFriction)
{
	friction = passedFriction;
	if (body)
		body->setFriction(btScalar(passedFriction));
}

void PhysicsComponent::setFriction()
{
	if (body)
		body->setFriction(btScalar(friction));
}

void PhysicsComponent::setRotationalFriction(double friction)
{
	rotationalFriction = friction;
	btScalar frictionScalar = btScalar(friction);
	if (body)
	{
		body->setRollingFriction(frictionScalar);
		body->setSpinningFriction(frictionScalar / btScalar(2.0));
	}
	
}

void PhysicsComponent::setRotationalFriction()
{
	btScalar frictionScalar = btScalar(rotationalFriction);
	if (body)
	{
		body->setRollingFriction(frictionScalar);
		body->setSpinningFriction(frictionScalar / btScalar(2.0));
	}
}

void PhysicsComponent::setVelocity(double x, double y, double z)
{
	velocity = btVector3(btScalar(x), btScalar(y), btScalar(z));
	if (body)
		body->setLinearVelocity(velocity);
}

void PhysicsComponent::setConstVelocity(bool flag)
{
	constVelocity = flag;
}

void PhysicsComponent::setTransform(Transform * transformPtr)
{
	btTransform transform = btTransform::getIdentity();
	auto pos = transformPtr->position;
	transform.setOrigin(btVector3(pos.x, pos.y, pos.z));
	auto orientation = transformPtr->orientation;
	transform.setRotation(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w));
	auto ms = body->getMotionState();
	ms->setWorldTransform(transform);
}

void PhysicsComponent::buildCollisionShape(std::shared_ptr<ModelData> mesh, glm::vec3 scale)
{
	//if (mass > 0.f) 
	//{
		if (mesh->points.size() > 100) //If the mesh is too complex to directly use, simplify it.
		{
			btConvexHullShape* originalConvexShape = new btConvexHullShape();
			for (glm::vec3 point : mesh->points)
			{
				originalConvexShape->addPoint(btVector3(point.x, point.y, point.z));
			}

			btShapeHull* hull = new btShapeHull(originalConvexShape);
			btScalar margin = originalConvexShape->getMargin();
			hull->buildHull(margin);
			shape = new btConvexHullShape();
			for (int i = 0; i < hull->numVertices(); i++)
			{
				((btConvexHullShape*)shape)->addPoint(hull->getVertexPointer()[i]);
			}
			delete originalConvexShape; //Delete temp pointers
			delete hull;
		}
		else
		{
			btVector4 point;
			shape = new btConvexHullShape();
			for (glm::vec3 p : mesh->points)
			{
				point = btVector4(p.x*scale.x, p.y*scale.y, p.z*scale.z, 1.0);
				((btConvexHullShape*)shape)->addPoint(point);
			}
		}
		///TODOD - Fix building triangle collision meshes for static objects
	//} 
	//else
	//{
		//std::vector<btScalar> vertices;
		//std::vector<int> indices;
		//for (glm::vec3 vert : mesh->vertices)
		//{
		//	vertices.push_back(vert.x);
		//	vertices.push_back(vert.y);
		//	vertices.push_back(vert.z);
		//}
		//for (unsigned short idx : mesh->indices)
		//{
		//	indices.push_back(idx);
		//}
		//int numOfTris = mesh->indices.size() / 3;
		//btTriangleIndexVertexArray* collisionMesh = new btTriangleIndexVertexArray(numOfTris, &indices[0], 1, indices.size(), &vertices[0], 3);
		//btTriangleMesh* collisionMesh = new btTriangleMesh();
		//for (int i = 0; i < mesh->indices.size(); i += 3)
		//{
		//	auto p1 = mesh->vertices[mesh->indices[i]];
		//	auto p2 = mesh->vertices[mesh->indices[i+1]];
		//	auto p3 = mesh->vertices[mesh->indices[i+2]];
		//	btVector3 bv1 = btVector3(p1.x, p1.y, p1.z);
		//	btVector3 bv2 = btVector3(p2.x, p2.y, p2.z);
		//	btVector3 bv3 = btVector3(p3.x, p3.y, p3.z);
		//	collisionMesh->addTriangle(bv1, bv2, bv3);
		//}
		//shape = new btBvhTriangleMeshShape(collisionMesh, true);
		//shape = new btBoxShape(scale/2.0);
	//}
}

void PhysicsComponent::buildCollisionShape(std::shared_ptr<std::vector<ConvexHull>> mesh, glm::vec3 scale)
{
	shape = new btCompoundShape();
	for (ConvexHull ch : *mesh)
	{
		unsigned int numDoubles = ch.m_points.size();
		btVector3 centroid(0.0, 0.0, 0.0);
		//Calculate centre of mass
		for (unsigned int indx = 0; indx < numDoubles; indx += 3)
		{
			centroid += btVector3(
				(btScalar)ch.m_points[indx] * scale.x,
				(btScalar)ch.m_points[indx + 1] * scale.y,
				(btScalar)ch.m_points[indx + 2] * scale.z);
		}
		centroid /= (btScalar)ch.m_triangles.size();
		// Create convex shape
		// Adjust points such that the centroid is at (0,0,0)
		btConvexHullShape* convexShape = new btConvexHullShape();
		for (unsigned int i = 0; i < numDoubles; i += 3)
		{
			btVector3 point(
				(btScalar)ch.m_points[i] * scale.x,
				(btScalar)ch.m_points[i + 1] * scale.y,
				(btScalar)ch.m_points[i + 2] * scale.z);
			point -= centroid;
			convexShape->addPoint(point, false);
		}
		convexShape->recalcLocalAabb();
		// Append to the compound shape
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(centroid);
		((btCompoundShape*)shape)->addChildShape(transform, convexShape);
	}
}

void PhysicsComponent::buildCollisionShape(ShapeData  boundingShape)
{
	shapeData = new ShapeData(boundingShape);
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
	case ShapeData::BoundingShape::CAPSULE:
		shape = new btCapsuleShape(boundingShape.radius, boundingShape.height);
		break;
	}
}

void PhysicsComponent::updateTransform(Transform* transformPtr)
{
	auto ms = body->getMotionState();
	btTransform transform;
	ms->getWorldTransform(transform);
	
	auto pos = transform.getOrigin();
	transformPtr->position = glm::vec3(pos.x(), pos.y(), pos.z());
	auto q = transform.getRotation();
	transformPtr->orientation.w = q.getW();
	transformPtr->orientation.x = q.getX();
	transformPtr->orientation.y = q.getY();
	transformPtr->orientation.z = q.getZ();
}

void PhysicsComponent::init(std::shared_ptr<Physics>& physics, std::weak_ptr<GameObject> owner, float mass)
{
	this->owner = owner;
	this->mass = mass;

	//Retrieve the transform of the mesh
	btTransform transform;
	auto sp = owner.lock();
	auto tp = sp != nullptr ? sp->getComponent<Transform>(ComponentType::TRANSFORM) : nullptr;
	transform.setOrigin(btVector3(tp->position.x, tp->position.y, tp->position.z));
	transform.setRotation(btQuaternion(tp->orientation.x, tp->orientation.y, tp->orientation.z, tp->orientation.w));

	if (mass > 0.f)
		shape->calculateLocalInertia(mass, localInertia);

	//Make sure the rigid body is in the same position as the mesh
	btDefaultMotionState* ms = new btDefaultMotionState(transform);

	btRigidBody::btRigidBodyConstructionInfo bodyInfo(mass, ms, shape, localInertia);

	//Construct rigid body and save pointer to this component's game object so it can be used in collision response.
	body = new btRigidBody(bodyInfo);
	body->setUserPointer(sp.get());
	
	//Add the body to the physics system
	physics->addBody(*this);
}

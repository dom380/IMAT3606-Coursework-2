#include "..\..\include\Components\PhysicsComponent.h"
#include "BulletCollision\CollisionShapes\btShapeHull.h"

PhysicsComponent::PhysicsComponent() : Component(ComponentType::RIGID_BODY)
{
	mass = 0.0f;
}

PhysicsComponent::PhysicsComponent(std::weak_ptr<GameObject> owner, std::shared_ptr<ModelData> mesh, float mass, bool convex) : Component(ComponentType::RIGID_BODY)
{
	this->owner = owner;
	this->mass = mass;
	buildCollisionShape(mesh, convex);
	if(mass> 0.f)
	{
		shape->calculateLocalInertia(mass, localInertia);
	}
	//Retrieve the transform of the mesh
	btTransform transform;
	auto sp = owner.lock();
	auto tp = sp != nullptr ? sp->getComponent<Transform>(ComponentType::TRANSFORM) : nullptr;
	glm::mat4 result = tp != nullptr ? glm::translate(glm::mat4(1.0f), tp->position) * glm::scale(glm::mat4(1.0), tp->scale) * glm::toMat4(tp->orientation) : glm::mat4(1.0);
	transform.setFromOpenGLMatrix(glm::value_ptr(result));
	//Make sure the rigid body is in the same position as the mesh
	btDefaultMotionState* ms = new btDefaultMotionState(transform);
	body = new btRigidBody(mass, ms, shape);
}

btRigidBody * PhysicsComponent::getBody()
{
	return body;
}

void PhysicsComponent::buildCollisionShape(std::shared_ptr<ModelData> mesh, bool convex)
{
	if (convex)
	{
		if (mesh->vertices.size() > 100) //If the mesh is too complex to directly use, simplify it.
		{
			btConvexHullShape* originalConvexShape = new btConvexHullShape();
			for (glm::vec3 point : mesh->vertices)
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
			shape = new btConvexHullShape();
			for (glm::vec3 p : mesh->vertices)
			{
				((btConvexHullShape*)shape)->addPoint(btVector3(p.x,p.y,p.z));
			}
		}
	}
	else
	{
		//todo
	}
}

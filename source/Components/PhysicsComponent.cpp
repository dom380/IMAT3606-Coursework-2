#include "..\..\include\Components\PhysicsComponent.h"
#include "BulletCollision\CollisionShapes\btShapeHull.h"
#include "GL\glm\glm\gtc\matrix_transform.hpp"
#include "GL\glm\glm\gtx\transform.hpp"

PhysicsComponent::PhysicsComponent(std::shared_ptr<Physics> &physics, std::weak_ptr<GameObject> owner, std::shared_ptr<ModelData> mesh, float mass, bool convex) : Component(ComponentType::RIGID_BODY)
{
	this->owner = owner;
	this->mass = mass;

	//Retrieve the transform of the mesh
	btTransform transform;
	auto sp = owner.lock();
	auto tp = sp != nullptr ? sp->getComponent<Transform>(ComponentType::TRANSFORM) : nullptr;
	auto rotMat = tp->orientation.w != 0.f ? glm::rotate(glm::radians(tp->orientation.w), glm::vec3(tp->orientation.x, tp->orientation.y, tp->orientation.z)) : glm::mat4(1.0);
	auto scaleMat = glm::scale(glm::mat4(1.0), tp->scale);
	auto transMat = glm::translate(glm::mat4(1.0f), tp->position);
	glm::mat4 result = tp != nullptr ? transMat * rotMat : glm::mat4(1.0);
	transform.setFromOpenGLMatrix(glm::value_ptr(result));

	buildCollisionShape(mesh, convex, tp->scale);

	//Make sure the rigid body is in the same position as the mesh
	btDefaultMotionState* ms = new btDefaultMotionState(transform);
	body = new btRigidBody(mass, ms, shape, localInertia);

	//Add the body to the physics system
	physics->addBody(*this); 
}

void PhysicsComponent::update(double dt)
{
	if (mass == 0.0f) return; //If mass is 0, it's not moving so no point updating the transform
	//Sync Transform with rigid body position
	auto ms = body->getMotionState();
	auto ownerPtr = owner.lock();
	auto transformPtr = ownerPtr != nullptr ? ownerPtr->getComponent<Transform>(ComponentType::TRANSFORM) : nullptr;
	if (transformPtr == nullptr) //This shouldn't happen. A game object with physics but no transform doesn't make much sense. Check for it anyway just to be safe.
	{
		return;
	}
	else //Note this won't work with negative scaling (Reflection). So don't do that.
	{
		btTransform transform;
		glm::mat4 mat;
		ms->getWorldTransform(transform);
		transform.getOpenGLMatrix(glm::value_ptr(mat));
		transformPtr->position = mat[3];
		transformPtr->scale = glm::vec3(glm::length(mat[0]), glm::length(mat[1]), glm::length(mat[2]));
		glm::mat4 rotMatrix = glm::lookAt(glm::vec3(0), glm::vec3(mat[2]), glm::vec3(mat[1]));
		transformPtr->orientation = glm::quat_cast(rotMatrix);
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

void PhysicsComponent::buildCollisionShape(std::shared_ptr<ModelData>& mesh, bool& convex, glm::vec3& scale)
{
	//if (mass > 0.f) 
	//{
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
				btVector4 point;
				shape = new btConvexHullShape();
				for (glm::vec3 p : mesh->points)
				{
					point = btVector4(p.x*scale.x, p.y*scale.y, p.z*scale.z, 1.0);
					((btConvexHullShape*)shape)->addPoint(point);
				}
			}
		}
		else
		{
			///TODO - Do something with concave objects
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

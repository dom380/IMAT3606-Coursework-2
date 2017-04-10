#pragma once
#ifndef PHYSICSCOMPONENT_H
#define PHYSICSCOMPONENT_H
#include "Component.h"
#include <GameObject.h>
#include "btBulletDynamicsCommon.h"
#include <Graphics\ModelData.h>
#include <memory>
#include <Physics\BulletPhysics.h>
#include <Physics\ShapeData.h>
class Physics;
/*
	Implementation of the Component interface responsible for holding
	physics simulation data.
*/
class PhysicsComponent : public Component
{
public:
	/*
		Constructor.
		std::shared_ptr<Physics> &physics - Pointer to the Physics implementation,
		std::weak_ptr<GameObject> owner - Pointer to the GameObject this component belongs to.
		std::shared_ptr<ModelData> mesh - Pointer to this components collision mesh.
		float mass - The mass of the GameObject. A value of 0.0 signifies an immovable object.
		bool convex - Specifies whether the collision mesh is convex.
	*/
	PhysicsComponent(std::shared_ptr<Physics> &physics, std::weak_ptr<GameObject> owner, std::shared_ptr<ModelData> mesh, float mass, bool convex);
	/*
		Constructor.
		std::shared_ptr<Physics> &physics - Pointer to the Physics implementation,
		std::weak_ptr<GameObject> owner - Pointer to the GameObject this component belongs to.
		ShapeData& boundingShape - Object specifying the GameObject's primative bounding shape.
		float mass - The mass of the GameObject. A value of 0.0 signifies an immovable object.
		bool convex - Specifies whether the collision mesh is convex.
	*/
	PhysicsComponent(std::shared_ptr<Physics> &physics, std::weak_ptr<GameObject>& owner, ShapeData& boundingShape, float mass);

	/*
		Update's the GameObject's transform component to match the physics simulation.
		If enabled, also maintains the specified constant linear velocity. 
		double dt - The current time step. Ignored. 
	*/
	void update(double dt);
	/*
		Interface method stub. May be used in the future.
	*/
	void RecieveMessage(Message* msg);
	/*
		Returns a pointer to the component's rigid body
		TODO - This is bleeding implementation details. Fix it!
	*/
	btRigidBody* getBody();
	/*
		Sets the restitution of the GameObject.
		double restitution - A value between 0..1 specifying how much energy is retained by this object after collision. 
	*/
	void setRestitution(double restitution);
	/*
		Sets the object's linear velocity. Note this overrides the object's current velocity.
		double x - The X component of the velocity vector.
		double y - The Y component of the velocity vector.
		double z - The Z component of the velocity vector.
	*/
	void setVelocity(double x, double y, double z);
	/*
		Specifies whether the component's velocity is constant. 
	*/
	void setConstVelocity(bool flag);

	
private:
	std::weak_ptr<GameObject> owner;
	float mass;
	btRigidBody* body;
	btCollisionShape* shape;
	btVector3 localInertia = btVector3(0.0, 0.0, 0.0);
	btVector3 velocity = btVector3(0.0, 0.0, 0.0);
	bool constVelocity = false;
	void buildCollisionShape(std::shared_ptr<ModelData>& mesh, bool& convex, glm::vec3& scale);
	void updateTransform(Transform* transformPtr);
};

#endif // !PHYSICSCOMPONENT_H


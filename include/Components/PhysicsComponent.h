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
#include <Physics\ConvexHull.h>
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
		std::shared_ptr<ModelData> mesh - Pointer to this components collision mesh.
		float mass - The mass of the GameObject. A value of 0.0 signifies an immovable object.
		bool convex - Specifies whether the collision mesh is convex.
	*/
	PhysicsComponent(std::shared_ptr<Physics> &physics, std::weak_ptr<GameObject> owner, std::shared_ptr<std::vector<ConvexHull>> mesh, float mass, bool convex);
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

	float getMass();
	double getRestitution();
	double getFriction();
	double getRotationalFriction();
	btVector3 getVelocity();
	bool isConstVelocity();
	bool isConvex();

	string getMeshFileName();

	ShapeData* getShape();

	void setMeshFileName(string fileName);
	/*
		Sets the restitution of the GameObject.
		double restitution - A value between 0..1 specifying how much energy is retained by this object after collision. 
	*/
	void setRestitution(double restitution);
	/*
		Sets the friction coefficent of the GameObject.
		double friction - A value between 0..1 specifying the frictional coefficent.
	*/
	void setFriction(double friction);
	/*
		Sets the rotational friction coefficent of the GameObject.
		double friction - A value between 0..1 specifying the rotational frictional coefficent.
	*/
	void setRotationalFriction(double friction);
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

	/*
		Aplies the specified transform to the physics object to.
	*/
	void setTransform(Transform* transformPtr);
private:
	std::weak_ptr<GameObject> owner;
	string meshFileName;
	bool convex;
	float mass;
	btRigidBody* body;
	btCollisionShape* shape;
	ShapeData* shapeData;
	btVector3 localInertia = btVector3(0.0, 0.0, 0.0);
	btVector3 velocity = btVector3(0.0, 0.0, 0.0);
	bool constVelocity = false;
	double rotationalFriction;
	//private utility methods
	/*
		Constructs the collision shape from the specified mesh
	*/
	void buildCollisionShape(std::shared_ptr<ModelData>& mesh, glm::vec3& scale);
	/*
		Constructs the collision shape from the specified ConvexHulls
	*/
	void buildCollisionShape(std::shared_ptr<std::vector<ConvexHull>> mesh, glm::vec3& scale);
	/*
		Synchronizes the Transform component with the physics simulation transform
	*/
	void updateTransform(Transform* transformPtr);
	/*
		Shared initialization code between constructors.
	*/
	void init(std::shared_ptr<Physics> &physics, std::weak_ptr<GameObject> owner, float mass);
};

#endif // !PHYSICSCOMPONENT_H


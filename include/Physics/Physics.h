#pragma once
#ifndef PHYSICS_H
#define PHYSICS_H
#include <vector>
#include <utils\UUID\guid.h>
class PhysicsComponent;

/*
	Interface for the Physics system.
*/
class Physics
{
public:
	/*
		Initialisation method called on start up.
		Implementations should perform one off and time consuming initalisation here.
	*/
	virtual void init() = 0;
	/*
		Set's the worlds gravity vector.
		double x - The X component of the gravity vector.
		double y - The Y component of the gravity vector.
		double z - The Z component of the gravity vector.
	*/
	virtual void setGravity(double x, double y, double z) = 0;
	/*
		Updates the physics simulation.
		double dt - The current time step.
	*/
	virtual void update(double dt) = 0;
	/*
		Add the specified PhysicsComponent to the dynamic world.
	*/
	virtual void addBody(PhysicsComponent comp) = 0;
	virtual void removeBody(PhysicsComponent comp) = 0;
	/*
		Set's the Physic's implementation pause state to the specified flag.
	*/
	void setPaused(bool flag);
	/*
		Enum specifying the implementation of the physics system.
	*/
	enum PhysicsImpl
	{
		BULLET,
	};
protected:
	bool paused = true;
};
#endif // !PHYSICS_H

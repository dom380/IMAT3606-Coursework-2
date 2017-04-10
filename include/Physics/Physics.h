#pragma once
#ifndef PHYSICS_H
#define PHYSICS_H
class PhysicsComponent;
class Physics
{
public:
	virtual void init() = 0;
	virtual void setGravity(double x, double y, double z) = 0;
	virtual void update(double dt) = 0;
	virtual void addBody(PhysicsComponent comp) = 0;
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

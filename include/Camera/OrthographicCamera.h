#pragma once
#ifndef ORTHOGRAPHICCAMERA_H
#define ORTHOGRAPHICCAMERA_H
#include "Camera.h"

/*
	Orthographic implementation of Camera
	TODO.
*/
class OrthographicCamera : public Camera
{
public:
	virtual void lookAt(float x, float y, float z) {};

	virtual void lookAt(glm::vec3 target) {};

	virtual void move(glm::vec3 newPos) {};

	virtual void move(float x, float y, float z) {};

private:
	virtual void update() = 0;

};

#endif // !ORTHOGRAPHICCAMERA_H

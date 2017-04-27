#pragma once
#ifndef SHAPEDATA_H
#define SHAPEDATA_H
#include <gl\glm\glm\glm.hpp>
/*
	Basic class to store primative bounding shape information.
	Fields not used are defaulted to -1.0
*/
class ShapeData
{
public:
	enum BoundingShape
	{
		BOX,
		SPHERE,
		CONE,
		CYLINDER,
		CAPSULE
	};
	BoundingShape boundingShape;
	float radius = -1.0;
	float height = -1.0;
	glm::vec3 halfExtents = glm::vec3(-1.0,-1.0,-1.0);
};

#endif // !SHAPEDATA_H

#pragma once
#ifndef MODELDATA_H
#define MODELDATA_H
#include "Material.h"
#include <vector>
using std::vector;
/*
	Utility class that collects together model data.
*/
class ModelData
{
public:
	Material material;
	unsigned int vaoHandle;
	vector<unsigned int> vboHandles;
	size_t indexSize;
	vector<glm::vec4> vertices;
	vector<glm::vec4> points;
	vector<unsigned short> indices;
};

#endif // !MODELDATA_H


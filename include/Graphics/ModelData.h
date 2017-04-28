#pragma once
#ifndef MODELDATA_H
#define MODELDATA_H
#include "Material.h"
#include <Renderers\Graphics.h>
#include <vector>
using std::vector;
/*
	Utility class that collects together mesh data.
*/
class ModelData
{
public:
	Material material;
	struct Vertex {
		glm::vec4 Position;
		glm::vec4 Normal;
		glm::vec2 TexCoords;
	};
	/*
	Returns the ID of the VertexArrayObject containing this mesh's data on the GPU.
	If the VertexArrayObject has not been created at this point it will be built and returned here.
	*/
	unsigned int getVertArray(Graphics* graphics);
	
	vector<unsigned int> vboHandles;
	size_t indexSize;
	vector<Vertex> vertices;
	vector<glm::vec4> points;
	vector<unsigned short> indices;
private:
	unsigned int vaoHandle;
};

#endif // !MODELDATA_H


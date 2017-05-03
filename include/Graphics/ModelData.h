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
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		glm::vec3 Ka;
		glm::vec3 Kd;
		float Shininess;
	};
	// A mesh entry for each mesh read in from the Assimp scene. A model is usually consisted of a collection of these. 
#define INVALID_MATERIAL 0xFFFFFFFF
	struct MeshEntry {


		unsigned int BaseVertex; //!< Total number of mesh indices. 
		unsigned int BaseIndex; //!< The base vertex of this mesh in the vertices array for the entire model.
		unsigned int NumIndices; //!< The base index of this mesh in the indices array for the entire model. 
		unsigned int MaterialIndex;

		MeshEntry()
		{

			NumIndices = 0;
			BaseVertex = 0;
			BaseIndex = 0;
			MaterialIndex = INVALID_MATERIAL;
		}

		~MeshEntry() {}
	};
	/*
	Returns the ID of the VertexArrayObject containing this mesh's data on the GPU.
	If the VertexArrayObject has not been created at this point it will be built and returned here.
	*/
	unsigned int getVertArray(Graphics* graphics);
	int numOfMeshes;
	vector<unsigned int> vboHandles;
	size_t indexSize;
	vector<MeshEntry> meshes;
	vector<Vertex> vertices;
	vector<glm::vec4> points;
	vector<unsigned int> indices;
private:
	unsigned int vaoHandle;
};

#endif // !MODELDATA_H


#pragma once
#ifndef MODELFILEREADER_H
#define MODELFILEREADER_H
#include <GL\glm\glm\glm.hpp>
#include <GL\glm\glm\gtx\hash.hpp>
#include <vector>
using std::vector;
#include <Graphics\Material.h>
#include <Graphics\ModelData.h>
#include <Physics\ConvexHull.h>
#include <memory>
using std::shared_ptr;

class ModelFileReader
{
public:
	/*
		Reads the specified model file and loads the data to the relevant std::vectors.
		const char * filePath, File path, relative to the 'modelLocation' property, to the file.
		vector<glm::vec4>& vertices, std::vector to be loaded with vertices.
		vector<glm::vec3>& normals, std::vector to be loaded with normals.
		vector<glm::vec2>& textures, std::vector to be loaded with texture coordinates.
		vector<unsigned short>&indices, std::vector to be loaded with indices.
		Material& material, Material to be loaded with the model's material properties if specified.
		vector<glm::vec4>& points, std::vector to be loaded with the raw, vertex data.
	*/
	virtual void readFile(const char * filePath, vector<glm::vec4>& vertices, vector<glm::vec3>& normals, vector<glm::vec2>& textures, vector<unsigned short>&indices, Material& material, vector<glm::vec4>& points) = 0;

	virtual void readFile(const char * filePath, shared_ptr<vector<ConvexHull>>& convexHulls) = 0;

	virtual vector<shared_ptr<ModelData>> readFile(const char * filePath) = 0;
};

#endif // !MODELFILEREADER_H

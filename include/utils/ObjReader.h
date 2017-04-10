#pragma once
#ifndef OBJREADER_H
#define OBJREADER_H
#include "ModelFileReader.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <Graphics\Material.h>
/*
	Utility class to parse  Wavefront .obj and .mtl files
*/
class ObjReader : public ModelFileReader {
public:
	//Default constructor.
	ObjReader();
	~ObjReader();
	/*
		Reads the specified .obj file and loads the data to the relevant std::vectors.
		const char * filePath, File path, relative to the 'modelLocation' property, to the obj file.
		vector<glm::vec4>& vertices, std::vector to be loaded with vertices.
		vector<glm::vec3>& normals, std::vector to be loaded with normals.
		vector<glm::vec2>& textures, std::vector to be loaded with texture coordinates.
		vector<unsigned short>&indices, std::vector to be loaded with indices.
		Material& material, Material to be loaded with the model's material properties (If a .mtl is specified).
	*/
	void readFile(const char * filePath, vector<glm::vec4>& vertices, vector<glm::vec3>& normals, vector<glm::vec2>& textures, vector<unsigned short>&indices, Material& material, vector<glm::vec4>& points);
private:
	//private memebers
	vector<glm::vec3> faceIndices; //x = position index. y = texture index. z = normal index
	std::unordered_map<glm::vec3, unsigned short> indexVerticeMap;
	vector<glm::vec4> unpackedVertices;
	vector<glm::vec3> unpackedNormals;
	vector<glm::vec2> unpackedTextures;
	//Private methods
	/*
		Reads the .mtl file.
		const char* filePath, File path to the .mtl file.
		Material& material, Material object to be read into.
	*/
	void readMtl(const char* filePath, Material& material);
	/*
		utility method to create a glm::vec3 from the specified point in the string stream.
	*/
	glm::vec3 readVec3(std::istringstream& stream);
};

#endif // !OBJREADER_H
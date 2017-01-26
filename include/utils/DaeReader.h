#pragma once
#ifndef DAEREADER_H
#define DAEREADER_H
#include "ModelFileReader.h"
/*
	Utility class to parse .dae file types. 
	TODO
*/
class DaeReader : public ModelFileReader
{
	void readFile(const char * filePath, vector<glm::vec4>& vertices, vector<glm::vec3>& normals, vector<glm::vec2>& textures, vector<unsigned short>&indices, Material& material) {};
};

#endif // !DAEREADER_H

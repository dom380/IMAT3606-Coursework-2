#pragma once
#ifndef ASSIMPREADER_H
#define ASSIMPREADER_H
#include "ModelFileReader.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flag
#include <assimp/types.h>
#include <Graphics\ModelData.h>

class AssimpReader : public ModelFileReader
{
public:
	AssimpReader();

	void readFile(const char * filePath, vector<glm::vec4>& vertices, vector<glm::vec3>& normals, vector<glm::vec2>& textures, vector<unsigned short>&indices, Material& material, vector<glm::vec4>& points);

	void readFile(const char * filePath, shared_ptr<vector<ConvexHull>>& convexHulls) {};

	shared_ptr<ModelData> readFile(const char * filePath);
private:
	const aiScene* pScene; //!< The Assimp aiScene object. 

	std::string m_sDirectory;

	Assimp::Importer Importer; //!< Assimp importer object for reading in files into the aiScene. 

	

};
#endif // !ASSIMPREADER_H

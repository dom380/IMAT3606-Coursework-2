#include <utils\AssimpReader.h>

AssimpReader::AssimpReader()
{
}

void AssimpReader::readFile(const char * filePath, vector<glm::vec4>& vertices, vector<glm::vec3>& normals, vector<glm::vec2>& textures, vector<unsigned short>& indices, Material & material, vector<glm::vec4>& points)
{
	pScene = Importer.ReadFile(filePath,
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_OptimizeMeshes
		);
	if (pScene) {
		for (int i = 0; i < pScene->mNumMeshes; ++i)
		{
			const aiMesh* paiMesh = pScene->mMeshes[i];
			if (paiMesh->mMaterialIndex >= 0)
			{
				aiMaterial *assimpMat = pScene->mMaterials[paiMesh->mMaterialIndex];
				aiColor3D colour = aiColor3D(0.f);
				assimpMat->Get(AI_MATKEY_COLOR_AMBIENT, colour);
				material.Ka = glm::vec3(colour.r, colour.g, colour.b);
				assimpMat->Get(AI_MATKEY_COLOR_DIFFUSE, colour);
				material.Kd = glm::vec3(colour.r, colour.g, colour.b);
				assimpMat->Get(AI_MATKEY_COLOR_SPECULAR, colour);
				material.Ks = glm::vec3(colour.r, colour.g, colour.b);
				assimpMat->Get(AI_MATKEY_SHININESS, colour);
				material.Ns = colour.r;
				material.used = true;
			}
			const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
			vector<glm::vec4> tempVertices, tempNormals;
			vector<glm::vec2> tempTextures;
			for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) 
			{
				const aiVector3D* pPos = &(paiMesh->mVertices[i]);
				const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
				const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ?
					&(paiMesh->mTextureCoords[0][i]) : &Zero3D;

				tempVertices.push_back(glm::vec4(pPos->x, pPos->y, pPos->z, 1.0));
				points.push_back(glm::vec4(pPos->x, pPos->y, pPos->z, 1.0));
				tempNormals.push_back(glm::vec4(pNormal->x, pNormal->y, pNormal->z, 1.0));
				tempTextures.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));
			}
			for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) 
			{
				const aiFace& Face = paiMesh->mFaces[i];
				assert(Face.mNumIndices == 3);
				indices.push_back(Face.mIndices[0]);
				indices.push_back(Face.mIndices[1]);
				indices.push_back(Face.mIndices[2]);
			}
			int vertSize = tempVertices.size();
			for (int i = 0; i < indices.size(); i++)
			{
				if (vertSize != tempVertices.size())
				{
					int debugCheck = 0;
					debugCheck++;
				}
				auto index = indices[i];
				if (tempVertices.size() > index) {
					vertices.push_back(tempVertices.at(index));
				}
				if (tempNormals.size() > index) {
					normals.push_back(tempNormals.at(index));
				}

				if (tempTextures.size() > index) {
					textures.push_back(tempTextures.at(index));
				}

			}
		}
	}
	else {
		printf("Error parsing '%s': '%s'\n", filePath, Importer.GetErrorString());
		throw std::runtime_error(Importer.GetErrorString());
	}
}

shared_ptr<ModelData> AssimpReader::readFile(const char * filePath)
{
	pScene = Importer.ReadFile(filePath,
		//aiProcess_ValidateDataStructure |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_OptimizeMeshes
		);
	if (pScene) 
	{
		shared_ptr<ModelData> mesh = std::make_shared<ModelData>();
		mesh->numOfMeshes = pScene->mNumMeshes;
		mesh->meshes.resize(pScene->mNumMeshes);
		unsigned int NumVertices = 0;
		unsigned int NumIndices = 0;

		for (unsigned int i = 0; i < mesh->meshes.size(); i++) {
			// Total mesh indices. 
			mesh->meshes[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;

			// Set the base vertex of this mesh (initial vertex for this mesh within the vertices array) to the current total vertices. 
			mesh->meshes[i].BaseVertex = NumVertices;

			// Set the base index of this mesh (initial index for this mesh within the indices array) to the current total indices. 
			mesh->meshes[i].BaseIndex = NumIndices;

			// Increment total vertices and indices. 
			NumVertices += pScene->mMeshes[i]->mNumVertices;
			NumIndices += mesh->meshes[i].NumIndices;
		}
		mesh->vertices.reserve(NumVertices);
		mesh->indices.reserve(NumIndices);
		for (int i = 0; i < pScene->mNumMeshes; ++i)
		{
			const aiMesh* paiMesh = pScene->mMeshes[i];
			ModelData::Vertex vertex;
			if (paiMesh->mMaterialIndex >= 0)
			{
				aiMaterial *assimpMat = pScene->mMaterials[paiMesh->mMaterialIndex];
				aiColor3D colour = aiColor3D(0.f);
				assimpMat->Get(AI_MATKEY_COLOR_AMBIENT, colour);
				vertex.Ka = glm::vec3(colour.r, colour.g, colour.b);
				assimpMat->Get(AI_MATKEY_COLOR_DIFFUSE, colour);
				vertex.Kd = glm::vec3(colour.r, colour.g, colour.b);
				assimpMat->Get(AI_MATKEY_SHININESS, colour);
				vertex.Shininess = colour.r;
				
				if (mesh->numOfMeshes == 1 && i == 0)
				{
					mesh->material.Ka = vertex.Ka;
					mesh->material.Kd = vertex.Kd;
					mesh->material.Ns = vertex.Shininess;
					assimpMat->Get(AI_MATKEY_COLOR_SPECULAR, colour);
					mesh->material.Ks = glm::vec3(colour.r, colour.g, colour.b);
					mesh->material.used = true;
				}			
			}
			const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
			for (unsigned int i = 0; i < paiMesh->mNumVertices; i++)
			{
				const aiVector3D* pPos = &(paiMesh->mVertices[i]);
				const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
				const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ?
					&(paiMesh->mTextureCoords[0][i]) : &Zero3D;

				
				glm::vec4 vector;
				vector.x = paiMesh->mVertices[i].x;
				vector.y = paiMesh->mVertices[i].y;
				vector.z = paiMesh->mVertices[i].z;
				vector.w = 1.0;
				vertex.Position = vector;
				mesh->points.push_back(vector);
				// Normals
				vector.x = paiMesh->mNormals[i].x;
				vector.y = paiMesh->mNormals[i].y;
				vector.z = paiMesh->mNormals[i].z;
				vertex.Normal = vector;
				// Texture Coordinates
				if (paiMesh->mTextureCoords[0])
				{
					glm::vec2 vec;
					vec.x = paiMesh->mTextureCoords[0][i].x;
					vec.y = paiMesh->mTextureCoords[0][i].y;
					vertex.TexCoords = vec;
				}
				else
					vertex.TexCoords = glm::vec2(0.0f, 0.0f);
				mesh->vertices.push_back(vertex);

			}
			for (unsigned int i = 0; i < paiMesh->mNumFaces; i++)
			{
				const aiFace& Face = paiMesh->mFaces[i];
				assert(Face.mNumIndices == 3);
				mesh->indices.push_back(Face.mIndices[0]);
				mesh->indices.push_back(Face.mIndices[1]);
				mesh->indices.push_back(Face.mIndices[2]);
			}
		}
		return mesh;
	}
	else 
	{
		printf("Error parsing '%s': '%s'\n", filePath, Importer.GetErrorString());
		throw std::runtime_error(Importer.GetErrorString());
	}
}

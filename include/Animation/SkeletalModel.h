#pragma once

#include <gl\glew.h>
#include <gl\glm\glm\glm.hpp>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing fla
#include <vector>
#include <map>
#include "Math3D.h"
#include "Graphics/Shader.h"
#include <memory>
#include <assimp/types.h>
//#include <SOIL\SOIL.h>

struct VertexStruct
{
	glm::vec3 position; //!< Vertex position 
	glm::vec3 normal; //!< Vertex normal
	glm::vec2 uvs; //!< Vertex uv's

	glm::vec3 colour;
};

struct VertexBoneData
{
	unsigned int IDs[4]; //!< An array of 4 bone Ids that influence a single vertex.
	float Weights[4]; //!< An array of the weight influence per bone. 

	VertexBoneData()
	{
		// 0's out the arrays. 
		Reset();
	}

	void Reset()
	{
		memset(IDs, 0, 4 * sizeof(IDs[0]));
		memset(Weights, 0, 4 * sizeof(Weights[0]));
	}

	void AddBoneData(unsigned int BoneID, float Weight)
	{
		for (unsigned int i = 0; i < 4; i++) {

			// Check to see if there are any empty weight values. 
			if (Weights[i] == 0.0) {
				// Add ID of bone. 
				IDs[i] = BoneID;

				// Set the vertex weight influence for this bone ID. 
				Weights[i] = Weight;
				return;
			}

		}
		// should never get here - more bones than we have space for
		assert(0);
	}
};

// Stores bone information
struct BoneInfo
{
	Matrix4f FinalTransformation; // Final transformation to apply to vertices 
	Matrix4f BoneOffset; // Initial offset from local to bone space. 

	BoneInfo()
	{
		BoneOffset.SetZero();
		FinalTransformation.SetZero();
	}
};

//struct Texture //!< Struct to hold data for a texture
//{
//	GLuint id;
//	std::string type;
//	aiString path;
//};

// A mesh entry for each mesh read in from the Assimp scene. A model is usually consisted of a collection of these. 
#define INVALID_MATERIAL 0xFFFFFFFF
struct MeshEntry {


	unsigned int BaseVertex; //!< Total number of mesh indices. 
	unsigned int BaseIndex; //!< The base vertex of this mesh in the vertices array for the entire model.
	unsigned int NumIndices; //!< The base index of this mesh in the indices array for the entire model. 
	unsigned int MaterialIndex;
	//Texture texture; //make this a vector to hold multiple textures ->>to do


	MeshEntry()
	{

		NumIndices = 0;
		BaseVertex = 0;
		BaseIndex = 0;
		MaterialIndex = INVALID_MATERIAL;
	}

	~MeshEntry() {}
};

class SkeletalModel
{
public:

	SkeletalModel(std::shared_ptr<Shader> shaderProgIn); //!< Constructor 

	~SkeletalModel(); //!< Destructor 

	void LoadMesh(const std::string& Filename/*, bool isTextured*/); //!< Loads an animated mesh from a given file path and if the mesh is textured

	void BoneTransform(float TimeInSeconds, std::vector<Matrix4f>& Transforms); //!< Traverses the scene hierarchy and fetches the matrix transformation for each bone given the time. 

	void SetBoneTransform(unsigned int Index, const Matrix4f& Transform); //!< Inserts a bone transformation in the uniform array at the given index. 



	void render(); //!< Renders each mesh in the model. 

	bool isTextured();

	float interval;

private:

	void LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones); //!< Loads the bone data from a given mesh. 
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim); //!< Calculates the interpolated quaternion between two keyframes. 
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim); //!< Calculates the interpolated scaling vector between two keyframes. 
	void CalcInterpolatedTranslation(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim); //!< Calculates the interpolated translation vector between two keyframes. 

	unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim); //!< Finds a rotation key given the current animation time. 
	unsigned int FindScale(float AnimationTime, const aiNodeAnim* pNodeAnim); // Finds a scaling key given the current animation time. 
	unsigned int FindTranslation(float AnimationTime, const aiNodeAnim* pNodeAnim); // Finds a translation key given the current animation time. 

	void ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform); //!< Recursive function that traverses the scene's node hierarchy and combines the matrix transformations. 

	void InitFromScene(const aiScene* pScene, const std::string& Filename); //!< Prepares the model for rendering. 
	void InitMesh(unsigned int index, const aiMesh* paiMesh, std::vector<VertexStruct>& Vertices,
		std::vector<GLuint>& Indices, std::vector<VertexBoneData>& Bones/*, Texture& texture*/); //!< Fetches mesh data from given Assimp mesh. 

	void Clear(); //!< Deletes the vertex array object. 

	/*Texture LoadMeshTexture(aiMaterial *mat, aiTextureType type, std::string typeName)
	{
		Texture l_texture;

		//for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
		//{
		//	aiString str;
		//	mat->GetTexture(type, i, &str);

		//	std::string filename = std::string(str.C_Str());
		//	filename = this->m_sDirectory + '/' + filename;

		//	//generate texture id
		//	GLuint textureID;
		//	glGenTextures(1, &textureID);

		//	int width, height;
		//	unsigned char *image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

		//	// Assign texture to ID
		//	glBindTexture(GL_TEXTURE_2D, textureID);
		//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		//	glGenerateMipmap(GL_TEXTURE_2D);

		//	// Parameters
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//	glBindTexture(GL_TEXTURE_2D, 0);
		//	SOIL_free_image_data(image);

		//	Texture texture;
		//	texture.id = textureID;
		//	texture.type = typeName;
		//	texture.path = str;

		//	l_texture = texture;
		//}

		return l_texture;
	}*/

	std::shared_ptr<Shader> m_pShaderProg;

	GLuint m_VAO; // Vertex array object. 
	GLuint vbo; //!< Vertex buffer object. 
	GLuint ebo; //!< Indices buffer object. 
	GLuint boneBo; //!< Bone data buffer object. 

	const aiScene* pScene; //!< The Assimp aiScene object. 

	std::string m_sDirectory;

	Assimp::Importer Importer; //!< Assimp importer object for reading in files into the aiScene. 

	unsigned int m_NumBones; //!< Total number of bones in the model. 

	std::map<std::string, unsigned int> m_BoneMapping; //!< Map of bone names to ids

	std::vector<BoneInfo> m_BoneInfo; //!< Array containing bone information such as offset matrix and final transformation. 

	Matrix4f GlobalTransformation; //!< Root node transformation. 
	Matrix4f m_GlobalInverseTransform;

	std::vector<MeshEntry> m_Entries; //!< Array of mesh entries 

	bool m_isTextured;

	std::vector<Matrix4f> Transforms;
};
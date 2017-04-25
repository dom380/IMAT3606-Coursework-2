#include "Animation/SkeletalModel.h"

SkeletalModel::SkeletalModel(std::shared_ptr<Shader> shaderProgIn)
{
	m_VAO = 0;

	pScene = NULL;

	// Initialise the total number of bones to 0. 
	m_NumBones = 0;

	// Obtain pointer to shader program to use for rendering. 
	m_pShaderProg = shaderProgIn;

}

SkeletalModel::~SkeletalModel()
{
	Clear();
}

void SkeletalModel::Clear()
{
	if (m_VAO != 0) {
		glDeleteVertexArrays(1, &m_VAO);
		m_VAO = 0;
	}
}

void SkeletalModel::LoadMesh(const std::string& Filename)
{
	this->m_sDirectory = Filename.substr(0, Filename.find_last_of('/'));

	// Release the previously loaded mesh (if it exists)
	Clear();

	// Create the VAO
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	// Generate the buffers for the vertices atttributes
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glGenBuffers(1, &boneBo);

	pScene = Importer.ReadFile(Filename.c_str(),
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_FlipUVs |
		aiProcess_LimitBoneWeights);

	if (pScene) {

		m_GlobalInverseTransform = pScene->mRootNode->mTransformation;
		m_GlobalInverseTransform.Inverse();

		InitFromScene(pScene, Filename);
	}
	else {
		printf("Error parsing '%s': '%s'\n", Filename.c_str(), Importer.GetErrorString());
	}

	glBindVertexArray(0);
}

void SkeletalModel::InitFromScene(const aiScene* pScene, const std::string& Filename)
{
	m_Entries.resize(pScene->mNumMeshes);
	//m_Textures.resize(pScene->mNumMaterials);

	// Prepare vectors for vertex attributes and indices
	std::vector<VertexStruct> vertices;
	std::vector<VertexBoneData> bones;
	std::vector<unsigned int> Indices;
	//Texture texture; //use vector of textures here

	unsigned int NumVertices = 0;
	unsigned int NumIndices = 0;

	for (unsigned int i = 0; i < m_Entries.size(); i++) {
		// Total mesh indices. 
		m_Entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;

		// Set the base vertex of this mesh (initial vertex for this mesh within the vertices array) to the current total vertices. 
		m_Entries[i].BaseVertex = NumVertices;

		// Set the base index of this mesh (initial index for this mesh within the indices array) to the current total indices. 
		m_Entries[i].BaseIndex = NumIndices;

		// Increment total vertices and indices. 
		NumVertices += pScene->mMeshes[i]->mNumVertices;
		NumIndices += m_Entries[i].NumIndices;
	}

	// Reserve space in the vectors for the vertex attributes and indices
	vertices.reserve(NumVertices);
	bones.resize(NumVertices);
	Indices.reserve(NumIndices);

	// Initialize the meshes in the scene one by one
	for (unsigned int i = 0; i < m_Entries.size(); i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(i, paiMesh, vertices, Indices, bones);
		//m_Entries[i].texture = texture;
	}

	// Generate and populate the buffers with vertex attributes and the indices
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexStruct), &vertices[0], GL_STATIC_DRAW);

	// Vertex positions 
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexStruct), (GLvoid*)0);

	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexStruct), (GLvoid*)offsetof(VertexStruct, normal));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(VertexStruct), (GLvoid*)offsetof(VertexStruct, uvs));

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(VertexStruct), (GLvoid*)offsetof(VertexStruct, colour));

	//// Vertex Texture Coords
	//gl::EnableVertexAttribArray(2);
	//gl::VertexAttribPointer(2, 2, gl::FLOAT, FALSE, sizeof(VertexStruct), (GLvoid*)offsetof(VertexStruct, uvs));

	// Bind the bone data buffer object
	glBindBuffer(GL_ARRAY_BUFFER, boneBo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bones[0]) * bones.size(), &bones[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

	vertices.clear();
	Indices.clear();
	bones.clear();
}

void SkeletalModel::InitMesh(unsigned int index, const aiMesh* paiMesh, std::vector<VertexStruct>& Vertices, std::vector<GLuint>& Indices, std::vector<VertexBoneData>& Bones/*, Texture& texture*/)
{
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
	VertexStruct v;

	if (paiMesh->mMaterialIndex >= 0)
	{
		//If it has a material?
		//B
		aiMaterial *material = pScene->mMaterials[paiMesh->mMaterialIndex];

		//if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		//{
		//	//load material
		//	//texture = LoadMeshTexture(material, aiTextureType_DIFFUSE, "material.texture_diffuse");
		//}
		//else
		//{
			//aiColor3D ambient = aiColor3D(0.f);
			aiColor3D diffuse = aiColor3D(0.f);
			//aiColor3D specular = aiColor3D(0.f);

			//material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
			material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
			//material->Get(AI_MATKEY_COLOR_SPECULAR, specular);

			v.colour = glm::vec3(diffuse.r, diffuse.g, diffuse.b);

			//store colours in buffer and send to shader
		//}
		//loop through each texture in material and add item to vector of textures
	}

	// Populate the vertex attribute vectors
	for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ?
			&(paiMesh->mTextureCoords[0][i]) : &Zero3D;

		glm::vec3 glmTempPos = glm::vec3(pPos->x, pPos->y, pPos->z);
		glm::vec3 glmTempNormal = glm::vec3(pNormal->x, pNormal->y, pNormal->z);
		glm::vec2 glmTempUV = glm::vec2(pTexCoord->x, pTexCoord->y);

		v.position = glmTempPos;
		v.normal = glmTempNormal;
		v.uvs = glmTempUV;
		Vertices.push_back(v);
	}

	// Load the mesh's bones. 
	if (paiMesh->HasBones()) {
		LoadBones(index, paiMesh, Bones);

	}

	// Populate the indices array 
	for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
		const aiFace& Face = paiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		Indices.push_back(Face.mIndices[0]);
		Indices.push_back(Face.mIndices[1]);
		Indices.push_back(Face.mIndices[2]);
	}
}

void SkeletalModel::LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones)
{

	// Loop through all bones in the Assimp mesh.
	for (unsigned int i = 0; i < pMesh->mNumBones; i++) {

		unsigned int BoneIndex = 0;

		// Obtain the bone name.
		std::string BoneName(pMesh->mBones[i]->mName.data);

		// If bone isn't already in the map. 
		if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {

			// Set the bone ID to be the current total number of bones. 
			BoneIndex = m_NumBones;

			// Increment total bones. 
			m_NumBones++;

			// Push new bone info into bones vector. 
			BoneInfo bi;
			m_BoneInfo.push_back(bi);
		}
		else {
			// Bone ID is already in map. 
			BoneIndex = m_BoneMapping[BoneName];
		}

		m_BoneMapping[BoneName] = BoneIndex;

		// Obtains the offset matrix which transforms the bone from mesh space into bone space. 
		m_BoneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;


		// Iterate over all the affected vertices by this bone i.e weights. 
		for (unsigned int j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {

			// Obtain an index to the affected vertex within the array of vertices.
			unsigned int VertexID = m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
			// The value of how much this bone influences the vertex. 
			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;

			// Insert bone data for particular vertex ID. A maximum of 4 bones can influence the same vertex. 
			Bones[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}
}

void SkeletalModel::BoneTransform(float TimeInSeconds, std::vector<Matrix4f>& Transforms)
{
	Matrix4f Identity;
	Identity.InitIdentity();

	float TicksPerSecond = pScene->mAnimations[0]->mTicksPerSecond;
	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, pScene->mAnimations[0]->mDuration);

	ReadNodeHierarchy(AnimationTime, pScene->mRootNode, Identity);

	Transforms.resize(m_NumBones);

	// Populates transforms vector with new bone transformation matrices. 
	for (unsigned int i = 0; i < m_NumBones; i++) {
		Transforms[i] = m_BoneInfo[i].FinalTransformation;
	}

}

unsigned int SkeletalModel::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// Check if there are rotation keyframes. 
	assert(pNodeAnim->mNumRotationKeys > 0);

	// Find the rotation key just before the current animation time and return the index. 
	for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}
	assert(0);

	return 0;
}

unsigned int SkeletalModel::FindScale(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	// Find the scaling key just before the current animation time and return the index. 
	for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}
	assert(0);

	return 0;
}

unsigned int SkeletalModel::FindTranslation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumPositionKeys > 0);

	// Find the translation key just before the current animation time and return the index. 
	for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}
	assert(0);

	return 0;
}


void SkeletalModel::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}
	// Obtain the current rotation keyframe. 
	unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);

	// Calculate the next rotation keyframe and check bounds. 
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);

	// Calculate delta time, i.e time between the two keyframes.
	float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime;

	// Calculate the elapsed time within the delta time.  
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);

	// Obtain the quaternions values for the current and next keyframe. 
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;

	// Interpolate between them using the Factor. 
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);

	// Normalise and set the reference. 
	Out = Out.Normalize();
}

void SkeletalModel::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	unsigned int ScalingIndex = FindScale(AnimationTime, pNodeAnim);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime;
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;

	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

void SkeletalModel::CalcInterpolatedTranslation(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}


	unsigned int PositionIndex = FindTranslation(AnimationTime, pNodeAnim);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime;
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;

	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

void SkeletalModel::SetBoneTransform(unsigned int Index, const Matrix4f& Transform)
{
	assert(Index < 100);

	m_pShaderProg->setUniformIndex(Index, Transform);
}

void SkeletalModel::render()
{
	BoneTransform(interval, Transforms);
	
	for (unsigned int i = 0; i < Transforms.size(); i++)
	{
		SetBoneTransform(i, Transforms[i]);
	}

	glBindVertexArray(m_VAO);

	// Render all the model's meshes.
	for (unsigned int i = 0; i < m_Entries.size(); i++) {
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, m_Entries.at(i).texture.id);
		//m_pShaderProg->setUniform("material.texture_diffuse", 0);

		glDrawElementsBaseVertex(GL_TRIANGLES,
			m_Entries[i].NumIndices,
			GL_UNSIGNED_INT,
			(void*)(sizeof(unsigned int) * m_Entries[i].BaseIndex),
			m_Entries[i].BaseVertex);
	}

	// Make sure the VAO is not changed from the outside 
	glBindVertexArray(0);
}

void SkeletalModel::ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform)
{
	Matrix4f IdentityTest;
	IdentityTest.InitIdentity();

	// Obtain the name of the current node 
	std::string NodeName(pNode->mName.data);

	// Use the first animation 
	const aiAnimation* pAnimation = pScene->mAnimations[0];

	// Obtain transformation relative to node's parent. 
	Matrix4f NodeTransformation(pNode->mTransformation);

	const aiNodeAnim* pNodeAnim = NULL;

	// Find the animation channel of the current node. 
	for (unsigned i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnimIndex = pAnimation->mChannels[i];

		// If there is a match for a channel with the current node's name, then we've found the animation channel. 
		if (std::string(pNodeAnimIndex->mNodeName.data) == NodeName) {
			pNodeAnim = pNodeAnimIndex;
		}
	}

	if (pNodeAnim) {

		//// Interpolate scaling and generate scaling transformation matrix
		//aiVector3D Scaling;
		//CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		//Matrix4f ScalingM;
		//ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		Matrix4f RotationM = Matrix4f(RotationQ.GetMatrix());

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		CalcInterpolatedTranslation(Translation, AnimationTime, pNodeAnim);
		Matrix4f TranslationM;
		TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);

		// Combine the above transformations
		NodeTransformation = TranslationM * RotationM;/* *ScalingM;*/
	}

	Matrix4f GlobalTransformation = ParentTransform * NodeTransformation;

	// Apply the final transformation to the indexed bone in the array. 
	if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
		unsigned int BoneIndex = m_BoneMapping[NodeName];
		m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform  * GlobalTransformation *
			m_BoneInfo[BoneIndex].BoneOffset;
	}

	// Do the same for all the node's children. 
	for (unsigned i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}

bool SkeletalModel::isTextured()
{
	return m_isTextured;
}

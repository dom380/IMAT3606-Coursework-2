#pragma once
#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H
#include <memory>
using std::shared_ptr;
#include <map>
using std::map;
#include <Graphics\Texture.h>
#include <string>
using std::string;
#include <GUI\Font.h>
#include <Graphics\Shader.h>
#include <Graphics\ModelData.h>
#include <Physics\ConvexHull.h>
#include <utils\ModelFileReader.h>
#include <utils\ObjReader.h>
#include <utils\DaeReader.h>
#include <utils\FbxReader.h>
#include <utils\AssimpReader.h>
/*
	Singleton class responsible for loading and storing all Assets.
*/
class AssetManager {
public:
	/*
		Returns the AssetManager instance or creates one if it doesn't exist.
	*/
	static shared_ptr<AssetManager> getInstance();
	/*
		Load and/or Return a Pointer to a Texture
		const char* fileName, The name of the texture to retrieve.
	*/
	shared_ptr<Texture> getTexture(const char* fileName);
	/*
		Load and/or Return a Pointer to a Font.
		const char* fileName, The name of the Font to retrieve.
		shared_ptr<Graphics>& graphics, Pointer to the graphics system.
	*/
	shared_ptr<Font> getFont(char* fontPath, shared_ptr<Graphics>& graphics);
	/*
		Load and/or Return a Pointer to a Font.
		std::pair<string, string> shaderName, A std::pair of strings specifying the vertex and fragment shaders.
	*/
	shared_ptr<Shader> getShader(std::pair<string, string> shaderName);
	/*
		Load and/or Return a vector of pointers to ModelData.
		const char* fileName, The name of the model to retrieve.
		shared_ptr<Graphics>& graphics, Pointer to the graphics system.
	*/
	shared_ptr<ModelData> getModelData(const char* fileName, shared_ptr<Graphics> graphics);
	/*
		Load and/or Return a Pointer to a vector of ConvexHull.
		const char* fileName, The name of the collision mesh to retrieve.
	*/
	shared_ptr<std::vector<ConvexHull>> getCollisionData(const char* fileName);

	/*
		Returns the full file path of the given script name.
		const char* fileName, The name of the script to retrieve.
	*/
	string getScript(const char* fileName);

	/*
		Enum specifying the type of resource.
	*/
	enum ResourceType {
		LEVEL,
		TEXTURE,
		MODEL,
		FONT,
		SHADER,
		SCRIPT
	};
	/*
		Returns the file path of to the folder containing files of the specified resource type.
	*/
	string getRootFolder(ResourceType resourceType);
	/*
		Sets the Root folder of the specified resource type.
		string path, File path to the folder containing the resource. Must include a trailing '/'.
		ResourceType resourceType, Enum specifying the type of resource.
	*/
	void setAssetFolder(string path, ResourceType resourceType);
	/*
		Clean up resources.
	*/
	void exit();


private:
	AssetManager() {};
	AssetManager(AssetManager const&) {}; // prevent copies
	void operator=(AssetManager const&) {}; // prevent assignments
	/*
		Utility method to parse a file name and return the extension.
		Does not include the '.'
		If no extension found returns an empty string.
		const string& fileName, The file name to check for extensions.
	*/
	string getFileExt(const string& fileName);
	/*
		Utility method to build the full file path from the supplied path and resource folder.
		ResourceType resourceType, Enum specifying the resource type.
		const char* path, Path to file.
	*/
	string buildFilePath(ResourceType resourceType, const char* path);
	/*
		!!DEPRECATED
		Utility method to read a model file.
		Checks the file extension and uses the relevant ModelFileReader implementation.
	*/
	void readModelFile(string fullPath, vector<glm::vec4>& vertices, vector<glm::vec3>& normals, vector<glm::vec2>& textures, vector<unsigned short>& indices, shared_ptr<ModelData>& data, vector<glm::vec4>& points);
	/*
		Utility method to read model file.
	*/
	shared_ptr<ModelData> readModelFile(string fullPath);

	/*
		Utility method to read a collision mesh file.
		Checks the file extension and uses the relevent file reader implementation.
	*/
	void readCollisionFile(string fullPath, shared_ptr<vector<ConvexHull>>& convexHulls);
	static bool initialised;
	static shared_ptr<AssetManager> instance;
	map<string, shared_ptr<Font>> fonts;
	map<string, shared_ptr<Texture>> textures;
	map<std::pair<string, string>, shared_ptr<Shader>> shaders;
	map<string, shared_ptr<ModelData>> modelData;
	map<string, shared_ptr<std::vector<ConvexHull>>> collisionData;
	map<string, string> scripts;
	string fontFolder;
	string levelFolder;
	string modelFolder;
	string textureFolder;
	string shaderFolder;
	string scriptFolder;
	shared_ptr<ModelFileReader> modelFileReader;
};

#endif // !ASSETMANAGER_H

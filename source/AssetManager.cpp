#include "AssetManager.h"

bool AssetManager::initialised = false;
shared_ptr<AssetManager> AssetManager::instance;

shared_ptr<AssetManager> AssetManager::getInstance()
{
	if (initialised) {
		return instance;
	}
	instance = shared_ptr<AssetManager>(new AssetManager());
	initialised = true;
	return instance;
}

shared_ptr<Texture> AssetManager::getTexture(const char * fileName)
{
	string sFileName = string(fileName);
	auto it = textures.find(sFileName);
	if (it != textures.end()) {
		return it->second;
	}
	Bitmap bmp = Bitmap::bitmapFromFile(buildFilePath(ResourceType::TEXTURE, fileName));
	bmp.flipVertically();
	shared_ptr<Texture> ptr = std::make_shared<Texture>(bmp);
	textures.emplace(std::pair<string, shared_ptr<Texture>>(sFileName, ptr));
	return ptr;	
}

shared_ptr<Font> AssetManager::getFont(char * fontName, shared_ptr<Graphics>& graphics)
{
	string sFontPath = string(fontName);
	auto it = fonts.find(sFontPath);
	if (it != fonts.end()) {
		return it->second;
	}
	FT_Library ft;
	FT_Error error = FT_Init_FreeType(&ft);
	string path = buildFilePath(ResourceType::FONT, fontName);
	shared_ptr<Font> fontPtr = std::make_shared<Font>(ft, path.c_str(), graphics);
	fontPtr->compile();
	fonts.emplace(std::pair<string, shared_ptr<Font>>(string(fontName), fontPtr));
	return fontPtr;
}

shared_ptr<Shader> AssetManager::getShader(std::pair<string, string> shaderName)
{
	auto it = shaders.find(shaderName);
	if (it != shaders.end())
	{
		return it->second;
	}
	shared_ptr<Shader> shader = std::make_shared<Shader>();
	shader->compileShader(buildFilePath(ResourceType::SHADER, shaderName.first.c_str()).c_str(), GL_VERTEX_SHADER);
	shader->compileShader(buildFilePath(ResourceType::SHADER, shaderName.second.c_str()).c_str(), GL_FRAGMENT_SHADER);
	shader->link();
	shader->bindShader();
	shaders.emplace(std::pair<std::pair<string, string>, shared_ptr<Shader>>(shaderName, shader));
	return shader;
}

shared_ptr<ModelData> AssetManager::getModelData(const char * fileName, shared_ptr<Graphics> graphics)
{
	auto it = modelData.find(fileName);
	if (it != modelData.end())
	{
		return it->second;
	}
	shared_ptr<ModelData> data = std::make_shared<ModelData>();
	string fullPath = buildFilePath(ResourceType::MODEL, fileName);
	vector<glm::vec4> vertices, points; vector<glm::vec3> normals; vector<glm::vec2> textures; vector<unsigned short>indices;
	readModelFile(fullPath, vertices, normals, textures, indices, data, points);
	data->vboHandles = graphics->bufferModelData(vertices, normals, textures, indices, data->vaoHandle);
	data->indexSize = indices.size();
	data->vertices = vertices;
	data->points = points;
	data->indices = indices;
	modelData.emplace(std::pair<string, shared_ptr<ModelData>>(fileName, data));
	return data;
}

shared_ptr<std::vector<ConvexHull>> AssetManager::getCollisionData(const char * fileName)
{
	auto it = collisionData.find(fileName);
	if (it != collisionData.end())
	{
		return it->second;
	}
	shared_ptr<std::vector<ConvexHull>> data = std::make_shared<std::vector<ConvexHull>>();
	string fullPath = buildFilePath(ResourceType::MODEL, fileName);
	readCollisionFile(fullPath, data);
	collisionData.emplace(std::pair <string, shared_ptr<std::vector<ConvexHull>>>(fileName, data));
	return data;
}

string AssetManager::getScript(const char * fileName)
{
	auto it = scripts.find(fileName);
	if (it != scripts.end())
	{
		return it->second;
	}
	string fullPath = buildFilePath(ResourceType::SCRIPT, fileName);
	scripts.emplace(fileName, fullPath);
	return fullPath;
}

void AssetManager::setAssetFolder(string path, AssetManager::ResourceType resourceType)
{
	switch (resourceType)
	{
	case ResourceType::LEVEL:
		levelFolder = path;
		break;
	case ResourceType::MODEL:
		modelFolder = path;
		break;
	case ResourceType::TEXTURE:
		textureFolder = path;
		break;
	case ResourceType::FONT:
		fontFolder = path;
		break;
	case ResourceType::SHADER:
		shaderFolder = path;
		break;
	case ResourceType::SCRIPT:
		scriptFolder = path;
		break;
	}
}

string AssetManager::getRootFolder(ResourceType resourceType)
{
	switch (resourceType)
	{
	case ResourceType::LEVEL:
		return levelFolder;
		break;
	case ResourceType::MODEL:
		return modelFolder;
		break;
	case ResourceType::TEXTURE:
		return textureFolder;
		break;
	case ResourceType::FONT:
		return fontFolder;
		break;
	case ResourceType::SHADER:
		return shaderFolder;
		break;
	case ResourceType::SCRIPT:
		return scriptFolder;
		break;
	}
}

void AssetManager::exit()
{
	textures.clear();
	fonts.clear();
	shaders.clear();
	modelData.clear();
}

string AssetManager::buildFilePath(ResourceType resourceType, const char * path)
{
	switch (resourceType)
	{
	case ResourceType::LEVEL:
		return string(levelFolder+path);
		break;
	case ResourceType::MODEL:
		return string(modelFolder + path);
		break;
	case ResourceType::TEXTURE:
		return string(textureFolder + path);
		break;
	case ResourceType::FONT:
		return string(fontFolder + path);
		break;
	case ResourceType::SHADER:
		return string(shaderFolder + path);
		break;
	case ResourceType::SCRIPT:
		return string(scriptFolder + path);
	}
}

void AssetManager::readModelFile(string fullPath, vector<glm::vec4>& vertices, vector<glm::vec3>& normals, vector<glm::vec2>& textures, vector<unsigned short>& indices, shared_ptr<ModelData>& data, vector<glm::vec4>& points)
{
	string fileExtension = getFileExt(fullPath);
	if (fileExtension == string("obj")) {
		modelFileReader = std::make_shared<ObjReader>();
		modelFileReader->readFile(fullPath.c_str(), vertices, normals, textures, indices, data->material, points);
	}
	else if (fileExtension == string("dae"))
	{
		modelFileReader = std::make_shared<DaeReader>();
		modelFileReader->readFile(fullPath.c_str(), vertices, normals, textures, indices, data->material, points);
	}
	else if (fileExtension == string("fbx"))
	{
		modelFileReader = std::make_shared<FbxReader>();
		modelFileReader->readFile(fullPath.c_str(), vertices, normals, textures, indices, data->material, points);
	}
	else
	{
		std::cerr << "Unsupported file format" << fullPath << std::endl;
	}
}

void AssetManager::readCollisionFile(string fullPath, shared_ptr<vector<ConvexHull>>& convexHulls)
{
	string fileExtension = getFileExt(fullPath);
	if (fileExtension == string("obj")) {
		modelFileReader = std::make_shared<ObjReader>();
		modelFileReader->readFile(fullPath.c_str(), convexHulls);
	}
	else
	{
		std::cerr << "Unsupported file format" << fullPath << std::endl;
	}
}

string AssetManager::getFileExt(const string& s) {

	size_t i = s.rfind('.', s.length());
	if (i != string::npos) {
		return(s.substr(i + 1, s.length() - i));
	}

	return("");
}

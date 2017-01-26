#include "utils\ObjReader.h"

ObjReader::ObjReader() {

}

ObjReader::~ObjReader()
{
}

void ObjReader::readFile(const char * filePath, vector<glm::vec4>& vertices, vector<glm::vec3>& normals, vector<glm::vec2>& textures, vector<unsigned short>&indices, Material& material)
{
	unpackedVertices.clear();
	unpackedNormals.clear();
	unpackedTextures.clear();
	faceIndices.clear();
	indexVerticeMap.clear();
	std::fstream inStream;
	inStream.open(filePath);
	if (!inStream.is_open()) {
		std::string errorMsg = "Error opening file ";
		errorMsg += filePath;
		throw std::runtime_error(errorMsg);
	}

	std::string newLine;
	while (std::getline(inStream, newLine)) {
		std::string lineStart = newLine.substr(0, 2);
		if (lineStart.length() == 0 || lineStart == "# ") {
			continue;
		}
		std::istringstream stringStream(newLine.substr(2));
		if (lineStart == "mt")
		{
			std::istringstream line(newLine.substr(7));
			std::string materialFile;
			line >> materialFile;
			std::string fullPath = filePath;
			size_t folderIndex = fullPath.find_last_of("/\\");
			std::string path = fullPath.substr(0, folderIndex);
			path += "/" + materialFile;
			readMtl(path.c_str(), material);
		}
		else if (lineStart == "v ") {
			glm::vec4 vertex(readVec3(stringStream), 1.0f);
			unpackedVertices.push_back(vertex);
		}
		else if (lineStart == "vt") {
			float x, y;
			stringStream >> x; stringStream >> y;
			glm::vec2 texture(x, y);
			unpackedTextures.push_back(texture);
		}
		else if (lineStart == "vn") {
			unpackedNormals.push_back(readVec3(stringStream));
		}
		else if (lineStart == "f ") {
			float indexValue;
			glm::vec3 faceVertex;
			while (stringStream >> indexValue)
			{

				faceVertex.x = indexValue-1; //Position index
				int peek = stringStream.peek();
				if (peek == 47) //If position is followed by a /
				{
					stringStream.get(); //Read passed /
					if (stringStream.peek() == 47) //If a second /
					{
						stringStream.get();//read passed it again
						stringStream >> indexValue;
						faceVertex.z = indexValue -1; //Read in normals index
						faceVertex.y = -1; //There was no texture index supplied.
						faceIndices.push_back(faceVertex); //Finished reading this face
					}
					else //There was only 1 / after the position idex
					{
						stringStream >> indexValue; //Read in texture index
						faceVertex.y = indexValue -1;
						int peek = stringStream.peek();
						if (peek == 47) //If there is a / after the texture index
						{
							stringStream.get();
							stringStream >> indexValue; //Read in normal index
							faceVertex.z = indexValue -1;
							faceIndices.push_back(faceVertex); //Finished reading this face
						}
						else
						{
							faceVertex.z = -1; //No normal index supplied
							faceIndices.push_back(faceVertex); //Finished reading this face
						}
					}
				}
			}
		} //end if
	
	}
	unsigned short nextIndex = 0;
	size_t arraySize = faceIndices.size(); //static_cast<int>(faceIndices.size());
	for (int i = 0; i < arraySize; i += 3) {
		for (int j = 0; j < 3; j++)
		{
			glm::vec3 face = faceIndices.at(i + j);
			std::unordered_map<glm::vec3, unsigned short>::iterator it = indexVerticeMap.find(face);
			if (it != indexVerticeMap.end())
			{
				indices.push_back(it->second);
			}
			else
			{
				vertices.push_back(unpackedVertices.at(face.x));
				if (unpackedTextures.size() > 0) {
					textures.push_back(unpackedTextures.at(face.y));
				}
				if (unpackedNormals.size() > 0) {
					normals.push_back(unpackedNormals.at(face.z));
				}
				indices.push_back(nextIndex);
				indexVerticeMap.insert(std::pair<glm::vec3, unsigned short>(face, nextIndex));
				nextIndex++;
			}

		}
	}
}

void ObjReader::readMtl(const char * filePath, Material & material)
{
	std::fstream inStream;
	inStream.open(filePath);
	if (!inStream.is_open()) {
		std::string errorMsg = "Error opening file ";
		errorMsg += filePath;
		std::cerr << errorMsg << ", ignoring." << std::endl;
		material.used = false;
		return;
	}

	std::string newLine;
	while (std::getline(inStream, newLine)) 
	{
		std::string lineStart = newLine.substr(0, 2);
		if (lineStart.length() == 0 || lineStart == "# ") {
			continue;
		}
		std::istringstream stringStream(newLine.substr(2));
		if (lineStart == "Ns")
		{
			stringStream >> material.Ns;
		}
		else if (lineStart == "Ka")
		{
			material.Ka = readVec3(stringStream);
		}
		else if (lineStart == "Kd")
		{
			material.Kd = readVec3(stringStream);
		}
		else if (lineStart == "Ks")
		{
			material.Ks = readVec3(stringStream);
		}
		else if (lineStart == "d")
		{
			stringStream >> material.d;
		}
	}
	material.used = true;
}

glm::vec3 ObjReader::readVec3(std::istringstream& stream)
{
	float x, y, z;
	stream >> x; stream >> y; stream >> z;
	return glm::vec3(x,y,z);
}

#pragma once
#ifndef MODELCOMPONENT_H
#define MODELCOMPONENT_H
#include "Graphics\stb_image.h"
#include "Graphics\Bitmap.h"
#include "Graphics\Texture.h"
#include <Camera\Camera.h>
#include "utils\ObjReader.h"
#include "Renderers\Graphics.h"
#include "GL\glm\glm\glm.hpp"
#include "Shader.h"
#include "Graphics\Transform.h"
#include <vector>
using std::vector;
#include "AssetManager.h"
#include <Graphics\Material.h>
#include "Components\Component.h"
#include "Components\RenderMessage.h"
#include "GameObject.h"

class GameObject;

/*
	Component representing a GameObject's model and texture.
*/
class ModelComponent : public Component {
public:
	/*
		Constructor.
		shared_ptr<Graphics>& graphics, Pointer to the Graphics system.
		std::weak_ptr<GameObject> owner, Weak Pointer to the GameObject that owns this model.
	*/
	ModelComponent(shared_ptr<Graphics>& graphics, std::weak_ptr<GameObject> owner);
	~ModelComponent();
	/*
		Method to load the model data and texture if supplied.
		const char* objFile, The relative path to the model data file from the 'modelLocation' property defined in configuration.
		const char* textureFile, The relative path to the texture file from the 'textureLocation' property defined in configuration. NULL is interprated as not having a texture file.
	*/
	void init(const char* objFile, const char* textureFile, string id = "");
	/*
		Empty implementation of Component::update()
	*/
	void update(double dt);
	/*
		Calls to the graphics system to render this model if the MessageType is RENDER. Ignores all others.
		If the MessageType is RENDER it must be safely castable to a RenderMessage pointer.
		Message* msg, The message to handle.
	*/
	void RecieveMessage(Message* msg);
	
	/*
		Returns a pointer to the models texture. Can be null.
	*/
	shared_ptr<Texture> getTexture();
	/*
		Returns the ID of the VertexArrayObject containing this models data on the GPU.
		If the VertexArrayObject has not been created at this point it will be build and returned here.
	*/
	unsigned int getVertArray();
	/*
		Returns the number of indices of this model.
	*/
	size_t getIndexSize();
	/*
		Returns the models material.
	*/
	Material getMaterial();
	/*
		Returns a Handle to the GameObject's Transform this model belongs to.
	*/
	Handle transformHandle;
	/*
		Returns a pointer to the GameObject's Transform this model belongs to.
	*/
	Transform* getTransform();
	string getId();
	/*
		Toggles the drawing flag to enable and disable rendering of this model.
	*/
	void toggleDrawing();
	/*
		Returns true if drawing enabled.
	*/
	bool isDrawing();
private:
	//Private Methods
	//Calls to Graphics system to render this model. Parameters supplied by RENDER messages. (see RecieveMessage())
	void render(shared_ptr<Camera>& camera);
	void render(shared_ptr<Camera>& camera, vector<Light> lights);
	void render(shared_ptr<Camera>& camera, unsigned int lightingBuffer, unsigned int lightingBlockId);
	//Private members
	bool initalised = false;
	shared_ptr<Graphics> graphics;
	std::weak_ptr<GameObject> owner;
	unsigned int verticesId;
	unsigned int texCoordsId;
	unsigned int normalsId;
	unsigned int indexId;
	unsigned int textureId;

	size_t indexSize;

	vector<unsigned int> vboHandles;
	unsigned int vaoHandle = 0;
	shared_ptr<Shader> shader;
	shared_ptr<Texture> texture;
	Material material;
	string id;
	bool drawing = true;
};
#endif // !MODEL_H

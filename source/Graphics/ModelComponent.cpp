#include "Graphics\ModelComponent.h"

ModelComponent::ModelComponent(shared_ptr<Graphics>& graphics, std::weak_ptr<GameObject> owner) : Component(ComponentType::ComponentTypes::MODEL)
{
	this->graphics = graphics;
	this->owner = owner;
}

ModelComponent::~ModelComponent() {
	texture.reset();
}

void ModelComponent::init(const char * objFile, const char * textureFile, string id)
{
	if (initalised) return;
	shared_ptr<ModelData> modelData = AssetManager::getInstance()->getModelData(objFile, graphics);
	vboHandles = modelData->vboHandles;

	
	// Load the texture
	if (textureFile == NULL)
	{
		textureName = "";
	}
	else
	{
		textureName = textureFile;
		texture = AssetManager::getInstance()->getTexture(textureFile);
	}
		
	indexSize = modelData->indexSize;
	material = modelData->material;

	if (material.used)
	{
		shader = AssetManager::getInstance()->getShader(std::pair<string, string>("phong.vert", "phong.frag"));
	}
	else
	{
		shader = AssetManager::getInstance()->getShader(std::pair<string, string>("basic.vert", "basic.frag"));
	}
	this->id = id;
	initalised = true;
}

void ModelComponent::update(double dt)
{

}

void ModelComponent::RecieveMessage(Message * msg)
{
	MsgType id = msg->id;
	if (id != MsgType::RENDER)
		return;
	else if (!drawing) //Did recieve message to render but disabled so return
		return;
	RenderMessage* renderMsg = ((RenderMessage *)msg); 
	if (renderMsg->lightingBlockId != -1 && renderMsg->lightingBuffer != -1) //Message does contain lightingBlockId and lightingBuffer so use them.
	{
		render(renderMsg->camera, renderMsg->lightingBuffer, renderMsg->lightingBlockId);
		return;
	}
	else if (renderMsg->lights.size() > 0) //Else if Lights have been directly passed, use those.
	{
		render(renderMsg->camera, renderMsg->lights);
		return;
	}
	render(renderMsg->camera); //Else render with no lighting.
}

void ModelComponent::render(shared_ptr<Camera>& camera)
{
	if (!initalised) return;
	graphics->renderModel(*this, shader, camera);
}

void ModelComponent::render(shared_ptr<Camera>& camera, vector<Light> lights)
{
	if (!initalised) return;
	graphics->renderModel(*this, shader, camera, lights);
}

void ModelComponent::render(shared_ptr<Camera>& camera, unsigned int lightingBuffer, unsigned int lightingBlockId)
{
	if (!initalised) return;
	graphics->renderModel(*this, shader, camera, lightingBuffer, lightingBlockId);
}

shared_ptr<Texture> ModelComponent::getTexture()
{
	return texture;
}

unsigned int ModelComponent::getVertArray()
{
	if (vaoHandle != 0) return vaoHandle;
	vaoHandle = graphics->createVertexArrayObject(vboHandles);
	return vaoHandle;
}

size_t ModelComponent::getIndexSize()
{
	return indexSize;
}

Material ModelComponent::getMaterial()
{
	return material;
}

Transform * ModelComponent::getTransform()
{
	auto spOwner = owner.lock();
	if (spOwner != nullptr) 
	{
		return spOwner->getComponent<Transform>(ComponentType::ComponentTypes::TRANSFORM);
	}
	return nullptr;
}

string ModelComponent::getId()
{
	return id;
}

void ModelComponent::toggleDrawing()
{
	drawing = !drawing;
}

bool ModelComponent::isDrawing()
{
	return drawing;
}



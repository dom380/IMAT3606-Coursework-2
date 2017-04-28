#include "Graphics\ModelComponent.h"

ModelComponent::ModelComponent(shared_ptr<Graphics>& graphics, std::weak_ptr<GameObject> owner) : Component(ComponentType::MODEL)
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
	modelData = AssetManager::getInstance()->getModelData(objFile, graphics);	
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
	if (modelData.at(0)->material.used)
	{
		shader = AssetManager::getInstance()->getShader(std::pair<string, string>("phong.vert", "phong.frag"));
		for (auto mesh : modelData)
		{
			materials.push_back(mesh->material);
		}
	}
	else
	{
		shader = AssetManager::getInstance()->getShader(std::pair<string, string>("basic.vert", "basic.frag"));
	}
	this->objFileName = objFile;
	this->id = id;
	initalised = true;
}

void ModelComponent::update(double dt)
{

}

void ModelComponent::RecieveMessage(Message * msg)
{
	MsgType id = msg->id;
	if (!drawing) //Did recieve message to render but disabled so return
		return;
	switch (id)
	{
		case MsgType::RENDER :
		{
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
		break;
		case MsgType::MATERIAL:
		{
			MaterialMessage* matMsg = ((MaterialMessage *)msg);
			for (int i = 0; i < materials.size(); ++i)
			{
				materials.at(i).Kd = matMsg->material.Kd;
				materials.at(i).Ka = matMsg->material.Ka;
			}
		}
		break;
	}
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

Transform * ModelComponent::getTransform()
{
	auto spOwner = owner.lock();
	if (spOwner != nullptr) 
	{
		return spOwner->getComponent<Transform>(ComponentType::TRANSFORM);
	}
	return nullptr;
}

string ModelComponent::getObjFileName()
{
	return objFileName;
}

string ModelComponent::getId()
{
	return id;
}

void ModelComponent::setID(string passedId)
{
	id = passedId;
}

void ModelComponent::toggleDrawing()
{
	drawing = !drawing;
}

bool ModelComponent::isDrawing()
{
	return drawing;
}

vector<shared_ptr<ModelData>> ModelComponent::getData()
{
	return modelData;
}

vector<Material> ModelComponent::getMaterials()
{
	return materials;
}



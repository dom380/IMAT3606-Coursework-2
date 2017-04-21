#pragma once

#include <memory>
#include "Renderers\Graphics.h"
#include <string>
#include "Component.h"
#include "RenderMessage.h"
#include "Graphics\Transform.h"
#include "Animation/SkeletalModel.h"
#include "GameObject.h"
#include "Graphics\stb_image.h"
#include <Camera\Camera.h>
#include "Renderers\Graphics.h"
#include "GL\glm\glm\glm.hpp"
#include "Graphics/Shader.h"
#include <vector>
using std::vector;
#include "AssetManager.h"
#include "Components\Component.h"

class AnimatedModelComponent : public Component
{
public:
	AnimatedModelComponent(shared_ptr<Graphics>& graphics, std::weak_ptr<GameObject> owner) : Component(ComponentType::ANIMATION)
	{
		this->graphics = graphics;
		this->owner = owner;
	}

	~AnimatedModelComponent()
	{

	}

	//need to know what models to pass through
	void init(std::vector<const char*> meshFiles, const char* textureFile, string id = "")
	{
		//model->LoadMesh(objFile, false);
		shader = AssetManager::getInstance()->getShader(std::make_pair("animation.vert", "animation.frag"));

		for (size_t i = 0; i < meshFiles.size(); i++)
		{
			models.push_back(std::make_shared<SkeletalModel>(shader));
			models.at(i)->LoadMesh(meshFiles[i]);
		}	
	}

	void update(double dt)
	{

	}

	void RecieveMessage(Message* msg)
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

	Transform * getTransform()
	{
		auto spOwner = owner.lock();
		if (spOwner != nullptr)
		{
			return spOwner->getComponent<Transform>(ComponentType::TRANSFORM);
		}
		return nullptr;
	}

	//text file of model animations and simply read those for list
	std::vector<shared_ptr<SkeletalModel>> models;
private:
	

	//TO DO
	void render(shared_ptr<Camera>& camera)
	{
		//if (!initalised) return;
		graphics->renderModel(*this, shader, camera);
	}

	//TO DO
	void render(shared_ptr<Camera>& camera, vector<Light> lights)
	{
		//if (!initalised) return;
		graphics->renderModel(*this, shader, camera, lights);
	}

	//TO DO
	void render(shared_ptr<Camera>& camera, unsigned int lightingBuffer, unsigned int lightingBlockId)
	{
		//if (!initalised) return;
		graphics->renderModel(*this, shader, camera, lightingBuffer, lightingBlockId);
	}

	shared_ptr<Graphics> graphics;
	std::weak_ptr<GameObject> owner;
	shared_ptr<Shader> shader;

	//vector<GLuint> vboHandles;
	//GLuint vaoHandle = 0;

	bool drawing = true;
};
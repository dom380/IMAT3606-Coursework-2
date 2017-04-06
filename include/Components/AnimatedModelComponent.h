#pragma once

#include <memory>
#include "Renderers\Graphics.h"
#include <string>
#include "Component.h"
#include "RenderMessage.h"

#include "Animation/SkeletalModel.h"

class GameObject;

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
		//text file of model animations and simply read those for list

		std::vector<shared_ptr<SkeletalModel>> models;

		//model->LoadMesh(objFile, false);
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

private:
	//TO DO
	void render(shared_ptr<Camera>& camera)
	{
		//if (!initalised) return;
		//graphics->renderModel(*this, shader, camera);
	}

	//TO DO
	void render(shared_ptr<Camera>& camera, vector<Light> lights)
	{
		//if (!initalised) return;
		//graphics->renderModel(*this, shader, camera, lights);
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

	bool drawing = true;
};
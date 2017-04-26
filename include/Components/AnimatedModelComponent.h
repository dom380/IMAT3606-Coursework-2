#pragma once
#ifndef ANIMATEDMODELCOMPONENT_H
#define ANIMATEDMODELCOMPONENT_H
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
#include "GL\glm\glm\glm.hpp"
#include "Graphics/Shader.h"
#include <vector>
#include <map>
using std::vector;
#include "AssetManager.h"
#include "Components\Component.h"

class GameObject;

class AnimatedModelComponent : public Component
{
public:
	AnimatedModelComponent(shared_ptr<Graphics>& graphics, std::weak_ptr<GameObject> owner);

	~AnimatedModelComponent();

	//need to know what models to pass through
	void init(const char* defaultAnim, std::vector<std::pair<const char*, const char*>> meshFiles, const char* textureFile, string id = "");

	void update(double dt);

	void RecieveMessage(Message* msg);

	Transform * getTransform();

	//text file of model animations and simply read those for list

	

	std::shared_ptr<SkeletalModel> getCurrentModel()
	{
		return models.at(currentAnim);
	}

private:
	std::map<std::string, std::shared_ptr<SkeletalModel>> models;

	//TO DO
	void render(shared_ptr<Camera>& camera);

	//TO DO
	void render(shared_ptr<Camera>& camera, vector<Light> lights);

	//TO DO
	void render(shared_ptr<Camera>& camera, unsigned int lightingBuffer, unsigned int lightingBlockId);

	shared_ptr<Graphics> graphics;
	std::weak_ptr<GameObject> owner;
	shared_ptr<Shader> shader;

	//vector<GLuint> vboHandles;
	//GLuint vaoHandle = 0;

	bool drawing = true;
	std::string currentAnim;
};

#endif // !MODEL_H
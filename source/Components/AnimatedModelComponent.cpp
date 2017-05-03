#include "Components/AnimatedModelComponent.h"

AnimatedModelComponent::AnimatedModelComponent(shared_ptr<Graphics>& graphics, std::weak_ptr<GameObject> owner) : Component(ComponentType::ANIMATION)
{
	this->graphics = graphics;
	this->owner = owner;
}

AnimatedModelComponent::~AnimatedModelComponent()
{

}

//need to know what models to pass through
void AnimatedModelComponent::init(const char* defaultAnim, std::vector<std::pair<const char*, const char*>> meshFiles, const char* textureFile, string id)
{
	shader = AssetManager::getInstance()->getShader(std::make_pair("animation.vert", "animation.frag"));

	for (size_t i = 0; i < meshFiles.size(); i++)
	{
		//need name from loader
		models.insert(std::make_pair(meshFiles.at(i).first, std::make_shared<SkeletalModel>(shader)));
		models.at(meshFiles.at(i).first)->LoadMesh(meshFiles.at(i).second);
	}

	currentAnim = defaultAnim;
}

void AnimatedModelComponent::update(double dt)
{
	auto it = models.begin();
	for (it; it != models.end(); ++it)
	{
		it->second->interval = dt;
	}
}

void AnimatedModelComponent::RecieveMessage(Message* msg)
{
	MsgType id = msg->id;
	switch (id)
	{
		case MsgType::RENDER:
		{
			if (!drawing) //Did recieve message to render but disabled so return
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
		break;
		case MsgType::ANIMATION:
		{
			if (!drawing) 
				return;
			AnimMessage* animMsg = ((AnimMessage *)msg);
			//currentAnim = animMsg->animation;
			auto it = models.find(animMsg->animation);
			if (it != models.end())
			{
				currentAnim = animMsg->animation;
			}
		}
		break;
	}
	


	//have a message to change anim
	
}

Transform * AnimatedModelComponent::getTransform()
{
	auto spOwner = owner.lock();
	if (spOwner != nullptr)
	{
		return spOwner->getComponent<Transform>(ComponentType::TRANSFORM);
	}
	return nullptr;
}

void AnimatedModelComponent::dispose()
{
	graphics->freeAnimatedModel(*this);
}

//TO DO
void AnimatedModelComponent::render(shared_ptr<Camera>& camera)
{
	//if (!initalised) return;
	graphics->renderModel(*this, shader, camera);
}

//TO DO
void AnimatedModelComponent::render(shared_ptr<Camera>& camera, vector<Light> lights)
{
	//if (!initalised) return;
	graphics->renderModel(*this, shader, camera, lights);
}

//TO DO
void AnimatedModelComponent::render(shared_ptr<Camera>& camera, unsigned int lightingBuffer, unsigned int lightingBlockId)
{
	//if (!initalised) return;

	graphics->renderModel(*this, shader, camera, lightingBuffer, lightingBlockId);
}

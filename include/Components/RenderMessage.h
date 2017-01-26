#pragma once
#ifndef RENDERMESSAGE_H
#define RENDERMESSAGE_H
#include "Message.h"
#include "Camera\Camera.h"
#include "Graphics\Light.h"
#include <memory>
using std::shared_ptr;
#include <vector>
using std::vector;
/*
	Implementation of the Message interface that supplies 
	camera and lighting data.
*/
class RenderMessage : public Message
{
public:
	/*
		Constructor
		shared_ptr<Camera> cam, Pointer to the active camera.
	*/
	RenderMessage(shared_ptr<Camera> cam) : Message(MsgType::RENDER)
	{
		camera = cam;
	}
	/*
		Constructor
		shared_ptr<Camera> cam, Pointer to the active camera.
		vector<Light> sceneLights, A std::vector of the light sources to render with.
	*/
	RenderMessage(shared_ptr<Camera> cam, vector<Light> sceneLights) : Message(MsgType::RENDER)
	{
		camera = cam;
		lights = sceneLights;
	}
	/*
		Constructor
		shared_ptr<Camera> cam, Pointer to the active camera.
		unsigned int lightingBuffer, The Id of the uniform buffer object containing the lighting data.
		unsigned int lightingBlockId, The lighting uniform block binding point.
	*/
	RenderMessage(shared_ptr<Camera> cam, unsigned int lightingBuffer, unsigned int lightingBlockId) : Message(MsgType::RENDER)
	{
		camera = cam;
		this->lightingBuffer = lightingBuffer;
		this->lightingBlockId = lightingBlockId;
	}
	shared_ptr<Camera> camera;
	vector<Light> lights;
	unsigned int lightingBuffer = -1, lightingBlockId = -1;
};

#endif // !RENDERMESSAGE_H


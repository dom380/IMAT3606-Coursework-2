#include "..\..\include\GUI\UIElement.h"
#ifndef NDEBUG
#include <utils\GLSupport.h>
#endif

UIElement::UIElement(shared_ptr<Graphics>& engineGraphics, shared_ptr<Transform>& passedTransform, const char * id, const char * textureName)
{
	haveVAO = false;
	graphics = engineGraphics;
	transform = passedTransform;
	uiID = id;	
	glm::mat4 projection = glm::ortho(0.0f, (float)graphics->getWidth(), 0.0f, (float)graphics->getHeight());
	shader->setUniform("projection", projection);
}

string UIElement::getID()
{
	return uiID;
}

void UIElement::setID(string ID)
{
	uiID = ID;
}

shared_ptr<Transform> UIElement::getTransform()
{
	return transform;
}

GLuint UIElement::getVertArray()
{
	return vaoHandle;
}

UIType UIElement::getType()
{
	return type;
}

glm::mat4 UIElement::getModel()
{
	return model;
}

void UIElement::setModel(glm::mat4 passedModel)
{
	model = passedModel;
}

void UIElement::updateModelUsingTransform(shared_ptr<Transform> passedTransform)
{
	if (passedTransform)
	{
		model[3][0] = passedTransform->position[0];
		model[3][1] = passedTransform->position[1];
		model[0][0] = passedTransform->scale[0];
		model[1][1] = passedTransform->scale[1];
		model = glm::rotate(model, glm::radians(passedTransform->orientation[2]), glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else
	{
		model[3][0] = transform->position[0];
		model[3][1] = transform->position[1];
		model[0][0] = transform->scale[0];
		model[1][1] = transform->scale[1];
		float angle = glm::angle(transform->orientation);
		model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	
}

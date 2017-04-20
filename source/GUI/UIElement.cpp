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
	// Load the texture
	if (textureName == NULL)
	{
	}
	else
	{
		texture = AssetManager::getInstance()->getTexture(textureName);
		
	}
	init();
	//Set texture
	glActiveTexture(GL_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, texture->object());
	shader->setUniform("tex", 0);
	//Set proj
	//glm::mat4 projection = glm::ortho(0.0f, (float)Engine::g_pEngine->getWindowWidth(), 0.0f, (float)Engine::g_pEngine->getWindowHeight());
	glm::mat4 projection = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f);
	shader->setUniform("projection", projection);
}

void UIElement::init()
{
	shader = AssetManager::getInstance()->getShader(std::pair<string, string>("ui.vert", "ui.frag"));
	shader->bindShader();
	vertices = {
		// Positions          // Colors           // Texture Coords
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // Top Right
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // Bottom Right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // Bottom Left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // Top Left 
	};
	indices = {  // Note that we start from 0!
		0, 1, 3, // First Triangle
		1, 2, 3  // Second Triangle
	};

	glGenVertexArrays(1, &vaoHandle);
	glGenBuffers(1, &vboHandle);
	glGenBuffers(1, &eboHandle);

	glBindVertexArray(vaoHandle);

	glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboHandle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO
}

void UIElement::render()
{
	if (!haveVAO) { //If VAO hasn't been created (on this thread) yet create it now.
		vaoHandle = graphics->createUIVertextArrayObject(vboHandle, eboHandle, vertices, indices);
		haveVAO = true;
	}
	graphics->renderUI(*this, shader);
}

string UIElement::getID()
{
	return uiID;
}

shared_ptr<Transform> UIElement::getTransform()
{
	return transform;
}

shared_ptr<Texture> UIElement::getTexture()
{
	return texture;
}

GLuint UIElement::getVertArray()
{
	return vaoHandle;
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
	}
	else
	{
		model[3][0] = transform->position[0];
		model[3][1] = transform->position[1];
		model[0][0] = transform->scale[0];
		model[1][1] = transform->scale[1];
	}
}

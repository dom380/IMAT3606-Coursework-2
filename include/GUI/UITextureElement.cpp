#include "UITextureElement.h"

UITextureElement::UITextureElement(shared_ptr<Graphics>& engineGraphics, shared_ptr<Transform>& passedTransform, const char * id, const char * passedTextureName)
{
	haveVAO = false;
	graphics = engineGraphics;
	transform = passedTransform;
	updateModelUsingTransform();
	type = UIType::TEXTURE;
	uiID = id;
	textureName = passedTextureName;
	// Load the texture
	if (textureName == "")
	{
	}
	else
	{
		texture = AssetManager::getInstance()->getTexture(textureName.c_str());
	}
	init();
	//Set texture
	glActiveTexture(GL_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, texture->object());
	shader->setUniform("tex", 0);
	//Set proj
	glm::mat4 projection = glm::ortho(0.0f, (float)graphics->getWidth(), 0.0f, (float)graphics->getHeight());
	shader->setUniform("projection", projection);
}

void UITextureElement::init()
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

void UITextureElement::render()
{
	if (!haveVAO)
	{
		vaoHandle = graphics->createUIVertextArrayObject(vboHandle, eboHandle, vertices, indices);
		haveVAO = true;
	}
		
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	shader->bindShader();

#ifndef NDEBUG
	//string check = OpenGLSupport().GetError();
#endif
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->object());
	shader->setUniform("tex", 0);

	shader->setUniform("model", model);
	glBindVertexArray(vaoHandle);

#ifndef NDEBUG
	//check = OpenGLSupport().GetError();
#endif
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}

shared_ptr<Texture> UITextureElement::getTexture()
{
	return texture;
}

string UITextureElement::getTextureName()
{
	return textureName;
}

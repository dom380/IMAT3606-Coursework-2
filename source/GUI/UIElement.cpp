#include "..\..\include\GUI\UIElement.h"
#ifndef NDEBUG
#include <utils\GLSupport.h>
#endif

UIElement::UIElement(const char * id, const char * textureName)
{
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
	GLfloat vertices[] = {
		// Positions          // Colors           // Texture Coords
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // Top Right
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // Bottom Right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // Bottom Left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // Top Left 
	};
	GLuint indices[] = {  // Note that we start from 0!
		0, 1, 3, // First Triangle
		1, 2, 3  // Second Triangle
	};

	GLuint VBO, EBO;
	glGenVertexArrays(1, &vaoHandle);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(vaoHandle);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	shader->bindShader();

	glm::mat4 projection = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f);
	shader->setUniform("projection", projection);
	
#ifndef NDEBUG
	string check = OpenGLSupport().GetError();
#endif
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->object());
	shader->setUniform("tex", 0);
	glBindVertexArray(vaoHandle);

#ifndef NDEBUG
	check = OpenGLSupport().GetError();
#endif
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

}

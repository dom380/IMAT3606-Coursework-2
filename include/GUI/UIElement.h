#pragma once
#ifndef UIELEMENT_H
#define UIELEMENT_H

#include <vector>
#include <string>

#include <Graphics\Shader.h>
#include "Graphics\stb_image.h"
#include "Graphics\Bitmap.h"
#include "Graphics\Texture.h"
#include "GL\glm\glm\gtc\matrix_transform.hpp"
#include "GL\glm\glm\gtx\transform.hpp"
#include "AssetManager.h"
#include "Renderers\Graphics.h"
//#include <Engine.h>
using namespace std;

class UIElement
{
private:
	bool haveVAO;
	vector<GLfloat> vertices;
	vector<GLuint> indices;
	string uiID;
	shared_ptr<Texture> texture;
	GLuint vaoHandle;
	GLuint vboHandle;
	GLuint eboHandle;
	shared_ptr<Shader> shader;
	shared_ptr<Graphics> graphics;
	shared_ptr<Transform> transform;
	glm::mat4 model;

public:
	UIElement(shared_ptr<Graphics>& graphics, shared_ptr<Transform>& transform,const char* id, const char* textureName);
	void init();
	void render();

	string getID();
	shared_ptr<Transform> getTransform();
	shared_ptr<Texture> getTexture();
	GLuint getVertArray();

	glm::mat4 getModel();
	void setModel(glm::mat4 model);
	void updateModelUsingTransform(shared_ptr<Transform> transform=NULL);

};
#endif
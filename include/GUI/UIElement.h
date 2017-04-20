#pragma once
#ifndef UIELEMENT_H
#define UIELEMENT_H

#include <vector>
#include <string>
#include <Graphics\Shader.h>
#include "GL\glm\glm\gtc\matrix_transform.hpp"
#include "GL\glm\glm\gtx\transform.hpp"
#include "AssetManager.h"
#include "Renderers\Graphics.h"

using namespace std;

class UIElement
{
protected:
	bool haveVAO;
	vector<GLfloat> vertices;
	vector<GLuint> indices;
	string uiID;
	//shared_ptr<Texture> texture;
	GLuint vaoHandle;
	GLuint vboHandle;
	GLuint eboHandle;
	shared_ptr<Shader> shader;
	shared_ptr<Graphics> graphics;
	shared_ptr<Transform> transform;
	glm::mat4 model;

public:
	UIElement() {};
	UIElement(shared_ptr<Graphics>& graphics, shared_ptr<Transform>& transform,const char* id, const char* textureName);
	virtual void init() = 0;
	virtual void render() = 0;

	string getID();
	shared_ptr<Transform> getTransform();
	//shared_ptr<Texture> getTexture();
	GLuint getVertArray();

	glm::mat4 getModel();
	void setModel(glm::mat4 model);
	void updateModelUsingTransform(shared_ptr<Transform> transform=NULL);

};
#endif
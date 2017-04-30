#pragma once
#ifndef UIELEMENT_H
#define UIELEMENT_H

#include <vector>
#include <string>
#include <GUI\UIType.h>
#include <GUI\Button.h>
#include <Graphics\Shader.h>
#include "GL\glm\glm\gtc\matrix_transform.hpp"
#include "GL\glm\glm\gtx\transform.hpp"
#include "AssetManager.h"
#include "Renderers\Graphics.h"

using namespace std;

class UIElement
{
protected:
	//This determines whether it should be saved to file or not
	bool active;
	shared_ptr<Button> button;
	bool haveVAO;
	vector<GLfloat> vertices;
	vector<GLuint> indices;
	string uiID;
	GLuint vaoHandle;
	GLuint vboHandle;
	GLuint eboHandle;
	shared_ptr<Shader> shader;
	shared_ptr<Graphics> graphics;
	shared_ptr<Transform> transform;
	glm::mat4 model;
	UIType type;
public:
	UIElement() {};
	UIElement(shared_ptr<Graphics>& graphics, shared_ptr<Transform>& transform,const char* id, const char* textureName);
	virtual void init() = 0;
	virtual void render() = 0;

	//This determines whether it should be saved to file or not
	void setActive(bool isActive);
	bool isActive();

	/*
	Returns a button
	*/
	shared_ptr<Button> getButton();
	/*
	Set button
	*/
	void setButton(shared_ptr<Button> passedButton);
	string getID();
	void setID(string ID);
	shared_ptr<Transform> getTransform();
	GLuint getVertArray();
	UIType getType();
	glm::mat4 getModel();
	void setModel(glm::mat4 model);
	void updateModelUsingTransform(shared_ptr<Transform> transform=NULL);

	void dispose();
};
#endif
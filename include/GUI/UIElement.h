#pragma once
#ifndef UIELEMENT_H
#define UIELEMENT_H

#include <vector>
#include <string>

#include <Editor\DebugMenuItem.h>
#include <Graphics\Shader.h>
#include "Graphics\stb_image.h"
#include "Graphics\Bitmap.h"
#include "Graphics\Texture.h"
#include "GL\glm\glm\gtc\matrix_transform.hpp"
#include "GL\glm\glm\gtx\transform.hpp"
#include "AssetManager.h"
//#include <Engine.h>
using namespace std;

class UIElement
{
private:
	string uiID;
	shared_ptr<Texture> texture;
	GLuint vaoHandle;
	shared_ptr<Shader> shader;
public:
	UIElement(const char* id, const char* textureName);
	void init();
	void render();

};
#endif
#pragma once
#ifndef UITEXTUREELEMENT_H
#define UITEXTUREELEMENT_H

#include <vector>
#include <string>

#include <GUI\UIElement.h>
#include "Graphics\stb_image.h"
#include "Graphics\Bitmap.h"
#include "Graphics\Texture.h"

using namespace std;

class UITextureElement : public UIElement
{
private:
	shared_ptr<Texture> texture;
	string textureName;
public:
	UITextureElement(shared_ptr<Graphics>& graphics, shared_ptr<Transform>& transform, const char* id, const char* textureName);
	void init();
	void render();
	shared_ptr<Texture> getTexture();
	string getTextureName();
	void setTexture(string textureName);
};
#endif
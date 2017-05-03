#include "..\..\include\GUI\TextBox.h"

TextBox::TextBox(string text, Font textfont, shared_ptr<Transform> pos, shared_ptr<Graphics>& graphics, glm::vec3& colour, string id)
{
	this->text = text;
	this->graphics = graphics;
	this->textColour = colour;
	this->uiID = id;
	this->font = textfont;
	this->transform = pos;
	this->charX = pos->position.x;
	this->charY = pos->position.y;
	this->active = true;
	this->savable = true;
	init();
}

TextBox::TextBox(const char * text, Font textfont, shared_ptr<Transform> pos, shared_ptr<Graphics>& graphics, glm::vec3& colour, string id)
{
	this->text = string(text);
	this->graphics = graphics;
	this->textColour = colour;
	this->uiID = id;
	this->font = textfont;
	this->transform = pos;
	this->charX = pos->position.x;
	this->charY = pos->position.y;
	this->active = true;
	this->savable = true;
	init();
}

TextBox & TextBox::operator=(TextBox & other)
{
	this->charX = other.charX;
	this->charY = other.charY;
	this->text = other.text;
	this->vaoHandle = other.vaoHandle;
	this->vboHandle = other.vboHandle;
	this->shader = other.shader;
	this->transform = other.transform;
	this->font = other.font;
	this->graphics = other.graphics;
	this->textColour = other.textColour;
	this->active = other.active;
	this->savable = other.savable;
	return *this;
}

void TextBox::updateText(string newText)
{
	text = newText;
}

void TextBox::render()
{
	if (!active)
		return;
	if (!haveVAO) { //If VAO hasn't been created (on this thread) yet create it now.
		vaoHandle = graphics->createTextVertexArrayObject(vboHandle);
		haveVAO = true;
	} 
	graphics->renderText(text, font, transform, vaoHandle, vboHandle, shader, textColour);
}

string TextBox::getText()
{
	return text;
}

void TextBox::setFont(Font passedFont)
{
	font = passedFont;
}

Font TextBox::getFont()
{
	return font;
}

glm::vec3 TextBox::getColour()
{
	return textColour;
}

void TextBox::setColour(glm::vec3 colour)
{
	textColour = colour;
}

void TextBox::init()
{
	type = UIType::TEXT;
	graphics->buildTextShader(vaoHandle, vboHandle, shader);
}

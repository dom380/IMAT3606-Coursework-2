#include "..\..\include\GUI\TextBox.h"

TextBox::TextBox(string text, Font textfont, shared_ptr<Transform> pos, shared_ptr<Graphics>& graphics, glm::vec3& colour, string id)
{
	this->text = text;
	this->graphics = graphics;
	this->textColour = colour;
	this->id = id;
	init(textfont, pos);
}

TextBox::TextBox(const char * text, Font textfont, shared_ptr<Transform> pos, shared_ptr<Graphics>& graphics, glm::vec3& colour, string id)
{
	this->text = string(text);
	this->graphics = graphics;
	this->textColour = colour;
	this->id = id;
	init(textfont, pos);
}

TextBox & TextBox::operator=(TextBox & other)
{
	this->charX = other.charX;
	this->charY = other.charY;
	this->text = other.text;
	this->VAO = other.VAO;
	this->VBO = other.VBO;
	this->textShader = other.textShader;
	this->transform = other.transform;
	this->font = other.font;
	this->graphics = other.graphics;
	this->textColour = other.textColour;
	return *this;
}

void TextBox::updateText(string newText)
{
	text = newText;
}

void TextBox::render()
{
	if (!haveVAO) { //If VAO hasn't been created (on this thread) yet create it now.
		VAO = graphics->createTextVertexArrayObject(VBO);
		haveVAO = true;
	} 
	graphics->renderText(text, font, transform, VAO, VBO, textShader, textColour);
}

string TextBox::getId()
{
	return id;
}

void TextBox::init(Font textfont, shared_ptr<Transform> pos)
{
	font = textfont;
	transform = pos;
	charX = pos->position.x;
	charY = pos->position.y;
	graphics->buildTextShader(VAO, VBO, textShader);
}

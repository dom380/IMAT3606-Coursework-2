#include "GUI/Button.h"
#include <gl\glm\glm\gtc\matrix_transform.hpp>
#include <algorithm> 

#ifndef NDEBUG
#include <utils\GLSupport.h>
#endif

Button::Button(string text, Font textfont, Transform pos, Graphics* graphics)
{
	this->text = text;
	this->graphics = graphics;
	init(textfont, pos);
}

Button::Button(const char * text, Font textfont, Transform pos, Graphics* graphics)
{
	this->text = string(text);
	this->graphics = graphics;
	init(textfont, pos);
}

Button & Button::operator=(Button& other)
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
	return *this;
}

//Button & Button::operator()(string text, Font textfont, Transform pos, Graphics* graphics)
//{
//	this->text = text;
//	this->graphics = graphics;
//	init(textfont, pos);
//	return *this;
//}
//
//Button & Button::operator()(const char * text, Font textfont, Transform pos, Graphics* graphics)
//{
//	this->text = string(text);
//	this->graphics = graphics;
//	init(textfont, pos);
//	return *this;
//}

void Button::init(Font textfont, Transform pos)
{
	font = textfont;
	transform = pos;
	charX = pos.position.x;
	charY = pos.position.y;
	graphics->buildTextShader(VAO, VBO, textShader);
	buildAABB();
}

void Button::buildAABB()
{
	aabb.x = transform.position.x;
	aabb.y = transform.position.y;
	aabb.width = 0;
	for (char c : text) {
		Font::Character character = font.getChar(c);
		aabb.y = std::min((transform.position.y - (character.size.y - character.bearing.y) * transform.scale.y), aabb.y);
		aabb.height = std::max(character.size.y * transform.scale.y, aabb.height);
		aabb.width += (character.offset >> 6) *  transform.scale.x;
	}
}

void Button::handle(Event event)
{
	//if not a click event we don't care
	if (event.eventType != EventType::CLICK) {
		return;
	}
	//if this button was clicked
	int x = event.x, y = graphics->getHeight()-event.y;
	if ((x >= aabb.x) && (x <= aabb.x + aabb.width) && (y >= aabb.y) && (y <= aabb.y + aabb.height))
	{
		//onClick()
		std::cout << "Button Clicked!" << std::endl;
	}
}


template<typename OnClick>
void Button<OnClick>::addOnClickFn(OnClick c)
{ 
	onClick = c;
}

void Button::render()
{
	graphics->renderText(text, font, transform, VAO, VBO, textShader);
}


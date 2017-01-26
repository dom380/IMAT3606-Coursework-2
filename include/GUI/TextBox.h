#pragma once
#ifndef TEXTBOX_H
#define TEXTBOX_H
#include "Renderers\Graphics.h"
#include "Font.h"
#include <Graphics\Transform.h>
#include <Graphics\Shader.h>
#include <string>
using std::string;
/*
	UI element that displays text.
*/
class TextBox {
public:
	//Constructors
	TextBox() {};
	/*
	Constructor
		string text, The text to display for the Textbox.
		Font textfont, The font to use for the text.
		shared_ptr<Transform>& pos, The position, scale and orientation of the Textbox.
		shared_ptr<Graphics>& graphics, Pointer to the graphics system.
		glm::vec3& colour, The colour of the text, normalised to 0..1. Defaulted to white (1.0,1.0,1.0).
		string id, The Textbox's Id.
	*/
	TextBox(string text, Font textfont, shared_ptr<Transform> pos, shared_ptr<Graphics>& graphics, glm::vec3& colour = glm::vec3(1.0, 1.0, 1.0), string id = "");
	/*
		Constructor
		const char* text, The text to display for the Textbox.
		Font textfont, The font to use for the text.
		shared_ptr<Transform>& pos, The position, scale and orientation of the Textbox.
		shared_ptr<Graphics>& graphics, Pointer to the graphics system.
		glm::vec3& colour, The colour of the text, normalised to 0..1. Defaulted to white (1.0,1.0,1.0).
		string id, The Textbox's Id.
	*/
	TextBox(const char* text, Font textfont, shared_ptr<Transform> pos, shared_ptr<Graphics>& graphics, glm::vec3& colour = glm::vec3(1.0, 1.0, 1.0), string id = "");
	~TextBox() {};
	/*
		Copy Constructor.
	*/
	TextBox& operator=(TextBox& other);
	/*
		Changes the current text to the new string.
		string newText, Replacement text.
	*/
	void updateText(string newText);
	/*
		Call to the graphics system to render this text.
	*/
	void render();
	string getId();
private:
	//private methods
	/*
		Initialisation method. Calls to the graphics system to build a Text shader.
	*/
	void init(Font textfont, shared_ptr<Transform> pos);
	//private memebers
	string text;
	Font font;
	shared_ptr<Shader> textShader;
	shared_ptr<Transform> transform;
	unsigned int VAO, VBO;
	shared_ptr<Graphics> graphics;
	float charX, charY; //Current position of next character to render
	glm::vec3 textColour;
	bool haveVAO = false; //Flag to specify is a Vertex Array Object has be created for the Textbox.
	string id;
};

#endif // !TEXTBOX_H


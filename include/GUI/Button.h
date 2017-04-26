#pragma once
#ifndef BUTTON_H
#define BUTTON_H
#include <string>
using std::string;
#include "EventListener.h"
#include "Font.h"
#include "Graphics\Shader.h"
#include <Graphics\Transform.h>
#include <GUI\TextBox.h>
#include <Renderers\Graphics.h>
#include <gl\glm\glm\gtc\matrix_transform.hpp>
#include <algorithm> 
#include <functional>
#include <memory>
using std::shared_ptr;


/*
	Button UI element
*/
class Button : public EventListener {
private:
	struct AABB {
		float x, y, width, height;
	};
	AABB aabb;
	shared_ptr<Graphics> graphics;
	shared_ptr<TextBox> textbox;

	std::function<void()> onClickCallback;

	std::string id;

	/*
		Inititalise the Button, creating a Textbox for rendering and building an AABB around the text.
	*/
	void init(Font textfont, shared_ptr<Transform> pos, glm::vec3& colour = glm::vec3(1.0, 1.0, 1.0)) {
		textbox = std::make_shared<TextBox>(text, textfont, pos, graphics);
		font = textfont;
		transform = pos;
		buildAABB();
	};

	/*
		Calculates a bounding box around the button text.
	*/
	void buildAABB() {
		float maxHeight = 0;
		aabb.x = transform->position.x;
		aabb.y = transform->position.y;
		aabb.width = 0;
		for (char c : text) { //For each character in text
			Font::Character character = font.getChar(c);
			aabb.y = std::min((transform->position.y - (character.size.y - character.bearing.y) * transform->scale.y), aabb.y); //Set button position of AABB to the current lowest (From top left) character bearing.
			maxHeight = std::max(character.size.y * transform->scale.y, maxHeight); //Get current tallest character
			aabb.width += (character.offset >> 6) *  transform->scale.x; //increment width for each character
		}
		aabb.height = (transform->position.y - aabb.y) + maxHeight; //Height of AABB is the maximum character height add the maximum underline bearing
	};
	
protected:
	string text;
	Font font;
	shared_ptr<Transform> transform;
public:
	//Constructors
	Button() {};
	/*
		Constructor
		string text, The text to display for the button.
		Font textfont, The font to use for the text.
		shared_ptr<Transform>& pos, The position, scale and orientation of the button.
		shared_ptr<Graphics>& graphics, Pointer to the graphics system.
		glm::vec3& colour, The colour of the text, normalised to 0..1. Defaulted to white (1.0,1.0,1.0), 
		string id, The button#s Id.
	*/
	Button(string text, Font textfont, shared_ptr<Transform>& pos, shared_ptr<Graphics>& graphics, glm::vec3& colour = glm::vec3(1.0,1.0,1.0), string id ="") {
		this->text = text;
		this->graphics = graphics;
		this->id = id;
		init(textfont, pos, colour);
	};
	/*
		Constructor
		const char* text, The text to display for the button.
		Font textfont, The font to use for the text.
		shared_ptr<Transform>& pos, The position, scale and orientation of the button.
		shared_ptr<Graphics>& graphics, Pointer to the graphics system.
		glm::vec3& colour, The colour of the text, normalised to 0..1. Defaulted to white (1.0,1.0,1.0),
		string id, The button#s Id.
	*/
	Button(const char* text, Font textfont, shared_ptr<Transform>& pos, shared_ptr<Graphics>& graphics, glm::vec3& colour = glm::vec3(1.0, 1.0, 1.0), string id = "") {
		this->text = string(text);
		this->graphics = graphics;
		this->id = id;
		init(textfont, pos, colour);
	};
	~Button(){};
	/*
		Copy constructor.
	*/
	Button& operator=(Button& other) {
		this->text = other.text;
		this->transform = other.transform;
		this->font = other.font;
		this->graphics = other.graphics;
		this->textbox = other.textbox;
		return *this;
	};

	/*
		Method to notify Button of Mouse Events it has subscribed to. 
		If the event was a click within the button's bounding box the  
		OnClick callback method will be called.
	*/
	void handle(MouseEvent event) {
		//if not a click event we don't care
		if (event.eventType != MouseEventType::LEFT_CLICK) {
			return;
		}

		if (event.actionType != MouseActionType::MOUSE_PRESS) {
			return;
		}
		//if this button was clicked
		int x = event.x, y = graphics->getHeight() - event.y;
		if ((x >= aabb.x) && (x <= aabb.x + aabb.width) && (y >= aabb.y) && (y <= aabb.y + aabb.height))
		{
			onClickCallback();
		}
	};
	/*
		Empty implementation of KeyEvent handeling.
	*/
	void handle(KeyEvent event) {
		//NOP
	}

	/*
		Sets the button's onClick callback function. 
		The callback function does not have to be void with no parameters as long as it is correctly bound
		using std::bind().
		std::function<void()> c, The callback function. 
	*/
	void addOnClickFn(std::function<void()> c) {
		onClickCallback = c;
	};

	/*
		Call to the graphics system to render this button.
	*/
	void render() {
		textbox->render();
	}

	std::string getId() {
		return id;
	}
};


#endif // !BUTTON_H

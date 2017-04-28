#pragma once
#ifndef BUTTON_H
#define BUTTON_H
#include <string>
using std::string;
using std::pair;
#include "EventListener.h"
#include "Font.h"
#include "Graphics\Shader.h"
#include <Graphics\Transform.h>
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

	std::function<void()> onClickCallback;
	bool active;
	
	string scriptName;
	string funcName;
	vector<pair<string,string>> params;

	
	
public:
	//Constructors
	Button() { active = false;  };
	/*
		Constructor
		string text, The text to display for the button.
		Font textfont, The font to use for the text.
		shared_ptr<Transform>& pos, The position, scale and orientation of the button.
		shared_ptr<Graphics>& graphics, Pointer to the graphics system.
		glm::vec3& colour, The colour of the text, normalised to 0..1. Defaulted to white (1.0,1.0,1.0), 
		string id, The button#s Id.
	*/
	Button( shared_ptr<Graphics>& passedGraphics, shared_ptr<Transform>& pos, Font font, string text) {
		init(passedGraphics, font, text, pos);
	};
	Button(shared_ptr<Graphics>& passedGraphics, shared_ptr<Transform>& pos) {
		init(passedGraphics, pos);
	};

	~Button(){};
	/*
		Copy constructor.
	*/
	Button& operator=(Button& other) {
	//	this->text = other.text;
		//this->transform = other.transform;
	//	this->font = other.font;
		this->graphics = other.graphics;
	//	this->textbox = other.textbox;
		return *this;
	};
	/*
	Inititalise the Button, creating a Textbox for rendering and building an AABB around the text.
	*/
	void init(shared_ptr<Graphics>& passedGraphics,Font font, string text, shared_ptr<Transform> transform) {
		this->graphics = passedGraphics;
		buildAABB(font, text, transform);
		active = true;
	};
	/*
	Inititalise the Button, creating a Textbox for rendering and building an AABB around the text.
	*/
	void init(shared_ptr<Graphics>& passedGraphics, shared_ptr<Transform> transform) {
		this->graphics = passedGraphics;
		buildAABB(transform);
		active = true;
	};
	/*
	/*
	Calculates a bounding box around the button text.
	*/
	void buildAABB(shared_ptr<Transform> transform) {
		float maxHeight = 0;
		//transform from center
		aabb.x = transform->position.x - (transform->scale.x / 2);
		aabb.y = transform->position.y - (transform->scale.y / 2);
		aabb.width = transform->scale.x;
		aabb.height = transform->scale.y;
	};
	void buildAABB(Font font, string text, shared_ptr<Transform> transform) {
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
	/*		Method to notify Button of Mouse Events it has subscribed to. 
		If the event was a click within the button's bounding box the  
		OnClick callback method will be called.
	*/
	bool handle(MouseEvent& event) {
		//if not a click event we don't care
		if (event.eventType == MouseEventType::MOUSE_MOVE) {
			return false;
		}

		if (event.actionType != MouseActionType::MOUSE_PRESS) {
			return false;
		}
		//if this button was clicked
		int x = event.x, y = graphics->getHeight() - event.y;
		if ((x >= aabb.x) && (x <= aabb.x + aabb.width) && (y >= aabb.y) && (y <= aabb.y + aabb.height))
		{
			if (onClickCallback) 
			{ 
				onClickCallback();  
				return true;
			}
		}
		return false;
	};
	/*
		Empty implementation of KeyEvent handeling.
	*/
	bool handle(KeyEvent& event) {
		//NOP
		return false;
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

	bool isActive() {
		return active;
	}

	void setActive(bool passedActive) {
		active = passedActive;	
	}
	
	void setScript(string setter) {
		scriptName = setter;
	}
	string getScript() {
		return scriptName;
	}
	void setFunc(string setter) {
		funcName = setter;
	}
	string getFunc() {
		return funcName;
	}
	void setParam(pair<string, string> setter) {
		params.push_back(setter);
	}
	vector<pair<string, string>> getParams() {
		return params;
	}

	void clearParams() {
		params.clear();
	}
};


#endif // !BUTTON_H

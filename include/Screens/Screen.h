#pragma once
#ifndef SCREEN_H
#define SCREEN
#include <string>
using std::string;
#include <memory>
using std::shared_ptr;


#include <GUI\Button.h>
#include <GUI\TextBox.h>
#include <GUI\UIElement.h>
#include <utils\XMLReader.h>

/*
	Abstract base class that represents a single arbitrary 'Screen' within the game.
	Implementations of this class provide the best place for game specific code.
*/
class Screen {
public:
	/*
		Pure Virtual method. Implementations can use this to update an time based operations.
		This is called at least once (If current active screen) every frame before rendering.
		double dt, The time passed since last update call.
	*/
	virtual void update(double dt) = 0;
	/*
		Pure Virtual method. Implementations should call any rendering code here.
		This is called once (If current active screen) every frame.
	*/
	virtual void render() = 0;
	/*
		Pure Virtual method. Resize the screen.
		int width, New screen width.
		int height, New screen height.
	*/
	virtual void resize(int width, int height) = 0;
	/*
		Pure Virtual method. Implementations should free any resources here.
	*/
	virtual void dispose() = 0;
	/*
		Sets the screen's Id.
		string id, The Id.
	*/
	void setID(string id) {
		screenId = id;
	};
	/*
		Returns the screen's Id.
	*/
	string getID() {
		return screenId;
	}

	void setXMLFilePath(string filePath)
	{
		xmlFilePath = filePath;
	}

	string getXMLFilePath()
	{
		return xmlFilePath;
	}
	void setXMLDocument(tinyxml2::XMLDocument* doc)
	{
		screenXmlDocument = doc;
	}
	void setXMLDocument(std::string fileName)
	{
		screenXmlDocument = XMLReader::GetDoc(fileName);
	}
	void setXMLDocument()
	{
		screenXmlDocument = XMLReader::GetDoc(xmlFilePath);
	}

	tinyxml2::XMLDocument* getXMLDocument()
	{
		return screenXmlDocument;
	}

	vector<shared_ptr<UIElement>> getUIElements()
	{
		return uiElements;
	}
	/*
	Clean up button resources.
	*/
	void disposeButtons()
	{
		for (auto button : buttons) {
			button.reset();
		}
	}
	/*
	Adds a new Button to the screen.
	shared_ptr<Button> button, Button to add.
	*/
	virtual void addButton(shared_ptr<Button> button)
	{
		buttons.push_back(button);
	}
	/*
	Adds a new TextBox to the screen.
	shared_ptr<TextBox> textbox, TextBox to add.
	*/
	virtual void addTextBox(shared_ptr<TextBox> textbox)
	{
		textBoxes.push_back(textbox);
	}

	virtual void addUIElement(shared_ptr<UIElement> uiElement)
	{
		uiElements.push_back(uiElement);
	}
protected:
	string screenId;
	string xmlFilePath;
	tinyxml2::XMLDocument* screenXmlDocument;
	vector<shared_ptr<Button>> buttons;
	vector<shared_ptr<TextBox>> textBoxes;
	vector<shared_ptr<UIElement>> uiElements;
};

#endif // !SCREEN_H


#pragma once
#ifndef FILE_SAVE_H
#define FILE_SAVE_H

#include "tinyxml2.h"
#include <GameObject.h>
#include <string>
/*
	Utility class to save files.

	Read File in Using TiXml2
	Get element that needs changing
	change element.
	doc.save
*/
class FileSaver
{
private:
	static bool UpdateTransform(tinyxml2::XMLDocument* doc, tinyxml2::XMLElement* ele, shared_ptr<Transform> transform);
public:
	/*
		Update existing game objects inside of the current screen doc
	*/
	static bool UpdateFile(tinyxml2::XMLDocument* doc, string levelID, int iObjectCount, shared_ptr<GameObject> go, shared_ptr<GameScreen> gameScreen);
	/*
	Update existing UI elements inside of the current screen doc
	*/
	static bool UpdateFile(tinyxml2::XMLDocument* doc, string levelID, int iObjectCount, shared_ptr<UIElement> uiE, shared_ptr<GameScreen> gameScreen);
	/*
		Add a new game object to the current screen doc
	*/
	static bool AddObjectToFile(tinyxml2::XMLDocument* doc, int iObjectCount, shared_ptr<GameObject> go, shared_ptr<GameScreen> gameScreen);
	/*
		Save the current screen doc to the xml file specified
	*/
	static bool SaveFile(tinyxml2::XMLDocument* doc, std::string fileName);
};
#endif // !FILE_SAVE_H

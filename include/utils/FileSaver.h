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
	//update existing transform comp of element in xmlfile
	static bool UpdateTransform(tinyxml2::XMLDocument* doc, tinyxml2::XMLElement* ele, Transform* transform);
	//update existing vec3
	static bool UpdateVec3Element(tinyxml2::XMLDocument* doc, tinyxml2::XMLElement* ele, glm::vec3 passedVector, string vecNames[3]);
	//add transform comp to element in file.
	static bool AddTransformToFile(tinyxml2::XMLDocument* doc, tinyxml2::XMLElement* ele, Transform* transform);
	//add vec3 
	static bool AddVec3ToElement(tinyxml2::XMLDocument* doc, tinyxml2::XMLElement* ele, glm::vec3 passedVector, string vecNames[3]);
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
		Add a new ui object to the current screen doc
	*/
	static bool AddObjectToFile(tinyxml2::XMLDocument* doc, int iObjectCount, shared_ptr<UIElement> uiE, shared_ptr<GameScreen> gameScreen);
	/*
		Save the current screen doc to the xml file specified
	*/
	static bool SaveFile(tinyxml2::XMLDocument* doc, std::string fileName);
};
#endif // !FILE_SAVE_H

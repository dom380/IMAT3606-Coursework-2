#pragma once
#ifndef FILE_SAVE_H
#define FILE_SAVE_H

#include "tinyxml2.h"
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

public:
	static bool UpdateFile(tinyxml2::XMLDocument* doc, int iCount, std::string objectName, std::string componentInnerElement, std::string elementToSave, std::string stringToSave);
	static bool SaveFile(tinyxml2::XMLDocument* doc, std::string fileName);
};
#endif // !FILE_SAVE_H

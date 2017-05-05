#pragma once
#ifndef __XMLREADER_H
#define __XMLREADER_H

#include "tinyxml2.h"
#include "string"
#include <GL\glm\glm\glm.hpp>

using namespace std;
//Allows data driven aspects
class XMLReader
{
public:
	static tinyxml2::XMLDocument* GetDoc(string i_sDocName);
	static int GetNumberOfGameObjectsInFile(tinyxml2::XMLDocument* doc);
	static int GetNumberOfUIElementsInFile(tinyxml2::XMLDocument* doc);
	static int GetNumberOfLightElementsInFile(tinyxml2::XMLDocument* doc);


	static int			iReadDataFromElements(string i_sDocName, int i_iInnerElementNumber,string i_sOuterElement);
	static float		fReadDataFromElements(string i_sDocName, int i_iInnerElementNumber, string i_sOuterElement);
	static string		sReadDataFromElements(string i_sDocName, int i_iInnerElementNumber, string i_sOuterElement);

	static glm::vec3	v3ReadDataFromElements(string i_sDocName, int i_iTotalInnerElementNumber, string i_sOuterElement);


private:

	static string ReadDataHelper(tinyxml2::XMLDocument* pDoc, int i_iInnerElementNumber, string i_sOuterElement);
	//static bool LoadDoc(string i_sDocName);
};
#endif

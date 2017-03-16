#include <utils\FileSaver.h>
#include <utils\XMLReader.h>

bool FileSaver::UpdateFile(tinyxml2::XMLDocument* doc, int iCount, std::string objectName, std::string componentInnerElementName, std::string elementToSave, std::string stringToSave)
{
	int ObjectCount = 0;
	if (doc)
	{
		tinyxml2::XMLElement* screenElement = doc->FirstChildElement("screen");
		const char* type = screenElement->Attribute("type");
		if (string(type) == string("level")) {
			tinyxml2::XMLElement* gameObjElement = screenElement->FirstChildElement("gameObjects")->FirstChildElement();
			while (gameObjElement != NULL) {
				tinyxml2::XMLElement* componentElement = gameObjElement->FirstChildElement("components")->FirstChildElement();
				while (componentElement != NULL)
				{

					const char* comptype = componentElement->Attribute("type");
					if (string(comptype) == "model")
					{
						tinyxml2::XMLElement* nameElement = componentElement->FirstChildElement("file");
						string NameElementText = nameElement->GetText();
						if (NameElementText == objectName)
						{
							if (ObjectCount != iCount)
							{
								break;
							}
						}
						else {
							break;
						}
					}
					else if (string(comptype) == "transform")
					{
						tinyxml2::XMLElement* componentInnerElement = componentElement->FirstChildElement(componentInnerElementName.c_str());
						if (componentInnerElement)
						{
							componentInnerElement->FirstChildElement(elementToSave.c_str())->SetText(stringToSave.c_str());
							//doc.SaveFile(fileName.c_str());
							return true;
						}
						else
						{
							//"component not found"
						}

					}
					componentElement = componentElement->NextSiblingElement();
				}
				ObjectCount++;
				gameObjElement = gameObjElement->NextSiblingElement();
			}

		}
		return true;
	}
	else
	{
		return false;
	}
}

bool FileSaver::SaveFile(tinyxml2::XMLDocument* doc, std::string fileName)
{
	tinyxml2::XMLError check = doc->SaveFile(fileName.c_str());
	if (check == tinyxml2::XML_SUCCESS)
	{
		return true;
	}
	else
	{
		return false;
	}
	
}

#include <utils\XMLReader.h>

tinyxml2::XMLDocument* XMLReader::GetDoc(string i_sDocName)
{
	tinyxml2::XMLDocument* docToRead = new tinyxml2::XMLDocument();
	tinyxml2::XMLError check = docToRead->LoadFile(i_sDocName.c_str());
	if (check != tinyxml2::XML_SUCCESS)
	{
		printf("ERROR - No Data Doc found");
		return nullptr;
	} 
	else
	{
		return docToRead;
	}
}
/*
bool XMLReader::LoadDoc(string i_sDocName)
{
	bool bLoadOkay = m_DataDoc.LoadFile(i_sDocName.c_str());
	if (!bLoadOkay)
	{
		printf("Could not load test file '%s'. Error='%s'. Exiting.\n", i_sDocName.c_str(), m_DataDoc.ErrorName());
	}
	else
	{
		//m_DataDoc.Print();
	}
	return bLoadOkay;
}*/

int XMLReader::iReadDataFromElements(string i_sDocName, int i_iInnerElementNumber, string i_sOuterElement)
{
	int iNu = atof(ReadDataHelper(GetDoc(i_sDocName), i_iInnerElementNumber, i_sOuterElement).c_str());
	return iNu;
}

float XMLReader::fReadDataFromElements(string i_sDocName, int i_iInnerElementNumber, string i_sOuterElement)
{
	float fNu = atof(ReadDataHelper(GetDoc(i_sDocName), i_iInnerElementNumber, i_sOuterElement).c_str());
	return fNu;
	
}

string XMLReader::sReadDataFromElements(string i_sDocName, int i_iInnerElementNumber, string i_sOuterElement)
{
	return ReadDataHelper(GetDoc(i_sDocName),  i_iInnerElementNumber, i_sOuterElement);
}

glm::vec3 XMLReader::v3ReadDataFromElements(string i_sDocName, int i_iTotalInnerElementNumber, string i_sOuterElement)
{
	tinyxml2::XMLDocument* DataDoc = GetDoc(i_sDocName);
	tinyxml2::XMLNode* pNode = nullptr;
	tinyxml2::XMLElement* pOuterElement = nullptr;
	tinyxml2::XMLElement* pInnerElement = nullptr;

	glm::vec3 oVector(1.0f);
	if (DataDoc->Error())
	{
		printf("ERROR - No Data Doc found");
		return oVector;
	}
	else
	{
		for (pNode = DataDoc->FirstChildElement(); pNode; pNode = pNode->NextSibling())
		{
			string sTemp = pNode->ToElement()->Value();
			if (sTemp == i_sOuterElement)
			{
				pOuterElement = pNode->ToElement();
				break;
			}
		}
		if (pOuterElement)
		{
			pNode = pOuterElement->FirstChildElement();
			if (pNode)
			{
				for (int x = 0; x < i_iTotalInnerElementNumber; x++)
				{
					
					pInnerElement = pNode->ToElement();
					if (pInnerElement)
					{
						printf("%s", pInnerElement->FirstChild()->Value());
						oVector[x] = atof(pInnerElement->FirstChild()->Value());
					}
					pNode = pNode->NextSiblingElement();
				}
			}
			else
			{
				printf("ERROR - inner element not found");
			}
		}
		else
		{
			printf("ERROR -%s not found", i_sOuterElement.c_str());
		}

	}

	return oVector;
}

string XMLReader::ReadDataHelper(tinyxml2::XMLDocument* pDoc, int i_iInnerElementNumber, string i_sOuterElement)
{
	tinyxml2::XMLDocument* DataDoc = pDoc;
	tinyxml2::XMLNode* pNode = nullptr;
	tinyxml2::XMLElement* pOuterElement = nullptr;
	tinyxml2::XMLElement* pInnerElement = nullptr;

	if (DataDoc->Error())
	{
		printf("ERROR - No Data Doc found");
		return "";
	}
	else
	{
		for (pNode = DataDoc->FirstChildElement(); pNode; pNode = pNode->NextSibling())
		{
			string sTemp = pNode->ToElement()->Value();
			if (sTemp == i_sOuterElement)
			{
				pOuterElement = pNode->ToElement();
				break;
			}
		}
		if (pOuterElement)
		{
			pNode = pOuterElement->FirstChildElement();
			if (pNode)
			{
				for (int x = 0; x < i_iInnerElementNumber; x++)
				{
					pNode = pNode->NextSiblingElement();
				}
				pInnerElement = pNode->ToElement();

			}
			else
			{
				printf("ERROR - inner element not found");
			}

		}
		else
		{
			printf("ERROR -%s not found", i_sOuterElement.c_str());
		}

		if (pInnerElement)
		{
			printf("%s", pInnerElement->FirstChild()->Value());
			return pInnerElement->FirstChild()->Value();
		}
	}

	return "";
}

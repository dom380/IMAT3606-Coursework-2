#include <utils\FileSaver.h>
#include <utils\XMLReader.h>
#include <utils\EnumParser.h>
#include <GUI\UITextureElement.h>

bool FileSaver::UpdateFile(tinyxml2::XMLDocument * doc, string levelID, int iObjectCount, shared_ptr<GameObject> go, shared_ptr<GameScreen> gameScreen)
{
	int XMLObjectCount = 0;
	bool SkipObject = false;
	if (doc)
	{
		tinyxml2::XMLElement* screenElement = doc->FirstChildElement("screen");
		int strSize = AssetManager::getInstance()->getRootFolder(AssetManager::ResourceType::LEVEL).size();
		int idsize = levelID.size();
		string levelIDStripped = levelID.substr(AssetManager::getInstance()->getRootFolder(AssetManager::ResourceType::LEVEL).size(), levelID.size());
		levelIDStripped = levelIDStripped.substr(0, levelIDStripped.size()-4);
		screenElement->SetAttribute("name", levelIDStripped.c_str());
		const char* type = screenElement->Attribute("type");
		if (string(type) == string("level")) {
			tinyxml2::XMLElement* gameObjElement = screenElement->FirstChildElement("gameObjects")->FirstChildElement();
			/*
				Iterate over all game objects in XML doc
			*/
			while (gameObjElement != NULL)
			{
				/*
					If XML object count != the object count
					and the object name is the same then skip.
					Used for objects with the same name.
				*/
				if (XMLObjectCount == iObjectCount)
				{
					tinyxml2::XMLElement* componentElement = gameObjElement->FirstChildElement("components")->FirstChildElement();
					/*
					Iterate over all components in object
					*/
					while (componentElement != NULL)
					{
						string XMLCompType = componentElement->Attribute("type");
						/*
						If XML object count != the object count
						and the object name is the same then skip.
						Used for objects with the same name.
						*/
						if (!SkipObject)
						{
							/*
								Iterate through GO components
							*/
							for (int i = ComponentType::MODEL; i < ComponentType::COMPONENT_TYPE_COUNT; i++)
							{
								ComponentType cType = static_cast<ComponentType>(i);
								
								if (go->HasComponent(cType))
								{
									//XML current GO and passedGO have same component
									if (cType == EnumParser<ComponentType>().parse(XMLCompType))
									{
										/*
											Switch between all components
										*/
										switch (cType)
										{
										case MODEL:
										{
											auto model = gameScreen->getComponentStore()->getComponent<ModelComponent>(go->GetComponentHandle(ComponentType::MODEL), ComponentType::MODEL);
											tinyxml2::XMLElement* objNameElement = componentElement->FirstChildElement("file");
											tinyxml2::XMLElement* textureElement = objNameElement->NextSiblingElement("texture");
											tinyxml2::XMLElement* renderActiveElement = objNameElement->NextSiblingElement("active");
											
											//check for objects with the same name (multiple wall.obj for example)
											if (XMLObjectCount != iObjectCount)
											{
												SkipObject = true;
												break;
											}
											else
											{
												/*
													Some elements may not exist yet, or require changing
												*/
												if (objNameElement)
													objNameElement->SetText(model->getObjFileName().c_str());
			
												componentElement->SetAttribute("id",model->getId().c_str());
							
												if (textureElement)
												{
													textureElement->SetText(model->getTextureName().c_str());
												}
												else if (strcmp(model->getTextureName().c_str(), "") != 0)
												{
													textureElement = doc->NewElement("texture");
													textureElement->SetText(model->getTextureName().c_str());
													componentElement->InsertEndChild(textureElement);
												}
												if (renderActiveElement)
												{
													renderActiveElement->SetText(model->isDrawing());
												}
												else
												{
													renderActiveElement = doc->NewElement("active");
													renderActiveElement->SetText(model->isDrawing());
													componentElement->InsertEndChild(renderActiveElement);
												}
												SkipObject = false;
												break;
											}
											
										
											break;
										}
										case ANIMATION:

											break;
										case RIGID_BODY:

											break;
										case LOGIC:
											//TODO
											break;
										case TRANSFORM:
											/*
											 Iterate through all inner transforms and their xyz and update doc
											*/
											auto transform = gameScreen->getComponentStore()->getComponent<Transform>(go->GetComponentHandle(ComponentType::TRANSFORM), ComponentType::TRANSFORM);
											if (!transform)
												return false;
											UpdateTransform(doc, componentElement, transform);
											break;
										}
									}
								}
							}
						}
						componentElement = componentElement->NextSiblingElement();
					}
				}
				XMLObjectCount++;
				if (XMLObjectCount > iObjectCount)
				{
					return true;
				}
				SkipObject = false;
				gameObjElement = gameObjElement->NextSiblingElement();
			}

		}
	}
	return false;
}

bool FileSaver::UpdateFile(tinyxml2::XMLDocument * doc, string levelID, int iObjectCount, shared_ptr<UIElement> uiElement, shared_ptr<GameScreen> gameScreen)
{
	int XMLObjectCount = 0;
	bool SkipObject = false;
	if (doc)
	{
		tinyxml2::XMLElement* screenElement = doc->FirstChildElement("screen");
		if (!screenElement)
			return false;
		tinyxml2::XMLElement* XMLUIElement = screenElement->FirstChildElement("uiElements");
		if (XMLUIElement)
		{
			XMLUIElement = XMLUIElement->FirstChildElement();
		}
		else
		{
			//No ui exists.
			return false;
		}
		/*
		Iterate over all ui in XML doc
		*/
		while (XMLUIElement != NULL)
		{
			//check for objects with the same name (multiple wall.obj for example)
			if (XMLObjectCount != iObjectCount)
			{
				SkipObject = true;
			}
			else
			{
				//ID
				tinyxml2::XMLElement* XMLUIIDElement = XMLUIElement->FirstChildElement("ID");
				if (XMLUIIDElement)
				{
					XMLUIIDElement->SetText(uiElement->getID().c_str());
				}
				//Type Specific Updates
				switch (uiElement->getType())
				{
					case UIType::TEXT:
					{
						shared_ptr<TextBox> tb = dynamic_pointer_cast<TextBox>(uiElement);
						//Update its text value
						tinyxml2::XMLElement* XMLUITextElement = XMLUIElement->FirstChildElement("value");
						if (XMLUITextElement)
						{
							XMLUITextElement->SetText(tb->getText().c_str());
						}
						//colour
						tinyxml2::XMLElement* XMLUIElementColour = XMLUIElement->FirstChildElement("colour");
						string colourName[3] = { "r", "g", "b" };
						if (!XMLUIElementColour)
						{
							XMLUIElementColour = doc->NewElement("colour");
						
							AddVec3ToElement(doc, XMLUIElementColour, tb->getColour(), colourName);
						}
						else
						{
							UpdateVec3Element(doc, XMLUIElementColour, tb->getColour(), colourName);
						}
						break;
					}
					case UIType::TEXTURE:
					{
						shared_ptr<UITextureElement> textureUI = dynamic_pointer_cast<UITextureElement>(uiElement);
						//update texture value
						tinyxml2::XMLElement* XMLUITextureElement = XMLUIElement->FirstChildElement("Texture");
						if (XMLUITextureElement)
						{
							XMLUITextureElement->SetText(textureUI->getTextureName().c_str());
						}
						break;
					}
				}

				//UpdateButton?
				if (uiElement->getButton())
				{
					tinyxml2::XMLElement* XMLUIButtonElement = XMLUIElement->FirstChildElement("onClick");
					if (uiElement->getButton()->isActive())
					{
						//UpdateButton
						if (XMLUIButtonElement)
						{
							tinyxml2::XMLElement* XMLUIButtonScriptElement = XMLUIButtonElement->FirstChildElement("script");
							XMLUIButtonScriptElement->SetText(uiElement->getButton()->getScript().c_str());
							tinyxml2::XMLElement* XMLUIButtonFunctionElement = XMLUIButtonElement->FirstChildElement("function");
							XMLUIButtonFunctionElement->SetText(uiElement->getButton()->getFunc().c_str());
							tinyxml2::XMLElement* XMLUIButtonParamsElement = doc->NewElement("params");
							//Do params exist in file?
							if (XMLUIButtonParamsElement)
							{
								tinyxml2::XMLElement* XMLUIButtonParamElement = XMLUIButtonParamsElement->FirstChildElement("param");
								for (int x = 0; x < uiElement->getButton()->getParams().size(); x++)
								{
									//Does the specific param exist
									if (XMLUIButtonParamElement)
									{
									}
									else
									{
										XMLUIButtonParamElement = doc->NewElement("param");
										XMLUIButtonParamsElement->InsertEndChild(XMLUIButtonParamElement);
									}
									XMLUIButtonParamElement->SetAttribute("name", uiElement->getButton()->getParams()[x].first.c_str());
									XMLUIButtonParamElement->SetText(uiElement->getButton()->getParams()[x].second.c_str());
									XMLUIButtonParamElement = XMLUIButtonParamElement->NextSiblingElement();
								}
							}
							else
							{
								//Create new params for file
								XMLUIButtonParamsElement = doc->NewElement("params");
								for (int x = 0; x < uiElement->getButton()->getParams().size(); x++)
								{
									tinyxml2::XMLElement* XMLUIButtonParamElement = doc->NewElement("param");
									XMLUIButtonParamElement->SetAttribute("name", uiElement->getButton()->getParams()[x].first.c_str());
									XMLUIButtonParamElement->SetText(uiElement->getButton()->getParams()[x].second.c_str());
									XMLUIButtonParamsElement->InsertEndChild(XMLUIButtonParamElement);
								}
								XMLUIButtonElement->InsertEndChild(XMLUIButtonParamsElement);
							}
						}
						else
						{
							//add new button
							XMLUIButtonElement = doc->NewElement("onClick");
							tinyxml2::XMLElement* XMLUIButtonScriptElement = doc->NewElement("script");
							XMLUIButtonScriptElement->SetText(uiElement->getButton()->getScript().c_str());
							XMLUIButtonElement->InsertEndChild(XMLUIButtonScriptElement);
							tinyxml2::XMLElement* XMLUIButtonFunctionElement = doc->NewElement("function");
							XMLUIButtonFunctionElement->SetText(uiElement->getButton()->getFunc().c_str());
							XMLUIButtonElement->InsertEndChild(XMLUIButtonFunctionElement);
							tinyxml2::XMLElement* XMLUIButtonParamsElement = doc->NewElement("params");
							for (int x = 0; x < uiElement->getButton()->getParams().size(); x++)
							{
								tinyxml2::XMLElement* XMLUIButtonParamElement = doc->NewElement("param");
								XMLUIButtonParamElement->SetAttribute("name", uiElement->getButton()->getParams()[x].first.c_str());
								XMLUIButtonParamElement->SetText(uiElement->getButton()->getParams()[x].second.c_str());
								XMLUIButtonParamsElement->InsertEndChild(XMLUIButtonParamElement);
							}
							XMLUIButtonElement->InsertEndChild(XMLUIButtonParamsElement);
							XMLUIElement->InsertEndChild(XMLUIButtonElement);
						}
					}
					else
					{
						//delete button
						if (XMLUIButtonElement)
						{
							XMLUIElement->DeleteChild(XMLUIButtonElement);
						}
					}
				}
				
				//Update its transform
				UpdateTransform(doc, XMLUIElement, uiElement->getTransform().get());
			}
			XMLObjectCount++;
			SkipObject = false;
			XMLUIElement = XMLUIElement->NextSiblingElement();
		}

		
	}
	return false;
}

bool FileSaver::AddObjectToFile(tinyxml2::XMLDocument* doc, int iObjectCount, shared_ptr<GameObject> go, shared_ptr<GameScreen> gameScreen)
{
	tinyxml2::XMLElement* screenElement = doc->FirstChildElement("screen");

	const char* type = screenElement->Attribute("type");
	if (string(type) == string("level")) {
		tinyxml2::XMLElement* gameObjsElement = screenElement->FirstChildElement("gameObjects");
		tinyxml2::XMLElement* gameObjectElement = doc->NewElement("gameobject");
		tinyxml2::XMLElement* componentsElement = doc->NewElement("components");
		
		//go through each component
		for (int i = ComponentType::MODEL; i < ComponentType::COMPONENT_TYPE_COUNT; i++)
		{
			ComponentType cType = static_cast<ComponentType>(i);
			//string ObjectCompType = ComponentNames[cType];
			if (go->HasComponent(cType))
			{
				/*
					Add new component as child of components
				*/
				tinyxml2::XMLElement* componentElement = doc->NewElement("component");
				componentElement->SetAttribute("type", EnumParser<ComponentType>().getString(cType).c_str());
				componentsElement->InsertEndChild(componentElement);
				switch (cType)
				{
					case MODEL:
					{
						/*
							Give component model information, optional texture
						*/
						auto model = gameScreen->getComponentStore()->getComponent<ModelComponent>(go->GetComponentHandle(ComponentType::MODEL), ComponentType::MODEL);
						tinyxml2::XMLElement* objNameElement = doc->NewElement("file");
						componentElement->InsertEndChild(objNameElement);
						objNameElement->SetText(model->getObjFileName().c_str());
						componentElement->SetAttribute("id", model->getId().c_str());
						//If there is a texture to add
						if (strcmp(model->getTextureName().c_str(), "") != 0)
						{
							tinyxml2::XMLElement* textureElement = doc->NewElement("texture");
							componentElement->InsertEndChild(textureElement);
							textureElement->SetText(model->getTextureName().c_str());
						}
						tinyxml2::XMLElement* renderActiveElement = doc->NewElement("active");
						componentElement->InsertEndChild(renderActiveElement);
						renderActiveElement->SetText(model->isDrawing());
						break;
					}
					case ANIMATION:

						break;
					case RIGID_BODY:

						break;
					case LOGIC:

						break;
					case TRANSFORM:
						/*
						Give component transform information adding xyz from transform elements
						*/
					
						auto model = gameScreen->getComponentStore()->getComponent<ModelComponent>(go->GetComponentHandle(ComponentType::MODEL), ComponentType::MODEL);
						AddTransformToFile(doc, componentElement, model->getTransform());
					
						break;
				}
			}
		}
		
	
		//Insert new object at end of gameobjects file.
		gameObjectElement->InsertEndChild(componentsElement);
		gameObjsElement->InsertEndChild(gameObjectElement);
		


	}
	
	return true;
}
bool FileSaver::AddObjectToFile(tinyxml2::XMLDocument* doc, int iObjectCount, shared_ptr<UIElement> uiE, shared_ptr<GameScreen> gameScreen)
{
	if (doc)
	{
		tinyxml2::XMLElement* screenElement = doc->FirstChildElement("screen");

		tinyxml2::XMLElement* XMLUIElements = screenElement->FirstChildElement("uiElements");
		tinyxml2::XMLElement* XMLUIElement = doc->NewElement("uiElement");
		//ID
		tinyxml2::XMLElement* XMLUIElementID = doc->NewElement("ID");
		XMLUIElementID->SetText(uiE->getID().c_str());
		XMLUIElement->InsertEndChild(XMLUIElementID);
		//Type
		tinyxml2::XMLElement* XMLUIElementType = doc->NewElement("Type");
		UIType type = uiE->getType();
		XMLUIElementType->SetText(EnumParser<UIType>().getString(type).c_str());
		XMLUIElement->InsertEndChild(XMLUIElementType);

		switch (uiE->getType())
		{
		case UIType::TEXT:
		{
			shared_ptr<TextBox> tb = dynamic_pointer_cast<TextBox>(uiE);
			//text value
			tinyxml2::XMLElement* XMLUIElementValue = doc->NewElement("value");

			XMLUIElementValue->SetText(tb->getText().c_str());
			XMLUIElement->InsertEndChild(XMLUIElementValue);

			//colour
			tinyxml2::XMLElement* XMLUIElementColour = doc->NewElement("colour");
			XMLUIElement->InsertEndChild(XMLUIElementColour);
			string colourNames[3] = { "r", "g", "b" };
			AddVec3ToElement(doc, XMLUIElementColour, tb->getColour(), colourNames);
			break;
		}
		case UIType::TEXTURE:
		{
			tinyxml2::XMLElement* XMLUIElementID = doc->NewElement("Texture");

			XMLUIElementID->SetText(dynamic_pointer_cast<UITextureElement>(uiE)->getTextureName().c_str());
			XMLUIElement->InsertEndChild(XMLUIElementID);
			break;
		}
		default:
			break;
		}

		//HasButton?
		if (uiE->getButton())
		{
			tinyxml2::XMLElement* XMLUIButtonElement = XMLUIElement->FirstChildElement("onClick");
			if (uiE->getButton()->isActive())
			{
				//add new button
				tinyxml2::XMLElement* XMLUIButtonElement = doc->NewElement("onClick");
				tinyxml2::XMLElement* XMLUIButtonScriptElement = doc->NewElement("script");
				XMLUIButtonScriptElement->SetText(uiE->getButton()->getScript().c_str());
				XMLUIButtonElement->InsertEndChild(XMLUIButtonScriptElement);
				tinyxml2::XMLElement* XMLUIButtonFunctionElement = doc->NewElement("function");
				XMLUIButtonFunctionElement->SetText(uiE->getButton()->getFunc().c_str());
				XMLUIButtonElement->InsertEndChild(XMLUIButtonFunctionElement);
				tinyxml2::XMLElement* XMLUIButtonParamsElement = doc->NewElement("params");
				for (int x = 0; x < uiE->getButton()->getParams().size(); x++)
				{
					tinyxml2::XMLElement* XMLUIButtonParamElement = doc->NewElement("param");
					XMLUIButtonParamElement->SetAttribute("name", uiE->getButton()->getParams()[x].first.c_str());
					XMLUIButtonParamElement->SetText(uiE->getButton()->getParams()[x].second.c_str());
					XMLUIButtonParamsElement->InsertEndChild(XMLUIButtonParamElement);
				}
				if (uiE->getButton()->getParams().size() > 0)
				{
					XMLUIButtonElement->InsertEndChild(XMLUIButtonParamsElement);
				}
				XMLUIElement->InsertEndChild(XMLUIButtonElement);
			}

		}
		
		//UI does not use a quat, but an angle in the z axis.
		AddTransformToFile(doc, XMLUIElement, uiE->getTransform().get());

		if (!XMLUIElements)
		{
			XMLUIElements = doc->NewElement("uiElements");
			screenElement->InsertEndChild(XMLUIElements);
		}
		XMLUIElements->InsertEndChild(XMLUIElement);
		
	}
	return true;
}

bool FileSaver::DeleteObjectFromFile(tinyxml2::XMLDocument * doc, int iObjectCount, shared_ptr<GameObject> go, shared_ptr<GameScreen> gameScreen)
{

	tinyxml2::XMLElement* screenElement = doc->FirstChildElement("screen");
	bool SkipObject = false;
	int XMLObjectCount = 0;
	const char* type = screenElement->Attribute("type");
	if (string(type) == string("level")) {
		tinyxml2::XMLElement* gameObjsElement = screenElement->FirstChildElement("gameObjects");
		if (!gameObjsElement)
		{
			return false;
		}
		tinyxml2::XMLElement* gameObjectElement = gameObjsElement->FirstChildElement();
		//iterate through gos until object count = xml count
		while (gameObjectElement != NULL)
		{
			/*
			If XML object count != the object count
			and the object name is the same then skip.
			Used for objects with the same name.
			*/

			if (go->HasComponent(ComponentType::MODEL))
			{
				//check for objects with the same name (multiple wall.obj for example)
				if (XMLObjectCount != iObjectCount)
				{
				}
				else
				{
					//delete object from file.
					gameObjsElement->DeleteChild(gameObjectElement);
					return true;
				}
			}

			XMLObjectCount++;
			gameObjectElement = gameObjectElement->NextSiblingElement();
		}
	}
	//could not delete obj, not found
	return false;
}

bool FileSaver::DeleteObjectFromFile(tinyxml2::XMLDocument * doc, int iObjectCount, shared_ptr<UIElement> uiE, shared_ptr<GameScreen> gameScreen)
{

	tinyxml2::XMLElement* screenElement = doc->FirstChildElement("screen");
	bool SkipObject = false;
	int XMLObjectCount = 0;

	tinyxml2::XMLElement* uiElements = screenElement->FirstChildElement("uiElements");
	if (!uiElements)
	{
		return false;
	}
	tinyxml2::XMLElement* uiElement = uiElements->FirstChildElement();
	//iterate through gos until object count = xml count
	while (uiElement != NULL)
	{
		/*
		If XML object count != the object count
		and the object name is the same then skip.
		Used for objects with the same name.
		*/


		//check for objects with the same name (multiple wall.obj for example)
		if (XMLObjectCount != iObjectCount)
		{
		}
		else
		{
			//delete object from file.
			uiElements->DeleteChild(uiElement);
			return true;
		}
		

		XMLObjectCount++;
		uiElement = uiElement->NextSiblingElement();
	}
	
	//could not delete obj, not found
	return false;
}

bool FileSaver::UpdateTransform(tinyxml2::XMLDocument* doc, tinyxml2::XMLElement* transformElement, Transform* transform)
{
	for (int transformElementIt = 0; transformElementIt < 3; transformElementIt++)
	{
		switch (transformElementIt)
		{
		case 0:
		{
			tinyxml2::XMLElement* transformInnerElement = transformElement->FirstChildElement("position");
			string xyzName[3] = { "x", "y", "z" };
			if (!transformInnerElement)
			{
				AddVec3ToElement(doc, transformInnerElement, transform->position, xyzName);
			}
			else
			{
				UpdateVec3Element(doc, transformInnerElement, transform->position, xyzName);
			}
			
			
			break;
		}
		case 1:
		{
			tinyxml2::XMLElement* transformInnerElement = transformElement->FirstChildElement("scale");
			string xyzName[3] = { "x", "y", "z" };
			if (!transformInnerElement)
			{
				AddVec3ToElement(doc, transformInnerElement, transform->scale, xyzName);
			}
			else
			{
				UpdateVec3Element(doc, transformInnerElement, transform->scale, xyzName);
			}
			break;
		}
		case 2:
		{
			tinyxml2::XMLElement* transformInnerElement = transformElement->FirstChildElement("orientation");
			/*
			When we load an orientation in, we convert it to a quarternion
			When saving, we must undo this process.
			*/
			glm::vec3 axis = glm::vec3(transform->orientation[0], transform->orientation[1], transform->orientation[2]);
			float transformAngle = transform->orientation[3];
			axis = glm::axis(transform->orientation);
			transformAngle = glm::degrees(glm::angle(transform->orientation));
			
			//If there is a rotation applied.
			if ((int)transformAngle == 0 || (int)transform->orientation[3] == 1)
			{
				break;
			}
			else
			{
				if (!transformInnerElement)
				{
					tinyxml2::XMLElement* xyzElement;
					//Create a orientation element.
					transformInnerElement = doc->NewElement("orientation");
					xyzElement = doc->NewElement("x");
					transformInnerElement->InsertEndChild(xyzElement);
					xyzElement = doc->NewElement("y");
					transformInnerElement->InsertEndChild(xyzElement);
					xyzElement = doc->NewElement("z");
					transformInnerElement->InsertEndChild(xyzElement);
					xyzElement = doc->NewElement("w");
					transformInnerElement->InsertEndChild(xyzElement);
					transformElement->InsertEndChild(transformInnerElement);
				}
			}
		
			for (int quat = 0; quat < 4; quat++)
			{
				//xyz
				switch (quat)
				{
				case 0:
				{
					std::ostringstream ss;
					ss << axis[quat];
					transformInnerElement->FirstChildElement("x")->SetText(string(ss.str()).c_str());
					break;
				}
				case 1:
				{
					std::ostringstream ss;
					ss << axis[quat];
					transformInnerElement->FirstChildElement("y")->SetText(string(ss.str()).c_str());
					break;
				}

				case 2:
				{
					std::ostringstream ss;
					ss << axis[quat];
					transformInnerElement->FirstChildElement("z")->SetText(string(ss.str()).c_str());
					break;
				}

				case 3:
				{
					std::ostringstream ss;
					ss << transformAngle;
					transformInnerElement->FirstChildElement("w")->SetText(string(ss.str()).c_str());
					break;
				}
				}
			}
			break;
		}
		default:
			break;
		}
	}
	return true;
}

bool FileSaver::UpdateVec3Element(tinyxml2::XMLDocument * doc, tinyxml2::XMLElement * ele, glm::vec3 passedVector, string vecNames[3])
{
	for (int vector3 = 0; vector3 < 3; vector3++)
	{
		//xyz
		switch (vector3)
		{
			case 0:
			{
				std::ostringstream ss;
				ss << passedVector[vector3];
				ele->FirstChildElement(vecNames[0].c_str())->SetText(string(ss.str()).c_str());

				break;
			}
			case 1:
			{
				std::ostringstream ss;
				ss << passedVector[vector3];
				ele->FirstChildElement(vecNames[1].c_str())->SetText(string(ss.str()).c_str());
				break;
			}

			case 2:
			{
				std::ostringstream ss;
				ss << passedVector[vector3];
				if (ele->FirstChildElement(vecNames[2].c_str()))
				{
					ele->FirstChildElement(vecNames[2].c_str())->SetText(string(ss.str()).c_str());
				}
				else
				{
					//vec[2] does not exist yet
					tinyxml2::XMLElement* vec2Element = doc->NewElement(vecNames[2].c_str());
					vec2Element->SetText(string(ss.str()).c_str());
					ele->InsertEndChild(vec2Element);
				}
			
				break;
			}
		}

	}
	return true;
}

bool FileSaver::AddTransformToFile(tinyxml2::XMLDocument * doc, tinyxml2::XMLElement * XMLTransElement, Transform* transform)
{
	//Transform
	for (int transformElement = 0; transformElement < 3; transformElement++)
	{
		switch (transformElement)
		{
		case 0:
		{
			tinyxml2::XMLElement* innerElement = doc->NewElement("position");
			XMLTransElement->InsertEndChild(innerElement);
			if (!innerElement)
				break;
			string xyzName[3] = { "x", "y", "z" };
			AddVec3ToElement(doc, innerElement, transform->position, xyzName);
			
			break;
		}
		case 1:
		{
			tinyxml2::XMLElement* innerElement = doc->NewElement("scale");
			XMLTransElement->InsertEndChild(innerElement);
			if (!innerElement)
				break;
			string xyzName[3] = { "x", "y", "z" };
			AddVec3ToElement(doc, innerElement, transform->scale, xyzName);
			break;
		}
		case 2:
		{
			//check orientation is required.
			glm::vec3 axis = glm::axis(transform->orientation);
			float transformAngle = glm::degrees(glm::angle(transform->orientation));
			//If there is a rotation applied.
			if ((int)transformAngle == 0)
			{
				break;
			}
			
			
			tinyxml2::XMLElement* innerElement = doc->NewElement("orientation");
			XMLTransElement->InsertEndChild(innerElement);
			if (!innerElement)
				break;
			for (int vector4 = 0; vector4 < 4; vector4++)
			{
				//xyz
				switch (vector4)
				{
					case 0:
					{
						std::ostringstream ss;
						ss << axis[vector4];
						tinyxml2::XMLElement* vecElement = doc->NewElement("x");
						innerElement->InsertEndChild(vecElement);
						vecElement->SetText(string(ss.str()).c_str());
						break;
					}
					case 1:
					{
						std::ostringstream ss;
						ss << axis[vector4];
						tinyxml2::XMLElement* vecElement = doc->NewElement("y");
						innerElement->InsertEndChild(vecElement);
						vecElement->SetText(string(ss.str()).c_str());
						break;
					}

					case 2:
					{
						std::ostringstream ss;
						ss << axis[vector4];
						tinyxml2::XMLElement* vecElement = doc->NewElement("z");
						innerElement->InsertEndChild(vecElement);
						vecElement->SetText(string(ss.str()).c_str());
						break;
					}
					
					case 3:
					{
						std::ostringstream ss;
						ss << transformAngle;
						tinyxml2::XMLElement* vecElement = doc->NewElement("w");
						innerElement->InsertEndChild(vecElement);
						vecElement->SetText(string(ss.str()).c_str());
						break;
					}
				}

			}
			break;
		}

		}
	}
	return true;
}

bool FileSaver::AddVec3ToElement(tinyxml2::XMLDocument* doc, tinyxml2::XMLElement * ele, glm::vec3 passedVector, string vecNames[3])
{
	for (int vector3 = 0; vector3 < 3; vector3++)
	{
		//xyz
		switch (vector3)
		{
		case 0:
		{
			std::ostringstream ss;
			ss << passedVector[vector3];
			tinyxml2::XMLElement* vecElement = doc->NewElement(vecNames[0].c_str());
			ele->InsertEndChild(vecElement);
			vecElement->SetText(string(ss.str()).c_str());
			break;
		}
		case 1:
		{
			std::ostringstream ss;
			ss << passedVector[vector3];
			tinyxml2::XMLElement* vecElement = doc->NewElement(vecNames[1].c_str());
			ele->InsertEndChild(vecElement);
			vecElement->SetText(string(ss.str()).c_str());
			break;
		}

		case 2:
		{
			std::ostringstream ss;
			ss << passedVector[vector3];
			tinyxml2::XMLElement* vecElement = doc->NewElement(vecNames[2].c_str());
			ele->InsertEndChild(vecElement);
			vecElement->SetText(string(ss.str()).c_str());
			break;
		}
		}

	}
	return false;
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

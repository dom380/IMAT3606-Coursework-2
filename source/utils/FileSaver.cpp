#include <utils\FileSaver.h>
#include <utils\XMLReader.h>
#include <utils\EnumParser.h>
#include <GUI\UITextureElement.h>
#include <utils/Utilities.h>

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
					tinyxml2::XMLElement* componentsElement = gameObjElement->FirstChildElement("components");
					tinyxml2::XMLElement* componentElement = componentsElement->FirstChildElement();
					/*
					Iterate over all components in object
					*/
					//while (componentElement != NULL)
					{
						//string XMLCompType = componentElement->Attribute("type");
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
									bool xmlHasComp = false;
									while (componentElement != NULL)
									{
										string XMLCompType = componentElement->Attribute("type");
										if (cType == EnumParser<ComponentType>().parse(XMLCompType))
										{
											xmlHasComp = true;
											break;
										}
										componentElement = componentElement->NextSiblingElement();
									}
									
									if (!xmlHasComp)
									{
									    componentElement = doc->NewElement("component");
										componentElement->SetAttribute("type", EnumParser<ComponentType>().getString(cType).c_str());
										componentsElement->InsertEndChild(componentElement);
									}
									
									//XML current GO and passedGO have same component
									//if (cType == EnumParser<ComponentType>().parse(XMLCompType))
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

										case TRANSFORM:
										{
											/*
											Iterate through all inner transforms and their xyz and update doc
											*/
											auto transform = gameScreen->getComponentStore()->getComponent<Transform>(go->GetComponentHandle(ComponentType::TRANSFORM), ComponentType::TRANSFORM);
											if (!transform)
												return false;
											UpdateTransform(doc, componentElement, transform);
											break;
										}
										case ANIMATION:

											break;
										case RIGID_BODY:
										{
											auto physComp = gameScreen->getComponentStore()->getComponent<PhysicsComponent>(go->GetComponentHandle(ComponentType::RIGID_BODY), ComponentType::RIGID_BODY);
											if (!physComp)
												return false;
											UpdateRigidBody(doc, componentElement, physComp);
											break;
										}
										case LOGIC:
										{
											auto logic = gameScreen->getComponentStore()->getComponent<LogicComponent>(go->GetComponentHandle(ComponentType::LOGIC), ComponentType::LOGIC);
											if (!logic)
												return false;
											UpdateLogic(doc, componentElement, logic);
											break;
										}

										case TRIGGER:
										{
											auto trigger = gameScreen->getComponentStore()->getComponent<CollisionTrigger>(go->GetComponentHandle(ComponentType::TRIGGER), ComponentType::TRIGGER);
											if (!trigger)
												return false;
											UpdateTrigger(doc, componentElement, trigger);
											break;
										}
										}
									}
								}
								componentElement = componentsElement->FirstChildElement();
							}
							
						}
						//componentElement = componentElement->NextSiblingElement();
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

bool FileSaver::UpdateFile(tinyxml2::XMLDocument * doc, string levelID, int iObjectCount, shared_ptr<UIElement> uiElement)
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

						//font
						tinyxml2::XMLElement* XMLUIFontElement = XMLUIElement->FirstChildElement("font");
						if (XMLUIFontElement)
						{
						}
						else
						{
							XMLUIFontElement = doc->NewElement("font");
							XMLUIElement->InsertEndChild(XMLUIFontElement);
						}
						auto splitPath = Utilities::splitFilePath(tb->getFont().getFontPath());
						string fontName = splitPath.at(splitPath.size() - 1);
						XMLUIFontElement->SetText(fontName.c_str());
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

					case TRANSFORM:
					{
						/*
						Give component transform information adding xyz from transform elements
						*/

						auto model = gameScreen->getComponentStore()->getComponent<ModelComponent>(go->GetComponentHandle(ComponentType::MODEL), ComponentType::MODEL);
						AddTransformToFile(doc, componentElement, model->getTransform());

						break;
					}
					case ANIMATION:
					{
						break;
					}
					case RIGID_BODY:
					{
						auto physComp = gameScreen->getComponentStore()->getComponent<PhysicsComponent>(go->GetComponentHandle(ComponentType::RIGID_BODY), ComponentType::RIGID_BODY);
						if (!physComp)
							return false;
						AddRigidBody(doc, componentElement, physComp);
						break;
					}
					case LOGIC:
					{
						auto logic = gameScreen->getComponentStore()->getComponent<LogicComponent>(go->GetComponentHandle(ComponentType::LOGIC), ComponentType::LOGIC);
						if (!logic)
							return false;
						AddLogicToFile(doc, componentElement, logic);
						break;
					}
					case TRIGGER:
					{
						auto trigger = gameScreen->getComponentStore()->getComponent<CollisionTrigger>(go->GetComponentHandle(ComponentType::TRIGGER), ComponentType::TRIGGER);
						if (!trigger)
							return false;
						AddTrigger(doc, componentElement, trigger);
						break;
					}
				}
			}
		}
	
		//Insert new object at end of gameobjects file.
		gameObjectElement->InsertEndChild(componentsElement);
		gameObjsElement->InsertEndChild(gameObjectElement);
	}
	
	return true;
}
bool FileSaver::AddObjectToFile(tinyxml2::XMLDocument* doc, int iObjectCount, shared_ptr<UIElement> uiE)
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

bool FileSaver::DeleteObjectFromFile(tinyxml2::XMLDocument * doc, int iObjectCount, shared_ptr<UIElement> uiE)
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

bool FileSaver::UpdateLogic(tinyxml2::XMLDocument * doc, tinyxml2::XMLElement * logicElement, LogicComponent * logic)
{
	tinyxml2::XMLElement* scriptElement = logicElement->FirstChildElement("script");
	if (scriptElement)
	{

	}
	else
	{
		scriptElement = doc->NewElement("script");
		logicElement->InsertEndChild(scriptElement);
	}
	string fullScriptName = logic->getScriptName() + ".lua";
	scriptElement->SetText(fullScriptName.c_str());
	return true;
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
				transformInnerElement = doc->NewElement("position");
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
				transformInnerElement = doc->NewElement("scale");
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

bool FileSaver::UpdateRigidBody(tinyxml2::XMLDocument * doc, tinyxml2::XMLElement * physicsElement, PhysicsComponent * phyComp)
{
	if (physicsElement->FirstChildElement("mass") == NULL)
	{
		physicsElement->InsertEndChild(doc->NewElement("mass"));
	}
	physicsElement->FirstChildElement("mass")->SetText(*phyComp->getMass());

	
	if (*phyComp->getMeshFileName() != "")
	{
		if (physicsElement->FirstChildElement("collision_mesh") == NULL)
		{
			physicsElement->InsertEndChild(doc->NewElement("collision_mesh"));
		}
		physicsElement->FirstChildElement("collision_mesh")->SetText(phyComp->getMeshFileName()->c_str());
	}
	else
	{
		//bounding shape
		tinyxml2::XMLElement* shapeElement = physicsElement->FirstChildElement("bounding_shape");
		if (shapeElement)
		{
			ShapeData::BoundingShape shapeType = phyComp->getShape()->boundingShape;
			string typeName = EnumParser<ShapeData::BoundingShape>().getString(shapeType);
			shapeElement->SetAttribute("type", typeName.c_str());
			tinyxml2::XMLElement* extentsElemenet;
			switch (shapeType)
			{
			case ShapeData::BOX:
				extentsElemenet = shapeElement->FirstChildElement("half_extents");
				if (!extentsElemenet)
				{
					extentsElemenet = doc->NewElement("half_extents");
					shapeElement->InsertEndChild(extentsElemenet);
				}
				if (extentsElemenet->FirstChildElement("x") == NULL)
				{
					extentsElemenet->InsertEndChild(doc->NewElement("x"));
				}
				extentsElemenet->FirstChildElement("x")->SetText(phyComp->getShape()->halfExtents[0]);
				if (extentsElemenet->FirstChildElement("y") == NULL)
				{
					extentsElemenet->InsertEndChild(doc->NewElement("y"));
				}
				extentsElemenet->FirstChildElement("y")->SetText(phyComp->getShape()->halfExtents[1]);
				if (extentsElemenet->FirstChildElement("z") == NULL)
				{
					extentsElemenet->InsertEndChild(doc->NewElement("z"));
				}
				extentsElemenet->FirstChildElement("z")->SetText(phyComp->getShape()->halfExtents[2]);
				break;
			case ShapeData::SPHERE:
				if (shapeElement->FirstChildElement("radius") == NULL)
				{
					shapeElement->InsertEndChild(doc->NewElement("radius"));
				}
				shapeElement->FirstChildElement("radius")->SetText(phyComp->getShape()->radius);
				break;
			case ShapeData::CONE:
				if (shapeElement->FirstChildElement("radius") == NULL)
				{
					shapeElement->InsertEndChild(doc->NewElement("radius"));
				}
				shapeElement->FirstChildElement("radius")->SetText(phyComp->getShape()->radius);
				if (shapeElement->FirstChildElement("height") == NULL)
				{
					shapeElement->InsertEndChild(doc->NewElement("height"));
				}
				shapeElement->FirstChildElement("height")->SetText(phyComp->getShape()->height);
				break;
			case ShapeData::CYLINDER:
				extentsElemenet = shapeElement->FirstChildElement("half_extents");
				if (!extentsElemenet)
				{
					extentsElemenet = doc->NewElement("half_extents");
					shapeElement->InsertEndChild(extentsElemenet);
				}
				if (extentsElemenet->FirstChildElement("x") == NULL)
				{
					extentsElemenet->InsertEndChild(doc->NewElement("x"));
				}
				extentsElemenet->FirstChildElement("x")->SetText(phyComp->getShape()->halfExtents[0]);
				if (extentsElemenet->FirstChildElement("y") == NULL)
				{
					extentsElemenet->InsertEndChild(doc->NewElement("y"));
				}
				extentsElemenet->FirstChildElement("y")->SetText(phyComp->getShape()->halfExtents[1]);
				if (extentsElemenet->FirstChildElement("z") == NULL)
				{
					extentsElemenet->InsertEndChild(doc->NewElement("z"));
				}
				extentsElemenet->FirstChildElement("z")->SetText(phyComp->getShape()->halfExtents[2]);
				break;
			case ShapeData::CAPSULE:
				if (shapeElement->FirstChildElement("radius") == NULL)
				{
					shapeElement->InsertEndChild(doc->NewElement("radius"));
				}
				shapeElement->FirstChildElement("radius")->SetText(phyComp->getShape()->radius);
				if (shapeElement->FirstChildElement("height") == NULL)
				{
					shapeElement->InsertEndChild(doc->NewElement("height"));
				}
				shapeElement->FirstChildElement("height")->SetText(phyComp->getShape()->height);
				break;
			default:
				break;
			}
		}
	}
	
	
	if (physicsElement->FirstChildElement("convex") == NULL)
	{
		physicsElement->InsertEndChild(doc->NewElement("convex"));
	}
	physicsElement->FirstChildElement("convex")->SetText(*phyComp->isConvex());

	if (physicsElement->FirstChildElement("restitution") == NULL)
	{
		physicsElement->InsertEndChild(doc->NewElement("restitution"));
	}
	physicsElement->FirstChildElement("restitution")->SetText(phyComp->getRestitution());

	if (physicsElement->FirstChildElement("friction") == NULL)
	{
		physicsElement->InsertEndChild(doc->NewElement("friction"));
	}
	physicsElement->FirstChildElement("friction")->SetText(phyComp->getFriction());

	if (physicsElement->FirstChildElement("rotational_friction") == NULL)
	{
		physicsElement->InsertEndChild(doc->NewElement("rotational_friction"));
	}
	physicsElement->FirstChildElement("rotational_friction")->SetText(*phyComp->getRotationalFriction());

	if (physicsElement->FirstChildElement("constant_velocity") == NULL)
	{
		physicsElement->InsertEndChild(doc->NewElement("constant_velocity"));
	}
	physicsElement->FirstChildElement("constant_velocity")->SetText(*phyComp->isConstVelocity());

	if (physicsElement->FirstChildElement("velocity") == NULL)
	{
		physicsElement->InsertEndChild(doc->NewElement("velocity"));
	}
	tinyxml2::XMLElement* velElement = physicsElement->FirstChildElement("velocity");

	if (velElement->FirstChildElement("x") == NULL)
	{
		velElement->InsertEndChild(doc->NewElement("x"));
	}
	velElement->FirstChildElement("x")->SetText(*phyComp->getVelocity()[0]);
	if (velElement->FirstChildElement("y") == NULL)
	{
		velElement->InsertEndChild(doc->NewElement("y"));
	}
	velElement->FirstChildElement("y")->SetText(*phyComp->getVelocity()[1]);
	if (velElement->FirstChildElement("z") == NULL)
	{
		velElement->InsertEndChild(doc->NewElement("z"));
	}
	velElement->FirstChildElement("z")->SetText(*phyComp->getVelocity()[2]);
	
	
	return true;
}

bool FileSaver::UpdateTrigger(tinyxml2::XMLDocument * doc, tinyxml2::XMLElement * triggerElement, CollisionTrigger * triggerComp)
{
	//script
	tinyxml2::XMLElement* scriptElement = triggerElement->FirstChildElement("script");
	if (scriptElement)
	{
	}
	else
	{
		scriptElement = doc->NewElement("script");
		triggerElement->InsertEndChild(scriptElement);
	}
	string fullScriptName = triggerComp->getScriptName() + ".lua";
	scriptElement->SetText(fullScriptName.c_str());
	//triggeronce
	tinyxml2::XMLElement* triggerOnceElement = triggerElement->FirstChildElement("trigger_once");
	if (triggerOnceElement)
	{
	}
	else
	{
		triggerOnceElement = doc->NewElement("trigger_once");
		triggerElement->InsertEndChild(triggerOnceElement);
	}
	triggerOnceElement->SetText(triggerComp->isTriggerOnce());
	//bounding shape
	tinyxml2::XMLElement* shapeElement = triggerElement->FirstChildElement("bounding_shape");
	if (shapeElement)
	{
		ShapeData::BoundingShape shapeType = triggerComp->getShape()->boundingShape;
		string typeName = EnumParser<ShapeData::BoundingShape>().getString(shapeType);
		shapeElement->SetAttribute("type", typeName.c_str());
		tinyxml2::XMLElement* extentsElemenet;
		switch (shapeType)
		{
		case ShapeData::BOX:
			extentsElemenet = shapeElement->FirstChildElement("half_extents");
			if (!extentsElemenet)
			{
				extentsElemenet = doc->NewElement("half_extents");
				shapeElement->InsertEndChild(extentsElemenet);
			}
			if (extentsElemenet->FirstChildElement("x") == NULL)
			{
				extentsElemenet->InsertEndChild(doc->NewElement("x"));
			}
			extentsElemenet->FirstChildElement("x")->SetText(triggerComp->getShape()->halfExtents[0]);
			if (extentsElemenet->FirstChildElement("y") == NULL)
			{
				extentsElemenet->InsertEndChild(doc->NewElement("y"));
			}
			extentsElemenet->FirstChildElement("y")->SetText(triggerComp->getShape()->halfExtents[1]);
			if (extentsElemenet->FirstChildElement("z") == NULL)
			{
				extentsElemenet->InsertEndChild(doc->NewElement("z"));
			}
			extentsElemenet->FirstChildElement("z")->SetText(triggerComp->getShape()->halfExtents[2]);
			break;
		case ShapeData::SPHERE:
			if (shapeElement->FirstChildElement("radius") == NULL)
			{
				shapeElement->InsertEndChild(doc->NewElement("radius"));
			}
			shapeElement->FirstChildElement("radius")->SetText(triggerComp->getShape()->radius);
			break;
		case ShapeData::CONE:
			if (shapeElement->FirstChildElement("radius") == NULL)
			{
				shapeElement->InsertEndChild(doc->NewElement("radius"));
			}
			shapeElement->FirstChildElement("radius")->SetText(triggerComp->getShape()->radius);
			if (shapeElement->FirstChildElement("height") == NULL)
			{
				shapeElement->InsertEndChild(doc->NewElement("height"));
			}
			shapeElement->FirstChildElement("height")->SetText(triggerComp->getShape()->height);
			break;
		case ShapeData::CYLINDER:
			extentsElemenet = shapeElement->FirstChildElement("half_extents");
			if (!extentsElemenet)
			{
				extentsElemenet = doc->NewElement("half_extents");
				shapeElement->InsertEndChild(extentsElemenet);
			}
			if (extentsElemenet->FirstChildElement("x") == NULL)
			{
				extentsElemenet->InsertEndChild(doc->NewElement("x"));
			}
			extentsElemenet->FirstChildElement("x")->SetText(triggerComp->getShape()->halfExtents[0]);
			if (extentsElemenet->FirstChildElement("y") == NULL)
			{
				extentsElemenet->InsertEndChild(doc->NewElement("y"));
			}
			extentsElemenet->FirstChildElement("y")->SetText(triggerComp->getShape()->halfExtents[1]);
			if (extentsElemenet->FirstChildElement("z") == NULL)
			{
				extentsElemenet->InsertEndChild(doc->NewElement("z"));
			}
			extentsElemenet->FirstChildElement("z")->SetText(triggerComp->getShape()->halfExtents[2]);
			break;
		case ShapeData::CAPSULE:
			if (shapeElement->FirstChildElement("radius") == NULL)
			{
				shapeElement->InsertEndChild(doc->NewElement("radius"));
			}
			shapeElement->FirstChildElement("radius")->SetText(triggerComp->getShape()->radius);
			if (shapeElement->FirstChildElement("height") == NULL)
			{
				shapeElement->InsertEndChild(doc->NewElement("height"));
			}
			shapeElement->FirstChildElement("height")->SetText(triggerComp->getShape()->height);
			break;
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

bool FileSaver::AddLogicToFile(tinyxml2::XMLDocument * doc, tinyxml2::XMLElement * logicElement, LogicComponent * logic)
{
	tinyxml2::XMLElement* scriptElement = doc->NewElement("script");
	logicElement->InsertEndChild(scriptElement);
	string fullScriptName = logic->getScriptName() + ".lua";
	scriptElement->SetText(fullScriptName.c_str());

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

bool FileSaver::AddRigidBody(tinyxml2::XMLDocument * doc, tinyxml2::XMLElement * physicsElement, PhysicsComponent * phyComp)
{
	physicsElement->InsertEndChild(doc->NewElement("mass"));
	physicsElement->FirstChildElement("mass")->SetText(*phyComp->getMass());

	if (*phyComp->getMeshFileName() != "")
	{
		physicsElement->InsertEndChild(doc->NewElement("collision_mesh"));
		physicsElement->FirstChildElement("collision_mesh")->SetText(phyComp->getMeshFileName()->c_str());
	}
	else
	{
		//bounding shape
		tinyxml2::XMLElement* shapeElement = doc->NewElement("bounding_shape");
		if (shapeElement)
		{
			ShapeData::BoundingShape shapeType = phyComp->getShape()->boundingShape;
			string typeName = EnumParser<ShapeData::BoundingShape>().getString(shapeType);
			shapeElement->SetAttribute("type", typeName.c_str());
			tinyxml2::XMLElement* extentsElemenet;
			switch (shapeType)
			{
			case ShapeData::BOX:
				extentsElemenet = doc->NewElement("half_extents");

				shapeElement->InsertEndChild(extentsElemenet);
				extentsElemenet->InsertEndChild(doc->NewElement("x"));
				extentsElemenet->FirstChildElement("x")->SetText(phyComp->getShape()->halfExtents[0]);
				
				extentsElemenet->InsertEndChild(doc->NewElement("y"));
				extentsElemenet->FirstChildElement("y")->SetText(phyComp->getShape()->halfExtents[1]);
				
				extentsElemenet->InsertEndChild(doc->NewElement("z"));
				extentsElemenet->FirstChildElement("z")->SetText(phyComp->getShape()->halfExtents[2]);
				break;
			case ShapeData::SPHERE:
				shapeElement->InsertEndChild(doc->NewElement("radius"));
				shapeElement->FirstChildElement("radius")->SetText(phyComp->getShape()->radius);
				break;
			case ShapeData::CONE:
				shapeElement->InsertEndChild(doc->NewElement("radius"));
				shapeElement->FirstChildElement("radius")->SetText(phyComp->getShape()->radius);
				
				shapeElement->InsertEndChild(doc->NewElement("height"));
				shapeElement->FirstChildElement("height")->SetText(phyComp->getShape()->height);
				break;
			case ShapeData::CYLINDER:
				extentsElemenet = doc->NewElement("half_extents");
				shapeElement->InsertEndChild(extentsElemenet);
				
				extentsElemenet->InsertEndChild(doc->NewElement("x"));
				extentsElemenet->FirstChildElement("x")->SetText(phyComp->getShape()->halfExtents[0]);
				
				extentsElemenet->InsertEndChild(doc->NewElement("y"));
				extentsElemenet->FirstChildElement("y")->SetText(phyComp->getShape()->halfExtents[1]);
				
				extentsElemenet->InsertEndChild(doc->NewElement("z"));
				extentsElemenet->FirstChildElement("z")->SetText(phyComp->getShape()->halfExtents[2]);
				break;
			case ShapeData::CAPSULE:
				
				shapeElement->InsertEndChild(doc->NewElement("radius"));
				shapeElement->FirstChildElement("radius")->SetText(phyComp->getShape()->radius);
				
				shapeElement->InsertEndChild(doc->NewElement("height"));
				shapeElement->FirstChildElement("height")->SetText(phyComp->getShape()->height);
				break;
			default:
				break;
			}
		}
		physicsElement->InsertEndChild(shapeElement);
	}
	
	physicsElement->InsertEndChild(doc->NewElement("convex"));
	physicsElement->FirstChildElement("convex")->SetText(*phyComp->isConvex());

	physicsElement->InsertEndChild(doc->NewElement("restitution"));
	physicsElement->FirstChildElement("restitution")->SetText(phyComp->getRestitution());

	physicsElement->InsertEndChild(doc->NewElement("friction"));
	physicsElement->FirstChildElement("friction")->SetText(phyComp->getFriction());

	physicsElement->InsertEndChild(doc->NewElement("rotational_friction"));
	physicsElement->FirstChildElement("rotational_friction")->SetText(*phyComp->getRotationalFriction());

	physicsElement->InsertEndChild(doc->NewElement("constant_velocity"));
	physicsElement->FirstChildElement("constant_velocity")->SetText(*phyComp->isConstVelocity());

	physicsElement->InsertEndChild(doc->NewElement("velocity"));
	
	tinyxml2::XMLElement* velElement = physicsElement->FirstChildElement("velocity");
	velElement->InsertEndChild(doc->NewElement("x"));
	velElement->FirstChildElement("x")->SetText(*phyComp->getVelocity()[0]);
	
	velElement->InsertEndChild(doc->NewElement("y"));
	velElement->FirstChildElement("y")->SetText(*phyComp->getVelocity()[1]);
	
	velElement->InsertEndChild(doc->NewElement("z"));
	velElement->FirstChildElement("z")->SetText(*phyComp->getVelocity()[2]);
	return true;
}

bool FileSaver::AddTrigger(tinyxml2::XMLDocument * doc, tinyxml2::XMLElement * triggerElement, CollisionTrigger * triggerComp)
{
	//script
	tinyxml2::XMLElement* scriptElement = doc->NewElement("script");
	triggerElement->InsertEndChild(scriptElement);
	
	string fullScriptName = triggerComp->getScriptName() + ".lua";
	scriptElement->SetText(fullScriptName.c_str());
	//triggeronce
	tinyxml2::XMLElement* triggerOnceElement = doc->NewElement("trigger_once");
	triggerElement->InsertEndChild(triggerOnceElement);
	
	triggerOnceElement->SetText(triggerComp->isTriggerOnce());
	//bounding shape
	tinyxml2::XMLElement* shapeElement = doc->NewElement("bounding_shape");
	if (shapeElement)
	{
		ShapeData::BoundingShape shapeType = triggerComp->getShape()->boundingShape;
		string typeName = EnumParser<ShapeData::BoundingShape>().getString(shapeType);
		shapeElement->SetAttribute("type", typeName.c_str());
		tinyxml2::XMLElement* extentsElemenet;
		switch (shapeType)
		{
		case ShapeData::BOX:
			extentsElemenet = doc->NewElement("half_extents");

			shapeElement->InsertEndChild(extentsElemenet);
			extentsElemenet->InsertEndChild(doc->NewElement("x"));
			extentsElemenet->FirstChildElement("x")->SetText(triggerComp->getShape()->halfExtents[0]);

			extentsElemenet->InsertEndChild(doc->NewElement("y"));
			extentsElemenet->FirstChildElement("y")->SetText(triggerComp->getShape()->halfExtents[1]);

			extentsElemenet->InsertEndChild(doc->NewElement("z"));
			extentsElemenet->FirstChildElement("z")->SetText(triggerComp->getShape()->halfExtents[2]);
			break;
		case ShapeData::SPHERE:
			shapeElement->InsertEndChild(doc->NewElement("radius"));
			shapeElement->FirstChildElement("radius")->SetText(triggerComp->getShape()->radius);
			break;
		case ShapeData::CONE:
			shapeElement->InsertEndChild(doc->NewElement("radius"));
			shapeElement->FirstChildElement("radius")->SetText(triggerComp->getShape()->radius);

			shapeElement->InsertEndChild(doc->NewElement("height"));
			shapeElement->FirstChildElement("height")->SetText(triggerComp->getShape()->height);
			break;
		case ShapeData::CYLINDER:
			extentsElemenet = doc->NewElement("half_extents");
			shapeElement->InsertEndChild(extentsElemenet);

			extentsElemenet->InsertEndChild(doc->NewElement("x"));
			extentsElemenet->FirstChildElement("x")->SetText(triggerComp->getShape()->halfExtents[0]);

			extentsElemenet->InsertEndChild(doc->NewElement("y"));
			extentsElemenet->FirstChildElement("y")->SetText(triggerComp->getShape()->halfExtents[1]);

			extentsElemenet->InsertEndChild(doc->NewElement("z"));
			extentsElemenet->FirstChildElement("z")->SetText(triggerComp->getShape()->halfExtents[2]);
			break;
		case ShapeData::CAPSULE:

			shapeElement->InsertEndChild(doc->NewElement("radius"));
			shapeElement->FirstChildElement("radius")->SetText(triggerComp->getShape()->radius);

			shapeElement->InsertEndChild(doc->NewElement("height"));
			shapeElement->FirstChildElement("height")->SetText(triggerComp->getShape()->height);
			break;
		default:
			break;
		}
	}
		triggerElement->InsertEndChild(shapeElement);

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

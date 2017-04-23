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
				if (!SkipObject)
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
											auto model = gameScreen->getComponentStore()->getComponent<ModelComponent>(go->GetComponentHandle(ComponentType::MODEL), ComponentType::MODEL);
											if (!model)
												return false;
											for (int transformElement = 0; transformElement < 3; transformElement++)
											{
												switch (transformElement)
												{
												case 0:
												{
													tinyxml2::XMLElement* componentInnerElement = componentElement->FirstChildElement("position");
													if (!componentInnerElement)
														break;
													for (int vector3 = 0; vector3 < 3; vector3++)
													{
														//xyz
														switch (vector3)
														{
														case 0:
														{
															std::ostringstream ss;
															ss << model->getTransform()->position[vector3];
															componentInnerElement->FirstChildElement("x")->SetText(string(ss.str()).c_str());
															break;
														}
														case 1:
														{
															std::ostringstream ss;
															ss << model->getTransform()->position[vector3];
															componentInnerElement->FirstChildElement("y")->SetText(string(ss.str()).c_str());
															break;
														}

														case 2:
														{
															std::ostringstream ss;
															ss << model->getTransform()->position[vector3];
															componentInnerElement->FirstChildElement("z")->SetText(string(ss.str()).c_str());
															break;
														}
														}

													}
													break;
												}
												case 1:
												{
													tinyxml2::XMLElement* componentInnerElement = componentElement->FirstChildElement("scale");
													if (!componentInnerElement)
														break;
													for (int vector3 = 0; vector3 < 3; vector3++)
													{
														//xyz
														switch (vector3)
														{
														case 0:
														{
															std::ostringstream ss;
															ss << model->getTransform()->scale[vector3];
															componentInnerElement->FirstChildElement("x")->SetText(string(ss.str()).c_str());
															break;
														}
														case 1:
														{
															std::ostringstream ss;
															ss << model->getTransform()->scale[vector3];
															componentInnerElement->FirstChildElement("y")->SetText(string(ss.str()).c_str());
															break;
														}

														case 2:
														{
															std::ostringstream ss;
															ss << model->getTransform()->scale[vector3];
															componentInnerElement->FirstChildElement("z")->SetText(string(ss.str()).c_str());
															break;
														}
														}

													}
													break;
												}
												case 2:
												{
													tinyxml2::XMLElement* componentInnerElement = componentElement->FirstChildElement("orientation");
													if (!componentInnerElement)
														break;
													for (int vector3 = 0; vector3 < 3; vector3++)
													{
														//xyz
														switch (vector3)
														{
														case 0:
														{
															std::ostringstream ss;
															ss << model->getTransform()->orientation[vector3];
															componentInnerElement->FirstChildElement("x")->SetText(string(ss.str()).c_str());
															break;
														}
														case 1:
														{
															std::ostringstream ss;
															ss << model->getTransform()->orientation[vector3];
															componentInnerElement->FirstChildElement("y")->SetText(string(ss.str()).c_str());
															break;
														}

														case 2:
														{
															std::ostringstream ss;
															ss << model->getTransform()->orientation[vector3];
															componentInnerElement->FirstChildElement("z")->SetText(string(ss.str()).c_str());
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
				UpdateTransform(doc, XMLUIElement, uiElement->getTransform());
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
		//If Texture
		if (uiE->getType() == UIType::TEXTURE)
		{
			tinyxml2::XMLElement* XMLUIElementID = doc->NewElement("Texture");

			XMLUIElementID->SetText(dynamic_pointer_cast<UITextureElement>(uiE)->getTextureName().c_str());
			XMLUIElement->InsertEndChild(XMLUIElementID);
		}

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


bool FileSaver::UpdateTransform(tinyxml2::XMLDocument* doc, tinyxml2::XMLElement* transformElement, shared_ptr<Transform> transform)
{
	for (int transformElementIt = 0; transformElementIt < 3; transformElementIt++)
	{
		switch (transformElementIt)
		{
		case 0:
		{
			tinyxml2::XMLElement* transformInnerElement = transformElement->FirstChildElement("position");
			if (!transformInnerElement)
			{
				tinyxml2::XMLElement* xyzElement;
				//Create a position element.
				transformInnerElement = doc->NewElement("position");
				xyzElement = doc->NewElement("x");
				transformInnerElement->InsertEndChild(xyzElement);
				xyzElement = doc->NewElement("y");
				transformInnerElement->InsertEndChild(xyzElement);
				xyzElement = doc->NewElement("z");
				transformInnerElement->InsertEndChild(xyzElement);
				transformElement->InsertEndChild(transformInnerElement);
			}

			for (int vector3 = 0; vector3 < 3; vector3++)
			{
				//xyz
				switch (vector3)
				{
				case 0:
				{
					std::ostringstream ss;
					ss << transform->position[vector3];
					transformInnerElement->FirstChildElement("x")->SetText(string(ss.str()).c_str());
					break;
				}
				case 1:
				{
					std::ostringstream ss;
					ss << transform->position[vector3];
					transformInnerElement->FirstChildElement("y")->SetText(string(ss.str()).c_str());
					break;
				}

				case 2:
				{
					std::ostringstream ss;
					ss << transform->position[vector3];
					transformInnerElement->FirstChildElement("z")->SetText(string(ss.str()).c_str());
					break;
				}
				}

			}
			break;
		}
		case 1:
		{
			tinyxml2::XMLElement* transformInnerElement = transformElement->FirstChildElement("scale");
			if (!transformInnerElement)
			{
				tinyxml2::XMLElement* xyzElement;
				//Create a scale element.
				transformInnerElement = doc->NewElement("scale");
				xyzElement = doc->NewElement("x");
				transformInnerElement->InsertEndChild(xyzElement);
				xyzElement = doc->NewElement("y");
				transformInnerElement->InsertEndChild(xyzElement);
				xyzElement = doc->NewElement("z");
				transformInnerElement->InsertEndChild(xyzElement);
				transformElement->InsertEndChild(transformInnerElement);
			}
			for (int vector3 = 0; vector3 < 3; vector3++)
			{
				//xyz
				switch (vector3)
				{
				case 0:
				{
					std::ostringstream ss;
					ss << transform->scale[vector3];
					transformInnerElement->FirstChildElement("x")->SetText(string(ss.str()).c_str());
					break;
				}
				case 1:
				{
					std::ostringstream ss;
					ss << transform->scale[vector3];
					transformInnerElement->FirstChildElement("y")->SetText(string(ss.str()).c_str());
					break;
				}

				case 2:
				{
					std::ostringstream ss;
					ss << transform->scale[vector3];
					transformInnerElement->FirstChildElement("z")->SetText(string(ss.str()).c_str());
					break;
				}
				}

			}
			break;
		}
		case 2:
		{
			tinyxml2::XMLElement* transformInnerElement = transformElement->FirstChildElement("orientation");
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
				transformElement->InsertEndChild(transformInnerElement);
			}
			for (int vector3 = 0; vector3 < 3; vector3++)
			{
				//xyz
				switch (vector3)
				{
				case 0:
				{
					std::ostringstream ss;
					ss << transform->orientation[vector3];
					transformInnerElement->FirstChildElement("x")->SetText(string(ss.str()).c_str());
					break;
				}
				case 1:
				{
					std::ostringstream ss;
					ss << transform->orientation[vector3];
					transformInnerElement->FirstChildElement("y")->SetText(string(ss.str()).c_str());
					break;
				}

				case 2:
				{
					std::ostringstream ss;
					ss << transform->orientation[vector3];
					transformInnerElement->FirstChildElement("z")->SetText(string(ss.str()).c_str());
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

bool FileSaver::AddTransformToFile(tinyxml2::XMLDocument * doc, tinyxml2::XMLElement * XMLUIElement, Transform* transform)
{
	//Transform
	for (int transformElement = 0; transformElement < 3; transformElement++)
	{
		switch (transformElement)
		{
		case 0:
		{
			tinyxml2::XMLElement* innerElement = doc->NewElement("position");
			XMLUIElement->InsertEndChild(innerElement);
			if (!innerElement)
				break;
			for (int vector3 = 0; vector3 < 3; vector3++)
			{
				//xyz
				switch (vector3)
				{
				case 0:
				{
					std::ostringstream ss;
					ss << transform->position[vector3];
					tinyxml2::XMLElement* vecElement = doc->NewElement("x");
					innerElement->InsertEndChild(vecElement);
					vecElement->SetText(string(ss.str()).c_str());
					break;
				}
				case 1:
				{
					std::ostringstream ss;
					ss << transform->position[vector3];
					tinyxml2::XMLElement* vecElement = doc->NewElement("y");
					innerElement->InsertEndChild(vecElement);
					vecElement->SetText(string(ss.str()).c_str());
					break;
				}

				case 2:
				{
					std::ostringstream ss;
					ss << transform->position[vector3];
					tinyxml2::XMLElement* vecElement = doc->NewElement("z");
					innerElement->InsertEndChild(vecElement);
					vecElement->SetText(string(ss.str()).c_str());
					break;
				}
				}

			}
			break;
		}
		case 1:
		{
			tinyxml2::XMLElement* innerElement = doc->NewElement("scale");
			XMLUIElement->InsertEndChild(innerElement);
			if (!innerElement)
				break;
			for (int vector3 = 0; vector3 < 3; vector3++)
			{
				//xyz
				switch (vector3)
				{
				case 0:
				{
					std::ostringstream ss;
					ss << transform->scale[vector3];
					tinyxml2::XMLElement* vecElement = doc->NewElement("x");
					innerElement->InsertEndChild(vecElement);
					vecElement->SetText(string(ss.str()).c_str());
					break;
				}
				case 1:
				{
					std::ostringstream ss;
					ss << transform->scale[vector3];
					tinyxml2::XMLElement* vecElement = doc->NewElement("y");
					innerElement->InsertEndChild(vecElement);
					vecElement->SetText(string(ss.str()).c_str());
					break;
				}

				case 2:
				{
					std::ostringstream ss;
					ss << transform->scale[vector3];
					tinyxml2::XMLElement* vecElement = doc->NewElement("z");
					innerElement->InsertEndChild(vecElement);
					vecElement->SetText(string(ss.str()).c_str());
					break;
				}
				}

			}
			break;
		}
		case 2:
		{
			tinyxml2::XMLElement* innerElement = doc->NewElement("orientation");
			XMLUIElement->InsertEndChild(innerElement);
			if (!innerElement)
				break;
			for (int vector3 = 0; vector3 < 3; vector3++)
			{
				//xyz
				switch (vector3)
				{
				case 0:
				{
					std::ostringstream ss;
					ss << transform->orientation[vector3];
					tinyxml2::XMLElement* vecElement = doc->NewElement("x");
					innerElement->InsertEndChild(vecElement);
					vecElement->SetText(string(ss.str()).c_str());
					break;
				}
				case 1:
				{
					std::ostringstream ss;
					ss << transform->orientation[vector3];
					tinyxml2::XMLElement* vecElement = doc->NewElement("y");
					innerElement->InsertEndChild(vecElement);
					vecElement->SetText(string(ss.str()).c_str());
					break;
				}

				case 2:
				{
					std::ostringstream ss;
					ss << transform->orientation[vector3];
					tinyxml2::XMLElement* vecElement = doc->NewElement("z");
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

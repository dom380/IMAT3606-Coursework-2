#include <utils\FileSaver.h>
#include <utils\XMLReader.h>
using namespace ComponentType;
bool FileSaver::UpdateFile(tinyxml2::XMLDocument * doc, int iObjectCount, shared_ptr<GameObject> go, shared_ptr<GameScreen> gameScreen)
{
	int XMLObjectCount = 0;
	bool SkipObject = false;
	if (doc)
	{
		tinyxml2::XMLElement* screenElement = doc->FirstChildElement("screen");
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
							for (int i = ComponentTypes::MODEL; i < ComponentTypes::COMPONENT_TYPE_COUNT; i++)
							{
								ComponentTypes cType = static_cast<ComponentType::ComponentTypes>(i);
								string ObjectCompType = ComponentNames[cType];
								
								if (go->HasComponent(cType))
								{
									//XML current GO and passedGO have same component
									if (XMLCompType == ObjectCompType)
									{
										/*
											Switch between all components
										*/
										switch (cType)
										{
										case MODEL:
										{
											auto model = gameScreen->getComponentStore()->getComponent<ModelComponent>(go->GetComponentHandle(ComponentType::ComponentTypes::MODEL), ComponentType::ComponentTypes::MODEL);
											tinyxml2::XMLElement* nameElement = componentElement->FirstChildElement("file");
											string NameElementText = nameElement->GetText();
											/*
												Check our XML GO name is the same as the GO name
											*/
											if (NameElementText == model->getId())
											{
												//check for objects with the same name (multiple wall.obj for example)
												if (XMLObjectCount != iObjectCount)
												{
													SkipObject = true;
													break;
												}
												else
												{
													SkipObject = false;
													break;
												}
											}
											else {
												SkipObject = true;
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
											auto model = gameScreen->getComponentStore()->getComponent<ModelComponent>(go->GetComponentHandle(ComponentType::ComponentTypes::MODEL), ComponentType::ComponentTypes::MODEL);
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

bool FileSaver::AddObjectToFile(tinyxml2::XMLDocument* doc, int iObjectCount, shared_ptr<GameObject> go, shared_ptr<GameScreen> gameScreen)
{
	tinyxml2::XMLElement* screenElement = doc->FirstChildElement("screen");

	const char* type = screenElement->Attribute("type");
	if (string(type) == string("level")) {
		tinyxml2::XMLElement* gameObjsElement = screenElement->FirstChildElement("gameObjects");
		tinyxml2::XMLElement* gameObjectElement = doc->NewElement("gameobject");
		tinyxml2::XMLElement* componentsElement = doc->NewElement("components");
		
		//go through each component
		for (int i = ComponentTypes::MODEL; i < ComponentTypes::COMPONENT_TYPE_COUNT; i++)
		{
			ComponentTypes cType = static_cast<ComponentType::ComponentTypes>(i);
			string ObjectCompType = ComponentNames[cType];
			if (go->HasComponent(cType))
			{
				/*
					Add new component as child of components
				*/
				tinyxml2::XMLElement* componentElement = doc->NewElement("component");
				componentElement->SetAttribute("type", ObjectCompType.c_str());
				componentsElement->InsertEndChild(componentElement);
				switch (cType)
				{
				case MODEL:
				{
					/*
						Give component model information, optional texture
					*/
					auto model = gameScreen->getComponentStore()->getComponent<ModelComponent>(go->GetComponentHandle(ComponentType::ComponentTypes::MODEL), ComponentType::ComponentTypes::MODEL);
					tinyxml2::XMLElement* nameElement = doc->NewElement("file");
					componentElement->InsertEndChild(nameElement);
					nameElement->SetText(model->getId().c_str());
					if (model->getTextureName() != "")
					{
						tinyxml2::XMLElement* textureElement = doc->NewElement("texture");
						componentElement->InsertEndChild(textureElement);
						textureElement->SetText(model->getTextureName().c_str());
					}
					
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
					
					auto model = gameScreen->getComponentStore()->getComponent<ModelComponent>(go->GetComponentHandle(ComponentType::ComponentTypes::MODEL), ComponentType::ComponentTypes::MODEL);
					for (int transformElement = 0; transformElement < 3; transformElement++)
					{
						switch (transformElement)
						{
						case 0:
						{
							tinyxml2::XMLElement* componentInnerElement = doc->NewElement("position");
							componentElement->InsertEndChild(componentInnerElement);
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
									tinyxml2::XMLElement* componentVecElement = doc->NewElement("x");
									componentInnerElement->InsertEndChild(componentVecElement);
									componentVecElement->SetText(string(ss.str()).c_str());
									break;
								}
								case 1:
								{
									std::ostringstream ss;
									ss << model->getTransform()->position[vector3];
									tinyxml2::XMLElement* componentVecElement = doc->NewElement("y");
									componentInnerElement->InsertEndChild(componentVecElement);
									componentVecElement->SetText(string(ss.str()).c_str());
									break;
								}

								case 2:
								{
									std::ostringstream ss;
									ss << model->getTransform()->position[vector3];
									tinyxml2::XMLElement* componentVecElement = doc->NewElement("z");
									componentInnerElement->InsertEndChild(componentVecElement);
									componentVecElement->SetText(string(ss.str()).c_str());
									break;
								}
								}

							}
							break;
						}
						case 1:
						{
							tinyxml2::XMLElement* componentInnerElement = doc->NewElement("scale");
							componentElement->InsertEndChild(componentInnerElement);
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
									tinyxml2::XMLElement* componentVecElement = doc->NewElement("x");
									componentInnerElement->InsertEndChild(componentVecElement);
									componentVecElement->SetText(string(ss.str()).c_str());
									break;
								}
								case 1:
								{
									std::ostringstream ss;
									ss << model->getTransform()->scale[vector3];
									tinyxml2::XMLElement* componentVecElement = doc->NewElement("y");
									componentInnerElement->InsertEndChild(componentVecElement);
									componentVecElement->SetText(string(ss.str()).c_str());
									break;
								}

								case 2:
								{
									std::ostringstream ss;
									ss << model->getTransform()->scale[vector3];
									tinyxml2::XMLElement* componentVecElement = doc->NewElement("z");
									componentInnerElement->InsertEndChild(componentVecElement);
									componentVecElement->SetText(string(ss.str()).c_str());
									break;
								}
								}

							}
							break;
						}
						case 2:
						{
							tinyxml2::XMLElement* componentInnerElement = doc->NewElement("orientation");
							componentElement->InsertEndChild(componentInnerElement);
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
									tinyxml2::XMLElement* componentVecElement = doc->NewElement("x");
									componentInnerElement->InsertEndChild(componentVecElement);
									componentVecElement->SetText(string(ss.str()).c_str());
									break;
								}
								case 1:
								{
									std::ostringstream ss;
									ss << model->getTransform()->orientation[vector3];
									tinyxml2::XMLElement* componentVecElement = doc->NewElement("y");
									componentInnerElement->InsertEndChild(componentVecElement);
									componentVecElement->SetText(string(ss.str()).c_str());
									break;
								}

								case 2:
								{
									std::ostringstream ss;
									ss << model->getTransform()->orientation[vector3];
									tinyxml2::XMLElement* componentVecElement = doc->NewElement("z");
									componentInnerElement->InsertEndChild(componentVecElement);
									componentVecElement->SetText(string(ss.str()).c_str());
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
		
	
		//Insert new object at end of gameobjects file.
		gameObjectElement->InsertEndChild(componentsElement);
		gameObjsElement->InsertEndChild(gameObjectElement);
		


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

#include "UIManager.h"
#include <Engine.h>
#include "UITextureElement.h"
#include <utils\LevelLoader.h>

bool UIManager::initialised = false;
shared_ptr<UIManager> UIManager::instance;
#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))


shared_ptr<UIManager> UIManager::getInstance()
{
	if (initialised) {
		
		return instance;
	}
	instance = shared_ptr<UIManager>(new UIManager());
	initialised = true;
	instance->clearable = false;
	return instance;
}

void UIManager::debugMenuItemUpdate()
{
	if (ImGui::BeginMenu("UI"))
	{
		vector<shared_ptr<UIElement>> uiElements = Engine::g_pEngine->getActiveScreen()->getUIElements();
		for (int x = 0; x < uiElements.size(); x++)
		{
			reset();
			if (opened.size() < uiElements.size())
			{
				opened.push_back(true);
			}
			char uiName[50];
			snprintf(uiName, sizeof(uiName), "UI_%s_%d", uiElements[x]->getID().c_str(), x);
			//Tree node creates a tree from the ui object name
			if (ImGui::TreeNode(uiName))
			{
				hasButton = uiElements[x]->getButton() ? uiElements[x]->getButton()->isActive() : false;
				if (!opened[x])
				{
					//init
					opened[x] = true;
				}
				
				//ID
				if (strlen(uiID) == 0)
				{
					strncpy_s(uiID, uiElements.at(x)->getID().c_str(), uiElements.at(x)->getID().size());
				}

				ImGui::InputText("UIID", uiID, sizeof(uiID));
				uiElements[x]->setID(uiID);
				

				//transform
				DebugMenu::getInstance()->gameObjectsMenuTransform(x, uiElements[x]->getTransform().get());
				uiElements[x]->setModel(glm::mat4(1.0f));
				uiElements[x]->updateModelUsingTransform();

				//HasButton?
				if (ImGui::Checkbox("HasButton", &hasButton))
				{
					
					if (hasButton)
					{
						if (uiElements[x]->getButton())
						{
							uiElements[x]->getButton()->setActive(hasButton);
							Engine::g_pEngine->getInput()->registerMouseListener(uiElements[x]->getButton());
						}
						else
						{
							//Add a button
							LevelLoader::loadButtonElement(uiElements[x]);
							uiElements[x]->getButton()->setActive(hasButton);
						}
						
					}
					else
					{
						if (uiElements[x]->getButton())
						{
							uiElements[x]->getButton()->setActive(hasButton);
							Engine::g_pEngine->getInput()->removeMouseListener(uiElements[x]->getButton());
						}
					}
				}
				if (hasButton)
				{

					if (ImGui::TreeNode("Button"))
					{
						
						if (strlen(buttonScriptName) == 0)
						{
							strncpy_s(buttonScriptName, uiElements.at(x)->getButton()->getScript().c_str(), uiElements.at(x)->getButton()->getScript().size());
						}
					
						if (strlen(buttonFuncName) == 0)
						{
							strncpy_s(buttonFuncName, uiElements.at(x)->getButton()->getFunc().c_str(), uiElements.at(x)->getButton()->getFunc().size());
						}
						
						if (strlen(buttonParamID) == 0)
						{
							if (uiElements.at(x)->getButton()->getParams().size() > 0)
							{
								strncpy_s(buttonParamID, uiElements.at(x)->getButton()->getParams()[0].first.c_str(), uiElements.at(x)->getButton()->getParams()[0].first.size());
							}
						}
						
						if (strlen(buttonParamText) == 0)
						{
							if (uiElements.at(x)->getButton()->getParams().size() > 0)
							{
								strncpy_s(buttonParamText, uiElements.at(x)->getButton()->getParams()[0].second.c_str(), uiElements.at(x)->getButton()->getParams()[0].second.size());
							}
						}
						//inputs
						ImGui::InputText("ScriptName", buttonScriptName, sizeof(buttonScriptName));
						ImGui::InputText("FuncName", buttonFuncName, sizeof(buttonFuncName));
						ImGui::InputText("ParamID", buttonParamID, sizeof(buttonParamID));
						ImGui::InputText("ParamText", buttonParamText, sizeof(buttonParamText));
						uiElements[x]->getButton()->setScript(buttonScriptName);
						uiElements[x]->getButton()->setFunc(buttonFuncName);
						if (ImGui::Button("UpdateButton"))
						{
							uiElements[x]->getButton()->setParam(pair<string, string>(buttonParamID, buttonParamText));
							LevelLoader::loadButtonFunc(uiElements[x]->getButton());
						}
						ImGui::SameLine();
						if (ImGui::Button("ClearButtonParams"))
						{
							uiElements[x]->getButton()->clearParams();
						}
						ImGui::TreePop();
					}
				}
				
				//Specific ui
				switch (uiElements.at(x)->getType())
				{
				case UIType::TEXT:
				{
					//edit text value
					shared_ptr<TextBox> uiText = dynamic_pointer_cast<TextBox>(uiElements.at(x));

					if (strlen(uiTextValue) == 0)
					{
						strncpy_s(uiTextValue, uiText->getText().c_str(), uiText->getText().size());
					}

					ImGui::InputText("Value", uiTextValue, sizeof(uiTextValue));
					uiText->updateText(uiTextValue);

					//colour
					static float dragSpeed = 0.0025f;
					glm::vec3 colour = uiText->getColour();
					ImGui::DragFloat3("Colour", &colour[0], dragSpeed);
					uiText->setColour(colour);
					//font
					DebugMenu::getInstance()->createFontsListBox();
					Font font = *AssetManager::getInstance()->getFont(const_cast<char*>(DebugMenu::getInstance()->listBoxItemSelected(uiText->getType()).c_str()), Engine::g_pEngine->getRenderer());
					uiText->setFont(font);
					
					//button
					if (hasButton)
					{
						//reinit bounding box
						uiElements[x]->getButton()->buildAABB(font, uiText->getText(), uiElements[x]->getTransform());
					}
					break;
				}
				case UIType::TEXTURE:
				{
					DebugMenu::getInstance()->createTextureListBox();
					shared_ptr<UITextureElement> uiT = dynamic_pointer_cast<UITextureElement>(uiElements.at(x));
					if (ImGui::Button("ApplyTexture"))
					{
						uiT->setTexture(DebugMenu::getInstance()->listBoxItemSelected(uiElements.at(x)->getType()));
					}
					//button
					if (hasButton)
					{
						//reinit bounding box
						uiElements[x]->getButton()->buildAABB(uiElements[x]->getTransform());
					}
					break;
				}
				default:
					break;
				}
				bool active = uiElements[x]->isActive();
				if (ImGui::Checkbox("Toggle", &active))
				{
					uiElements[x]->setActive(active);
				}

				ImGui::TreePop();
			}
			else
			{
				opened[x] = false;
			}
		}
		ImGui::EndMenu();
	}
}

void UIManager::reset()
{
	hasButton = false;
	if (strlen(uiID) != 0)
		strncpy_s(uiID, "", 1);
	if (strlen(buttonScriptName) != 0)
		strncpy_s(buttonScriptName, "", 1);
	if (strlen(buttonFuncName) != 0)
		strncpy_s(buttonFuncName, "", 1);
	if (strlen(uiTextValue) != 0)
	{
		strncpy_s(uiTextValue, "", 1);
	}
}

void UIManager::update()
{
}

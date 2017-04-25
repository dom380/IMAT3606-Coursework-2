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
			char uiName[50];
			snprintf(uiName, sizeof(uiName), "UI_%s_%d", uiElements[x]->getID().c_str(), x);
			//Tree node creates a tree from the ui object name
			if (ImGui::TreeNode(uiName))
			{
				//ID
				static char uiID[64] = "";

				if (strlen(uiID) == 0)
				{
					strncpy_s(uiID, uiElements.at(x)->getID().c_str(), uiElements.at(x)->getID().size());
				}

				ImGui::InputText("UIID", uiID, sizeof(uiID));
				if (ImGui::Button("ApplyID"))
				{
					uiElements[x]->setID(uiID);
				}

				//transform
				DebugMenu::getInstance()->gameObjectsMenuTransform(x, uiElements[x]->getTransform().get());
				uiElements[x]->setModel(glm::mat4(1.0f));
				uiElements[x]->updateModelUsingTransform();

				//HasButton?
				static bool hasButton = uiElements[x]->getButton() ? uiElements[x]->getButton()->isActive() : false;
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
						}
						
					}
					else
					{
						if (uiElements[x]->getButton())
						{
							Engine::g_pEngine->getInput()->removeMouseListener(uiElements[x]->getButton());
						}
					}
				}
				if (hasButton)
				{
					if (ImGui::TreeNode("Button"))
					{
						static char buttonScriptName[64] = "";
						if (strlen(buttonScriptName) == 0)
						{
							strncpy_s(buttonScriptName, uiElements.at(x)->getButton()->getScript().c_str(), uiElements.at(x)->getButton()->getScript().size());
						}
						static char buttonFuncName[64] = "";
						if (strlen(buttonFuncName) == 0)
						{
							strncpy_s(buttonFuncName, uiElements.at(x)->getButton()->getFunc().c_str(), uiElements.at(x)->getButton()->getFunc().size());
						}
						static char buttonParamID[64] = "";
						if (strlen(buttonParamID) == 0)
						{
							if (uiElements.at(x)->getButton()->getParams().size() > 0)
							{
								strncpy_s(buttonParamID, uiElements.at(x)->getButton()->getParams()[0].first.c_str(), uiElements.at(x)->getButton()->getParams()[0].first.size());
							}
						}
						static char buttonParamText[64] = "";
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

						if (ImGui::Button("UpdateButton"))
						{
							uiElements[x]->getButton()->setScript(buttonScriptName);
							uiElements[x]->getButton()->setFunc(buttonFuncName);
							uiElements[x]->getButton()->setParam(pair<string, string>(buttonParamID, buttonParamText));
							LevelLoader::loadButtonFunc(uiElements[x]->getButton());
						}
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
					static char uiValue[64] = "";

					if (strlen(uiValue) == 0)
					{
						strncpy_s(uiValue, uiText->getText().c_str(), uiText->getText().size());
					}

					ImGui::InputText("Value", uiValue, sizeof(uiValue));
					uiText->updateText(uiValue);

					//colour
					static float dragSpeed = 0.0025f;
					static glm::vec3 colour = uiText->getColour();
					ImGui::DragFloat3("Colour", &colour[0], dragSpeed);
					uiText->setColour(colour);
					//font
					DebugMenu::getInstance()->createFontsListBox();
					Font font = *AssetManager::getInstance()->getFont(const_cast<char*>(DebugMenu::getInstance()->listBoxItemSelected(uiText->getType()).c_str()), Engine::g_pEngine->getRenderer());
					uiText->setFont(font);
					
					//AttemptRebuild
					if (ImGui::Button("Update"))
					{
						uiText->init();
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

					break;
				}
				case UIType::BUTTON:
					break;
				default:
					break;
				}
				

				ImGui::TreePop();
			}
			
		}
		ImGui::EndMenu();
	}
}

void UIManager::update()
{
}

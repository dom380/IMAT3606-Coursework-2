#include "UIManager.h"
#include "UITextureElement.h"

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
					uiText->setText(uiValue);
					break;
				}
				case UIType::TEXTURE:
				{
					DebugMenu::getInstance()->createTextureListBox();
					shared_ptr<UITextureElement> uiT = dynamic_pointer_cast<UITextureElement>(uiElements.at(x));
					if (ImGui::Button("ApplyTexture"))
					{
						uiT->setTexture(DebugMenu::getInstance()->textureItemSelected());
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

#include "UIManager.h"

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
	return instance;
}

void UIManager::debugMenuItemUpdate()
{
	if (ImGui::BeginMenu("UI"))
	{
		vector<shared_ptr<UIElement>> uiElements = Engine::g_pEngine->getActiveScreen()->getUIElements();
		for (int x = 0; x < uiElements.size(); x++)
		{
			ImGui::PushID(x);
			char uiName[50];
			snprintf(uiName, sizeof(uiName), "UI_%s_%d", uiElements[x]->getID().c_str(), x);
			//Tree node creates a tree from the ui object name
			if (ImGui::TreeNode(uiName))
			{
				DebugMenu::getInstance()->gameObjectsMenuTransform(x, uiElements[x]->getTransform().get());
				uiElements[x]->setModel(glm::mat4(1.0f));
				uiElements[x]->updateModelUsingTransform();
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
		ImGui::EndMenu();
	}
}

void UIManager::update()
{
}

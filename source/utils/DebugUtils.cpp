#include "..\..\include\utils\DebugUtils.h"

bool DebugUtils::initialised = false;
shared_ptr<DebugUtils> DebugUtils::instance;
shared_ptr<DebugUtils> DebugUtils::getInstance()
{
	if (initialised) {
		return instance;
	}
	instance = shared_ptr<DebugUtils>(new DebugUtils());
	instance->init();
	initialised = true;
	return instance;
}

void DebugUtils::init()
{
	popupActive = false;
}

void DebugUtils::update()
{
	if (popupActive)
	{
		ImGui::OpenPopup(popupHeaderText.c_str());
		popupActive = false;
	}
	if (ImGui::BeginPopup(popupHeaderText.c_str()))
	{
		if (popupDescriptionText.empty())
		{
			ImGui::Text(popupHeaderText.c_str());
		}
		else
		{
			ImGui::Text(popupDescriptionText.c_str());
		}
		
		ImGui::EndPopup();
	}
	else
	{
		//we want to reset description after popup closed, incase next popup has no desc.
		popupDescriptionText = "";
	}
}

void DebugUtils::popup(string headerText, string DescText)
{
	popupActive = true;
	popupHeaderText = headerText;
	if (popupDescriptionText.empty())
		popupDescriptionText = DescText;
}

void DebugUtils::setPopupDescription(string DescText)
{
	popupDescriptionText = DescText;
}

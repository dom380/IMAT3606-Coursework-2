#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

#include <algorithm>
#include <memory>
#include <Editor/imgui/imgui.h>
#include <Graphics\ModelComponent.h>

using std::shared_ptr;
/*
DEBUG UTILS
SINGLETON (Not static, requires updating in order to maintain what debug utils are active)

Utilities, linked to the imgui debug menu.
Allows for generic debugging functions, such as popups (for errors).

*/

class DebugUtils
{
private:
	DebugUtils() {};
	DebugUtils(DebugUtils const&) {};
	void operator=(DebugUtils const&) {};

	static bool initialised;
	static shared_ptr<DebugUtils> instance;
	/*
	Allows the popup to be shown.
	Immedietaly inactive once popup is rendered.
	Not related to when the popup is actually closed.
	*/
	bool popupActive;

	//Header/status for popup 
	string popupHeaderText;
	//Description, to be shown to the user
	string popupDescriptionText;

	

public:
	static shared_ptr<DebugUtils> getInstance();

	void init();
	/*
	Master update
	*/
	void update();
	/*
		popup - pass a status to the popup. 
		if no description is provided, the header
		will be used as the description.

	*/
	void popup(string popupHeaderText, string popupDescText="");
	/*
		The description to go into the popup.
	*/
	void setPopupDescription(string errorText);

};
#endif // !DEBUGUTILS_H

#include "..\..\..\include\Editor\imgui\ImguiHandler.h"
#include "Editor/imgui/imgui.h"

bool ImguiHandler::imGuiHasInputFocus()
{
	ImGuiIO& io = ImGui::GetIO();
	bool hasFocus = false;

	if (io.WantCaptureKeyboard)
		hasFocus |= true;
	if (io.WantCaptureMouse)
		hasFocus |= true;
	if (io.WantTextInput)
		hasFocus |= true;
	return hasFocus;
}

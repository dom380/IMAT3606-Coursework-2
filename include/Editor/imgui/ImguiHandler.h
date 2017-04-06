#pragma once
#ifndef IMGUIHANDLER_H
#define IMGUIHANDLER_H

#include <memory>
using std::shared_ptr;
/*
Abstract class for handling imgui
*/

class Window;
class ImguiHandler {
private:
public:
	/*
		Pass the base window
	*/
	virtual bool init(Window* window) = 0;
	/*
	Calls invalidateDeviceObjects & Base ImGui Shutdown
	*/
	virtual void shutdown() = 0;
	/*
	Setup specific inputs, window size.
	Then calls Base NewFrame.
	*/
	virtual void newFrame() = 0;
	/*
	Deletes handles for data & textures
	*/
	virtual void invalidateDeviceObjects() = 0;
	/*
	Creates & setup handles, shaders, font texture
	*/
	virtual bool createDeviceObjects() = 0;
	/*
	Create font texture by building a texture atlas and binding.
	*/
	virtual bool createFontsTexture() = 0;

	virtual void        imGuiMouseButtonCallback(shared_ptr<Window> window, int button, int action, int mods) = 0;
	virtual void        imGuiScrollCallback(shared_ptr<Window> window, double xoffset, double yoffset) = 0;
	virtual void        imGuiKeyCallback(shared_ptr<Window> window, int key, int scancode, int action, int mods) = 0;
	virtual void        imGuiCharCallback(shared_ptr<Window> window, unsigned int c) = 0;
	/*
	Check if imGUI has requested input
	*/
	virtual bool		imGuiHasInputFocus();
};

#endif
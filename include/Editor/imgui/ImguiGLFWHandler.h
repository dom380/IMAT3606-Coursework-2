#ifndef  IMGUIGLFWHANDER_H
#define  IMGUIGLFWHANDER_H

/*
	An IMGUI handler for GLFW based GL program.
	TODO: Base handler for IMGUI for multiple gl programs.
*/
#include <Editor/imgui/imguiHandler.h>
#include <memory>
using std::shared_ptr;


class Window;
class ImguiGLFWHandler : ImguiHandler
{
private:
	/*
		Copy and Paste?
	*/
	static const char*	GetClipboardText(void* user_data);
	static void			SetClipboardText(void* user_data, const char* text);

public:
	ImguiGLFWHandler() {};
	/*
		Setup Callbacks & Key Mapping
	*/
	bool init(Window* window);
	/*
		Calls invalidateDeviceObjects & Base ImGui Shutdown
	*/
	void shutdown();
	/*
		Setup GLFW specific inputs, window size. 
		Then calls Base NewFrame.
	*/
	void newFrame();
	/*
		Deletes handles for data & textures
	*/
	void invalidateDeviceObjects();
	/*
		Creates & setup handles, shaders, font texture
	*/
	bool createDeviceObjects();
	/*
		Create font texture by building a texture atlas and binding.
	*/
	bool createFontsTexture();

	// GLFW callbacks (installed by default if you enable 'install_callbacks' during initialization)
	// Provided here if you want to chain callbacks.
	// You can also handle inputs yourself and use those as a reference.
	void        imGuiMouseButtonCallback(shared_ptr<Window> window, int button, int action, int mods);
	void        imGuiScrollCallback(shared_ptr<Window>, double xoffset, double yoffset);
	void        imGuiKeyCallback(shared_ptr<Window>, int key, int scancode, int action, int mods);
	void        imGuiCharCallback(shared_ptr<Window>, unsigned int c);

};
#endif // ! IMGUIGLFWHANDER_H

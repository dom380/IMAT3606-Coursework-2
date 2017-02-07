#ifndef  IMGUIGLFWHANDER_H
#define  IMGUIGLFWHANDER_H

/*
	An IMGUI handler for GLFW based GL program.
	TODO: Base handler for IMGUI.
*/

struct GLFWwindow;
class ImguiGLFWHandler
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
	bool init(GLFWwindow* window, bool install_callbacks);
	/*
		Calls invalidateDeviceObjects & Base ImGui Shutdown
	*/
	void shutdown();
	/*
		Setup GLFW specific inputs, window size. 
		Then calls Base NewFrame.
		TODO: Can incorporate this with engine side inputs.
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

};
#endif // ! IMGUIGLFWHANDER_H

#ifndef  IMGUIGLFWHANDER_H
#define  IMGUIGLFWHANDER_H

/*
	An IMGUI handler for GLFW based GL program.
	TODO: Base handler for IMGUI for multiple gl programs.
*/
#include <memory>
using std::shared_ptr;

struct GLFWwindow;
class ImguiGLFWHandler
{
private:
	ImguiGLFWHandler() {};
	ImguiGLFWHandler(ImguiGLFWHandler const&) {}; // prevent copies
	void operator=(ImguiGLFWHandler const&) {}; // prevent assignments
	/*
		Copy and Paste?
	*/
	static const char*	GetClipboardText(void* user_data);
	static void			SetClipboardText(void* user_data, const char* text);

	static shared_ptr<ImguiGLFWHandler> instance;
	static bool initialised;
public:
	/*
	Returns the AssetManager instance or creates one if it doesn't exist.
	*/
	static shared_ptr<ImguiGLFWHandler> getInstance();
	/*
		Setup Callbacks & Key Mapping
	*/
	bool init(GLFWwindow* window);
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

	// GLFW callbacks (installed by default if you enable 'install_callbacks' during initialization)
	// Provided here if you want to chain callbacks.
	// You can also handle inputs yourself and use those as a reference.
	void        ImGui_ImplGlfwGL3_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	void        ImGui_ImplGlfwGL3_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void        ImGui_ImplGlfwGL3_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void        ImGui_ImplGlfwGL3_CharCallback(GLFWwindow* window, unsigned int c);

};
#endif // ! IMGUIGLFWHANDER_H

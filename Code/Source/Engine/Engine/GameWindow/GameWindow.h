#pragma once

struct GLFWwindow;

class Device;

class GameWindow
{
public:
	GameWindow();
	~GameWindow();
	GameWindow(const GameWindow&) = delete;

	void Initialize();

	void GetCursorPosition(double& x, double& y);
	void GetWindowSize(uint32_t& width, uint32_t& height);

	const char** GetRequiredExtensions(uint32_t& count);

	inline VkSurfaceKHR GetSurface() { return mySurface; }
	inline VkSurfaceFormatKHR GetSurfaceFormat() { return mySurfaceFormat; }
	inline VkSurfaceCapabilitiesKHR GetSurfaceCapabilities() { return mySurfaceCapabilities; }

	void SetVkInstance(VkInstance vkInstance) { myVkInstance = vkInstance; }

	VkSurfaceKHR CreateSurface(Device& vkDevice);
	void DestroySurface();

	// Returns true if window is closed
	bool PollEvents();

	GameWindow& operator=(const GameWindow&) = delete;

private:
	static void OnWindowResized(GLFWwindow* window, int width, int height);

	GLFWwindow* myWindow;
	VkInstance myVkInstance;
	VkSurfaceKHR mySurface;
	VkSurfaceCapabilitiesKHR mySurfaceCapabilities;
	VkSurfaceFormatKHR mySurfaceFormat;
};

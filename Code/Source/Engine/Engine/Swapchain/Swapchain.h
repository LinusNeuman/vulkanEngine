#pragma once

class GameWindow;
class Texture;

class Swapchain
{
public:
	Swapchain(GameWindow & gameWindow, uint32_t bufferCount, bool vsync);
	~Swapchain();

	inline uint32_t GetSwapchainImageCount() { return mySwapchainImageCount; }
	inline Texture* GetTextureAtIndex(const uint32_t index) { return myTextures[index]; }

	operator VkSwapchainKHR() { return mySwapchain; }

private:
	void Initialize();

	VkSwapchainKHR mySwapchain;
	GameWindow* myGameWindow;
	Texture** myTextures;
	uint32_t mySwapchainImageCount;
	bool myVsync;
};

#pragma once

class Device;
class GPU;
class GameWindow;
class Swapchain;
class Texture;
class RenderPass;
class Framebuffer;
class Queue;
class CommandPool;
class CommandBuffer;

class Engine
{
public:
	void Run();

	void SetWidth(const uint32_t width) { myWidth = width; }
	void SetHeight(const uint32_t height) { myHeight = height; }

	inline const uint32_t GetWidth() const { return myWidth; }
	inline const uint32_t GetHeight() const { return myHeight; }

	void SetApplicationName(const char* name)
	{
		myApplicationName = name;
	}

	void ResizeSwapchain();
	void Shutdown();

	//Vk
	inline VkInstance GetVkInstance() { return myVkInstance; }
	inline Device* GetMainDevice() { return myDevice; }

	//Singleton
	static Engine* const GetInstance()
	{
		if (myInstance == nullptr)
		{
			myInstance = new Engine();
		}

		return myInstance;
	}
	static void DestroyInstance()
	{
		delete myInstance;
		myInstance = nullptr;
	}

private:
	Engine();
	~Engine();
	Engine(const Engine& engine) = delete;
	Engine& operator=(Engine& engine) = delete;

	static Engine* myInstance;

	void InitializeVulkan();
	void DestroyVulkan();

	void CreateSwapchain();
	void DestroySwapchain();

	void SetupVulkanDebug();
	void DestroyVulkanDebug();

	bool BeginRender();
	bool EndRender();

	VkPhysicalDevice GetBestGPU();

	VkDebugReportCallbackCreateInfoEXT debug_create_report_info = { };
	VkDebugReportCallbackEXT myDebugReport;

	VkInstance myVkInstance;
	GPU* myGPU;
	Device* myDevice;
	Swapchain* mySwapchain;
	Queue* myQueue;

	CommandPool* myCommandPool;
	CommandBuffer* myCommandBuffer;

	uint32_t myActiveSwapchainIndex;
	VkFence mySwapchainAquireFence;

	VkSemaphore mySemaphore;

	std::vector<Texture*> myDepthStencilTextures;
	std::vector<RenderPass*> myRenderPasses;
	std::vector<Framebuffer*> myFramebuffers;

	GameWindow* myGameWindow;

	uint32_t myWidth;
	uint32_t myHeight;

	const char* myApplicationName;

	bool myIsRunning;
};

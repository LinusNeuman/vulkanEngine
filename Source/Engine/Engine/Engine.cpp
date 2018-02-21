#include "stdafx.h"
#include "Engine.h"

#include "Device\Device.h"
#include "GPU\GPU.h"
#include "CommandPool\CommandPool.h"
#include "CommandBuffer\CommandBuffer.h"
#include "Memory\Memory.h"
#include "GameWindow\GameWindow.h"
#include "Swapchain\Swapchain.h"
#include "Texture\Texture.h"
#include "RenderPass\RenderPass.h"
#include "Framebuffer\Framebuffer.h"
#include "Queue\Queue.h"
#include "CommandPool\CommandPool.h"

#include <iostream>
#include <sstream>
#include <array>
#include "Shader\Shader.h"

#ifdef _WIN32
#include <Windows.h>
#endif

Engine* Engine::myInstance = nullptr;

#ifndef RELEASE
std::array<const char*, 2> INSTANCE_LAYERS = { "VK_LAYER_LUNARG_standard_validation", "VK_LAYER_RENDERDOC_Capture" };
std::array<const char*, 1> INSTANCE_EXTENSIONS = { VK_EXT_DEBUG_REPORT_EXTENSION_NAME };
std::array<const char*, 0> DEVICE_EXTENSIONS = {};
#else
std::array<const char*, 0> INSTANCE_LAYERS = {};
std::array<const char*, 0> INSTANCE_EXTENSIONS = {};
std::array<const char*, 0> DEVICE_EXTENSIONS = {};
#endif

Engine::Engine()
{
	InitializeMemory(512);

	myIsRunning = false;

	myWidth = 800;
	myHeight = 600;

	myVkInstance = VK_NULL_HANDLE;
	myDebugReport = VK_NULL_HANDLE;
	myGPU = nullptr;
	myDevice = nullptr;
	myGameWindow = nullptr;
	mySwapchain = nullptr;

	PrintMessage("Engine");
	ShiftIndentLevel(1);

	Shader shader("Shaders/test.vert");
}

Engine::~Engine()
{
	DestroyMemory();
}


bool Engine::BeginRender()
{
	VkResult vkResult = vkAcquireNextImageKHR(*myDevice, *mySwapchain, UINT64_MAX, VK_NULL_HANDLE, mySwapchainAquireFence, &myActiveSwapchainIndex);
	if (vkResult == VK_ERROR_OUT_OF_DATE_KHR)
	{
		ResizeSwapchain();
		return true;
	}

	ErrorCheck(vkWaitForFences(*myDevice, 1, &mySwapchainAquireFence, true, UINT64_MAX));
	ErrorCheck(vkResetFences(*myDevice, 1, &mySwapchainAquireFence));
	ErrorCheck(vkQueueWaitIdle(*myQueue));

	return false;
}

bool Engine::EndRender()
{
	VkResult results = VK_SUCCESS;

	VkSwapchainKHR swapchain = *mySwapchain;
	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = &mySemaphore;
	present_info.swapchainCount = 1;
	present_info.pSwapchains = &swapchain;
	present_info.pImageIndices = &myActiveSwapchainIndex;
	present_info.pResults = &results;

	VkResult vkResult = vkQueuePresentKHR(*myQueue, &present_info);
	if (vkResult == VK_ERROR_OUT_OF_DATE_KHR)
	{
		ResizeSwapchain();
		return true;
	}

	ErrorCheck(results);

	return false;
}

void Engine::ResizeSwapchain()
{
	DestroySwapchain();
	CreateSwapchain();
}

void Engine::Run()
{
	if (myIsRunning == true)
	{
		return;
	}

	myIsRunning = true;

	//Initialize
	InitializeVulkan();

	//Run game loop
	while (myIsRunning == true)
	{
		if (myGameWindow->PollEvents() == true)
		{
			Shutdown();
		}
		else
		{
			if (myWidth == 0 || myHeight == 0)
			{
				myGameWindow->GetWindowSize(myWidth, myHeight);
				if (myWidth > 0 && myHeight > 0)
				{
					CreateSwapchain();
				}
				continue;
			}
			if (BeginRender() == true)
			{
				continue;
			}

			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			vkBeginCommandBuffer(*myCommandBuffer, &begin_info);

			VkRect2D render_area = {};
			render_area.offset.x = 0;
			render_area.offset.y = 0;
			render_area.extent.width = myWidth;
			render_area.extent.height = myHeight;

			double x;
			double y;
			myGameWindow->GetCursorPosition(x, y);

			std::array<VkClearValue, 2> clear_values = {};
			clear_values[0].color.float32[0] = static_cast<float>(x / myWidth);
			clear_values[0].color.float32[1] = static_cast<float>(y / myHeight);
			clear_values[0].color.float32[2] = 0.0f;
			clear_values[0].color.float32[3] = 1.0f;
			clear_values[1].depthStencil.depth = 0.0f;
			clear_values[1].depthStencil.stencil = 0;

			VkViewport viewport = {};
			viewport.x = 0;
			viewport.y = 0;
			viewport.width = static_cast<float>(myWidth);
			viewport.height = static_cast<float>(myHeight);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			vkCmdSetViewport(*myCommandBuffer, 0, 1, &viewport);

			VkRenderPassBeginInfo render_pass_begin_info = {};
			render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			render_pass_begin_info.renderPass = *myRenderPasses[myActiveSwapchainIndex];
			render_pass_begin_info.framebuffer = *myFramebuffers[myActiveSwapchainIndex];
			render_pass_begin_info.renderArea = render_area;
			render_pass_begin_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
			render_pass_begin_info.pClearValues = clear_values.data();

			vkCmdBeginRenderPass(*myCommandBuffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdEndRenderPass(*myCommandBuffer);

			vkEndCommandBuffer(*myCommandBuffer);

			VkCommandBuffer commandBuffer = myCommandBuffer->GetVkCommandBuffer();

			VkSubmitInfo submit_info = {};
			submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submit_info.commandBufferCount = 1;
			submit_info.pCommandBuffers = &commandBuffer;
			submit_info.signalSemaphoreCount = 1;
			submit_info.pSignalSemaphores = &mySemaphore;

			vkQueueSubmit(*myQueue, 1, &submit_info, VK_NULL_HANDLE);

			EndRender();
		}
	}

	vkDeviceWaitIdle(*myDevice);

	//Destroy
	DestroyVulkan();
}

VKAPI_ATTR VkBool32 VKAPI_CALL
DebugReportCallback(
	VkDebugReportFlagsEXT msg_flags,
	VkDebugReportObjectTypeEXT /*obj_type*/,
	uint64_t /*src_obj*/,
	size_t /*location*/,
	int32_t /*msg_code*/,
	const char* layer_prefix,
	const char* msg,
	void* /*user_data*/
)
{
	std::ostringstream stream;

	stream << "Vulkan Message: ";
	switch (msg_flags)
	{
	case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
		stream << "DEBUG: ";
		break;
	case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
		stream << "INFO: ";
		break;
	case VK_DEBUG_REPORT_WARNING_BIT_EXT:
		stream << "WARNING: ";
		break;
	case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
		stream << "PERFORMANCE WARNING: ";
		break;
	case VK_DEBUG_REPORT_ERROR_BIT_EXT:
		stream << "ERROR: ";
		break;
	}
	stream << "Layer: " << layer_prefix << " Message: " << msg << std::endl << std::endl;
	switch (msg_flags)
	{
	case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
		PrintMessage(stream.str().c_str());
		break;
	case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
		PrintMessage(stream.str().c_str());
		break;
	case VK_DEBUG_REPORT_WARNING_BIT_EXT:
		PrintWarningMessage(stream.str().c_str());
		break;
	case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
		PrintWarningMessage(stream.str().c_str());
		break;
	case VK_DEBUG_REPORT_ERROR_BIT_EXT:
		PrintErrorMessage(stream.str().c_str());
		break;
	}

#ifdef _WIN32
	if (msg_flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
	{
		std::string txt = stream.str();
		std::wstring wtxt(txt.begin(), txt.end());
		MessageBox(NULL, wtxt.c_str(), L"Vulkan Error!", 0);
	}
#endif

	return false;
}

void Engine::InitializeVulkan()
{
	// Window
	PrintMessageGreen("Initialize GLFW Window");
	myGameWindow = new GameWindow();
	myGameWindow->Initialize();

	// Initialize vulkan
	VkApplicationInfo application_info = {};
	application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

	application_info.apiVersion = VK_MAKE_VERSION(1, 0, 3);
	application_info.pApplicationName = myApplicationName;
	application_info.pEngineName = ENGINE_NAME;
	application_info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
	application_info.applicationVersion = VK_MAKE_VERSION(0, 1, 0);

	debug_create_report_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	debug_create_report_info.pfnCallback = DebugReportCallback;
	debug_create_report_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
		VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
		VK_DEBUG_REPORT_WARNING_BIT_EXT;

	uint32_t count = 0;
	const char** extensionsRequired = myGameWindow->GetRequiredExtensions(count);

	std::vector<const char*> extensions;

	for (uint32_t i = 0; i < INSTANCE_EXTENSIONS.size(); ++i)
	{
		extensions.push_back(INSTANCE_EXTENSIONS[i]);
	}

	for (uint32_t i = 0; i < count; ++i)
	{
		extensions.push_back(extensionsRequired[i]);
	}

	VkInstanceCreateInfo instance_create_info = {};
	instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_create_info.pApplicationInfo = &application_info;
	instance_create_info.enabledLayerCount = static_cast<uint32_t>(INSTANCE_LAYERS.size());
	instance_create_info.ppEnabledLayerNames = INSTANCE_LAYERS.data();
	instance_create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	instance_create_info.ppEnabledExtensionNames = extensions.data();
	instance_create_info.pNext = &debug_create_report_info;

	PrintMessageGreen("Initialize Vulkan API");
	ShiftIndentLevel(1);

	ErrorCheck(vkCreateInstance(&instance_create_info, VK_ALLOCATOR, &myVkInstance));

	myGameWindow->SetVkInstance(myVkInstance);

	SetupVulkanDebug();

	// Initialize Device, no multi device yet
	VkPhysicalDevice physicalDevice = GetBestGPU();

	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);

	double vram = 0; // In GB
	double ram = 0; // In GB
	for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryHeapCount; ++i)
	{
		if (physicalDeviceMemoryProperties.memoryHeaps[i].flags == VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
		{
			vram = static_cast<double>(physicalDeviceMemoryProperties.memoryHeaps[i].size) / (1024.0 * 1024.0 * 1024.0);
		}
		else
		{
			ram = static_cast<double>(physicalDeviceMemoryProperties.memoryHeaps[i].size) / (1024.0 * 1024.0 * 1024.0);
		}
	}

	PrintMessageGreen("GPU: " + std::string(physicalDeviceProperties.deviceName));
	PrintMessageGreen("VRAM: " + std::to_string(vram) + " GB");
	PrintMessageGreen("RAM: " + std::to_string(ram) + " GB");

	myGPU = new GPU(physicalDevice);

	PrintMessageGreen("Initialize Device");
	myDevice = new Device(*myGPU, false, VK_QUEUE_GRAPHICS_BIT);

	PrintMessageGreen("Initialize Window Surface");

	PrintMessageGreen("Initialize Swapchain");
	PrintMessageGreen("Images Count: 2", 1);
	CreateSwapchain();

	VkFenceCreateInfo fence_create_info = {};
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	ErrorCheck(vkCreateFence(*myDevice, &fence_create_info, VK_ALLOCATOR, &mySwapchainAquireFence));

	VkSemaphoreCreateInfo semaphore_create_info = {};
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	ErrorCheck(vkCreateSemaphore(*myDevice, &semaphore_create_info, VK_ALLOCATOR, &mySemaphore));

	myCommandPool = new CommandPool(*myDevice, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
	myCommandBuffer = new CommandBuffer(myCommandPool, true);

	myQueue = new Queue();
	myQueue->Initialize();

	ShiftIndentLevel(-2);
	PrintMessage("Game");
	ShiftIndentLevel(1);
}

void Engine::CreateSwapchain()
{
	if (myWidth == 0 || myHeight == 0)
	{
		return;
	}

	myGameWindow->CreateSurface(*myDevice);

	mySwapchain = new Swapchain(*myGameWindow, 2, true);

	uint32_t swapchainImageCount = mySwapchain->GetSwapchainImageCount();

	for (uint32_t i = 0; i < swapchainImageCount; ++i)
	{
		Texture* myDepthStencilTexture = new Texture(*myDevice);
		myDepthStencilTexture->SetWidth(myWidth);
		myDepthStencilTexture->SetHeight(myHeight);
		myDepthStencilTexture->SetImageUsageFlags(VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
		myDepthStencilTexture->SetImageType(VkImageType::VK_IMAGE_TYPE_2D);
		myDepthStencilTexture->SetImageViewType(VkImageViewType::VK_IMAGE_VIEW_TYPE_2D);

		std::array<VkFormat, 3> depthStencilFormats;
		depthStencilFormats[0] = VK_FORMAT_D32_SFLOAT_S8_UINT;
		depthStencilFormats[1] = VK_FORMAT_D24_UNORM_S8_UINT;
		depthStencilFormats[2] = VK_FORMAT_D16_UNORM_S8_UINT;

		std::array<VkFormat, 2> depthFormats;
		depthFormats[0] = VK_FORMAT_D32_SFLOAT;
		depthFormats[1] = VK_FORMAT_D16_UNORM;

		VkFormat selectedFormat = VK_FORMAT_UNDEFINED;
		bool stencilAvailable = true;
		for (size_t j = 0; j < depthStencilFormats.size(); ++j)
		{
			if (myDepthStencilTexture->TryFormat(depthStencilFormats[j], VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == true)
			{
				selectedFormat = depthStencilFormats[j];
				break;
			}
		}
		if (selectedFormat == VK_FORMAT_UNDEFINED)
		{
			stencilAvailable = false;
			for (size_t j = 0; j < depthFormats.size(); ++j)
			{
				if (myDepthStencilTexture->TryFormat(depthFormats[j], VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == true)
				{
					selectedFormat = depthFormats[j];
					break;
				}
			}
		}

		assert(selectedFormat != VK_FORMAT_UNDEFINED && "Undefined format for depth stencil texture");

		VkImageAspectFlagBits imageAspectFlagBits = static_cast<VkImageAspectFlagBits>(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
		if (stencilAvailable == false)
		{
			imageAspectFlagBits = VK_IMAGE_ASPECT_DEPTH_BIT;
		}

		myDepthStencilTexture->SetImageAspectMask(imageAspectFlagBits);
		myDepthStencilTexture->SetFormat(selectedFormat);

		myDepthStencilTexture->Initialize();

		myDepthStencilTextures.push_back(myDepthStencilTexture);
	}

	myRenderPasses.reserve(swapchainImageCount);

	for (uint32_t i = 0; i < swapchainImageCount; ++i)
	{
		std::vector<Texture*> textures(2);
		textures[0] = mySwapchain->GetTextureAtIndex(i);
		textures[1] = myDepthStencilTextures[i];

		RenderPass* renderPass = new RenderPass(*myDevice);
		renderPass->SetAttachments(textures);
		renderPass->Initialize();

		myRenderPasses.push_back(renderPass);
		myFramebuffers.push_back(new Framebuffer(*myDevice, *renderPass));
	}
}

void Engine::DestroySwapchain()
{
	vkDeviceWaitIdle(*myDevice);

	myGameWindow->GetWindowSize(myWidth, myHeight);

	for (uint32_t i = 0; i < myRenderPasses.size(); ++i)
	{
		delete myRenderPasses[i];
		delete myFramebuffers[i];
		delete myDepthStencilTextures[i];
	}

	myDepthStencilTextures.clear();
	myRenderPasses.clear();
	myFramebuffers.clear();

	delete mySwapchain;
	mySwapchain = nullptr;

	myGameWindow->DestroySurface();
}

void Engine::DestroyVulkan()
{
	vkDestroySemaphore(*myDevice, mySemaphore, VK_ALLOCATOR);
	mySemaphore = VK_NULL_HANDLE;

	delete myCommandBuffer;
	myCommandBuffer = nullptr;

	delete myCommandPool;
	myCommandPool = nullptr;

	vkDestroyFence(*myDevice, mySwapchainAquireFence, VK_ALLOCATOR);
	mySwapchainAquireFence = VK_NULL_HANDLE;

	for (uint32_t i = 0; i < myRenderPasses.size(); ++i)
	{
		delete myRenderPasses[i];
	}
	myRenderPasses.clear();

	for (uint32_t i = 0; i < myFramebuffers.size(); ++i)
	{
		delete myFramebuffers[i];
	}
	myFramebuffers.clear();

	for (uint32_t i = 0; i < myDepthStencilTextures.size(); ++i)
	{
		delete myDepthStencilTextures[i];
	}
	myDepthStencilTextures.clear();

	myDepthStencilTextures.~vector();
	myRenderPasses.~vector();
	myFramebuffers.~vector();

	delete myQueue;
	myQueue = nullptr;

	delete mySwapchain;
	mySwapchain = nullptr;

	delete myDevice;
	myDevice = nullptr;

	delete myGPU;
	myGPU = nullptr;

	DestroyVulkanDebug();

	myGameWindow->DestroySurface();

	delete myGameWindow;
	myGameWindow = nullptr;

	// Destroy instance
	vkDestroyInstance(myVkInstance, VK_ALLOCATOR);
	myVkInstance = nullptr;

	PrintMessage("", -1);
}

PFN_vkCreateDebugReportCallbackEXT fvkCreateDebugReportCallbackEXT = nullptr;
PFN_vkDestroyDebugReportCallbackEXT fvkDestroyDebugReportCallbackEXT = nullptr;

void Engine::SetupVulkanDebug()
{
#ifndef RELEASE
	uint32_t layer_count = 0;
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

	VkLayerProperties* layer_properties = new VkLayerProperties[layer_count];
	vkEnumerateInstanceLayerProperties(&layer_count, layer_properties);

	/*PrintMessage("Instance Layers:");
	ShiftIndentLevel(1);

	for (uint32_t i = 0; i < layer_count; ++i)
	{
		PrintMessage(layer_properties[i].layerName);
	}

	PrintMessage("", -1);*/

	delete[] layer_properties;
	layer_properties = nullptr;

	fvkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(myVkInstance, "vkCreateDebugReportCallbackEXT");
	fvkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(myVkInstance, "vkDestroyDebugReportCallbackEXT");
	if (fvkCreateDebugReportCallbackEXT == nullptr || fvkDestroyDebugReportCallbackEXT == nullptr)
	{
		assert(false && "Debug classes are nullptr");
		std::exit(-1);
	}

	PrintMessage("Debug: Enabled");
	fvkCreateDebugReportCallbackEXT(myVkInstance, &debug_create_report_info, VK_ALLOCATOR, &myDebugReport);
#else
	PrintMessage("Debug: Disabled");
#endif
}

void Engine::DestroyVulkanDebug()
{
#ifndef RELEASE
	fvkDestroyDebugReportCallbackEXT(myVkInstance, myDebugReport, VK_ALLOCATOR);
	myDebugReport = VK_NULL_HANDLE;
#endif
}

VkPhysicalDevice Engine::GetBestGPU()
{
	uint32_t gpu_count = 0;
	vkEnumeratePhysicalDevices(myVkInstance, &gpu_count, nullptr);

	VkPhysicalDevice* physicalDevices = new VkPhysicalDevice[gpu_count];
	vkEnumeratePhysicalDevices(myVkInstance, &gpu_count, physicalDevices);

	uint32_t gpu_index = UINT32_MAX;
	for (uint32_t i = 0; i < gpu_count; ++i)
	{
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalDeviceProperties);

		if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			gpu_index = i;
			break;
		}
	}
	if (gpu_index == UINT32_MAX)
	{
		gpu_index = 0;
	}

	VkPhysicalDevice physicalDevice = physicalDevices[gpu_index];

	delete[] physicalDevices;
	physicalDevices = nullptr;

	return physicalDevice;
}

void Engine::Shutdown()
{
	myIsRunning = false;
}

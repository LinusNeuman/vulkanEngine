#include "stdafx.h"
#include "Swapchain.h"
#include "..\GameWindow\GameWindow.h"
#include "..\Device\Device.h"
#include "..\GPU\GPU.h"
#include "..\Texture\Texture.h"
#include "Engine\Engine.h"

Swapchain::Swapchain(GameWindow& gameWindow, uint32_t bufferCount, bool vsync)
	: myGameWindow(&gameWindow),
	mySwapchainImageCount(bufferCount),
	myVsync(vsync)
{
	mySwapchain = VK_NULL_HANDLE;

	Initialize();
}

Swapchain::~Swapchain()
{
	for (uint32_t i = 0; i < mySwapchainImageCount; ++i)
	{
		myTextures[i]->SetImage(nullptr);
		delete myTextures[i];
	}
	delete[] myTextures;

	vkDestroySwapchainKHR(Engine::GetInstance()->GetMainDevice()->GetVkDevice(), mySwapchain, VK_ALLOCATOR);
}

void Swapchain::Initialize()
{
	Engine* engine = Engine::GetInstance();
	Device& device = *engine->GetMainDevice();

	VkSurfaceCapabilitiesKHR surfaceCapabilities = myGameWindow->GetSurfaceCapabilities();

	if (surfaceCapabilities.maxImageCount > 0)
	{
		if (mySwapchainImageCount > surfaceCapabilities.maxImageCount)
			mySwapchainImageCount = surfaceCapabilities.maxImageCount;
	}

	if (mySwapchainImageCount < surfaceCapabilities.minImageCount) mySwapchainImageCount = surfaceCapabilities.minImageCount;

	VkPhysicalDevice physicalDevice = engine->GetMainDevice()->GetGPU().GetPhysicalDevice();

	VkPresentModeKHR present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
	if (myVsync == true)
	{
		present_mode = VK_PRESENT_MODE_FIFO_KHR;
		uint32_t present_mode_count = 0;
		ErrorCheck(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, myGameWindow->GetSurface(), &present_mode_count, nullptr));
		std::vector<VkPresentModeKHR> present_modes(present_mode_count);
		ErrorCheck(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, myGameWindow->GetSurface(), &present_mode_count, present_modes.data()));

		for (uint32_t i = 0; i < present_mode_count; ++i)
		{
			if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				present_mode = present_modes[i];
				break;
			}
		}
	}

	VkSwapchainCreateInfoKHR swapchain_create_info = {};
	swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_create_info.surface = myGameWindow->GetSurface();
	swapchain_create_info.minImageCount = mySwapchainImageCount;
	swapchain_create_info.imageFormat = myGameWindow->GetSurfaceFormat().format;
	swapchain_create_info.imageColorSpace = myGameWindow->GetSurfaceFormat().colorSpace;
	swapchain_create_info.imageExtent.width = engine->GetWidth();
	swapchain_create_info.imageExtent.height = engine->GetHeight();
	swapchain_create_info.imageArrayLayers = 1;
	swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchain_create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchain_create_info.presentMode = present_mode;
	swapchain_create_info.clipped = VK_TRUE;
	swapchain_create_info.oldSwapchain = mySwapchain;

	ErrorCheck(vkCreateSwapchainKHR(device, &swapchain_create_info, VK_ALLOCATOR, &mySwapchain));

	ErrorCheck(vkGetSwapchainImagesKHR(device, mySwapchain, &mySwapchainImageCount, nullptr));
	VkImage* images = new VkImage[mySwapchainImageCount];
	ErrorCheck(vkGetSwapchainImagesKHR(device, mySwapchain, &mySwapchainImageCount, images));

	myTextures = new Texture*[mySwapchainImageCount];
	for (uint32_t i = 0; i < mySwapchainImageCount; ++i)
	{
		myTextures[i] = new Texture(device);
		myTextures[i]->SetFormat(swapchain_create_info.imageFormat);
		myTextures[i]->SetWidth(swapchain_create_info.imageExtent.width);
		myTextures[i]->SetHeight(swapchain_create_info.imageExtent.height);
		myTextures[i]->SetImage(images[i]);

		myTextures[i]->Initialize();
	}

	delete[] images;
	images = nullptr;
}

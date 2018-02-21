#include "stdafx.h"
#include "GameWindow.h"
#include "glfw\glfw3.h"
#include "..\Device\Device.h"
#include "..\GPU\GPU.h"
#include "..\Engine.h"

GameWindow::GameWindow()
{
	myWindow = nullptr;
	mySurface = VK_NULL_HANDLE;
	myVkInstance = VK_NULL_HANDLE;
}

GameWindow::~GameWindow()
{
	glfwDestroyWindow(myWindow);
	glfwTerminate();
}

void GameWindow::Initialize()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	Engine* engine = Engine::GetInstance();

	myWindow = glfwCreateWindow(engine->GetWidth(), engine->GetHeight(), ENGINE_NAME, nullptr, nullptr);
}

void GameWindow::GetCursorPosition(double& x, double& y)
{
	glfwGetCursorPos(myWindow, &x, &y);
}

void GameWindow::GetWindowSize(uint32_t& width, uint32_t& height)
{
	int _width, _height;
	glfwGetWindowSize(myWindow, &_width, &_height);

	width = static_cast<uint32_t>(_width);
	height = static_cast<uint32_t>(_height);
}

const char** GameWindow::GetRequiredExtensions(uint32_t& count)
{
	return glfwGetRequiredInstanceExtensions(&count);
}

VkSurfaceKHR GameWindow::CreateSurface(Device& device)
{
	ErrorCheck(glfwCreateWindowSurface(myVkInstance, myWindow, VK_ALLOCATOR, &mySurface));

	VkPhysicalDevice physicalDevice = device.GetGPU().GetPhysicalDevice();
	VkBool32 supported = VK_FALSE;
	vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, device.GetQueueFamilyIndex(), mySurface, &supported);
	if (supported == VK_FALSE)
	{
		assert(false && "WSI not supported");
		std::exit(-1);
	}

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, mySurface, &mySurfaceCapabilities);

	uint32_t format_count = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, mySurface, &format_count, nullptr);
	if (format_count == 0)
	{
		assert(false && "Surface formats missing");
		std::exit(-1);
	}
	std::vector<VkSurfaceFormatKHR> formats(format_count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, mySurface, &format_count, formats.data());
	if (formats[0].format == VK_FORMAT_UNDEFINED)
	{
		mySurfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
		mySurfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	}
	else
	{
		mySurfaceFormat = formats[0];
	}

	return mySurface;
}

void GameWindow::DestroySurface()
{
	if (mySurface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(myVkInstance, mySurface, VK_ALLOCATOR);
		mySurface = VK_NULL_HANDLE;
	}
}

bool GameWindow::PollEvents()
{
	if (glfwWindowShouldClose(myWindow))
	{
		return true;
	}

	glfwPollEvents();

	return false;
}

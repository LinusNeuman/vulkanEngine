#include "stdafx.h"
#include "Device.h"

#include "Engine\GPU\GPU.h"
#include "Engine\Engine.h"

#include <stdio.h>
#include <stdarg.h>

Device::Device(GPU& gpu, bool exclusiveSupportedFlags, VkQueueFlags supportedFlags)
{
	myVkInstanceReference = Engine::GetInstance()->GetVkInstance();
	myGPU = &gpu;
	myVkDevice = VK_NULL_HANDLE;

	myQueueFamilyIndex = GetBestQueueFamilyIndex(*myGPU, exclusiveSupportedFlags, supportedFlags);

	Initialize();
}

Device::Device(GPU& gpu, bool exclusiveSupportedFlags, int supportedFlags) : Device(gpu, exclusiveSupportedFlags, static_cast<VkQueueFlags>(supportedFlags)) { }

Device::~Device()
{
	if (myVkDevice != VK_NULL_HANDLE)
	{
		vkDestroyDevice(myVkDevice, VK_ALLOCATOR);
		myVkDevice = VK_NULL_HANDLE;
	}
}

uint32_t Device::GetBestQueueFamilyIndex(GPU& gpu, bool exclusiveSupportedFlags, VkQueueFlags queueFlagBits)
{
	VkPhysicalDevice physicalDevice = gpu.GetPhysicalDevice();

	uint32_t family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &family_count, nullptr);

	VkQueueFamilyProperties* queueFamilyProperties = new VkQueueFamilyProperties[family_count];
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &family_count, queueFamilyProperties);

	uint32_t queueFamilyIndex = 0;
	bool found = false;

	for (uint32_t i = 0; i < family_count; ++i)
	{
		VkQueueFlags bits = queueFamilyProperties[i].queueFlags & queueFlagBits;

		if (bits > 0)
		{
			found = true;

			if (exclusiveSupportedFlags == true)
			{
				if (queueFamilyProperties[i].queueFlags != queueFlagBits)
				{
					found = false;
				}
			}

			if (found == true)
			{
				queueFamilyIndex = i;
				break;
			}
		}
	}

	if (found == false)
	{
		for (uint32_t i = 0; i < family_count; ++i)
		{
			VkQueueFlags bits = queueFamilyProperties[i].queueFlags & queueFlagBits;

			if (bits == queueFlagBits)
			{
				found = true;
				queueFamilyIndex = i;
				break;
			}
		}
	}

	if (found == false)
	{
		assert(false && "Vulkan error: No available queue that supports graphics on the gpu");
		std::exit(-1);
	}

	delete[] queueFamilyProperties;
	queueFamilyProperties = nullptr;

	return queueFamilyIndex;
}

void Device::Initialize()
{
	float queuePriorites = 0.0f;

	VkDeviceQueueCreateInfo device_queue_create_info = {};
	device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	device_queue_create_info.pQueuePriorities = &queuePriorites;
	device_queue_create_info.queueCount = 1;
	device_queue_create_info.queueFamilyIndex = myQueueFamilyIndex;

	std::vector<const char*> extensions;
	extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	VkDeviceCreateInfo device_create_info = {};
	device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.queueCreateInfoCount = 1;
	device_create_info.pQueueCreateInfos = &device_queue_create_info;
	device_create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	device_create_info.ppEnabledExtensionNames = extensions.data();

	ErrorCheck(vkCreateDevice(*myGPU, &device_create_info, VK_ALLOCATOR, &myVkDevice));

	vkGetPhysicalDeviceProperties(*myGPU, &myPhysicalDeviceProperties);
	vkGetPhysicalDeviceMemoryProperties(*myGPU, &myPhysicalDeviceMemoryProperties);
}

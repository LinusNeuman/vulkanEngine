#include "stdafx.h"
#include "Texture.h"
#include "..\Device\Device.h"
#include "..\GPU\GPU.h"
#include "..\Engine.h"

Texture::Texture(Device& device)
{
	myDevice = &device;
	myFileName = "";

	myWidth = myHeight = 0;

	myImage = VK_NULL_HANDLE;
	myImageView = VK_NULL_HANDLE;
	myDeviceMemory = VK_NULL_HANDLE;

	myMultiSampleCount = VK_SAMPLE_COUNT_1_BIT;
	myImageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	myImageType = VK_IMAGE_TYPE_2D;
	myImageViewType = VK_IMAGE_VIEW_TYPE_2D;
	myImageAspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
}

Texture::Texture(const Texture& texture)
{
	myDevice = texture.myDevice;
	myWidth = texture.myWidth;
	myHeight = texture.myHeight;
	myFileName = texture.myFileName;
	myFormat = texture.myFormat;
	myImageUsage = texture.myImageUsage;
	myImageView = texture.myImageView;
	myImageType = texture.myImageType;
	myImageViewType = texture.myImageViewType;
	myImageAspectMask = texture.myImageAspectMask;
	myMultiSampleCount = texture.myMultiSampleCount;

	myImage = VK_NULL_HANDLE;
	myImageView = VK_NULL_HANDLE;
	myDeviceMemory = VK_NULL_HANDLE;

	Initialize();
}

Texture::~Texture()
{
	Destroy();
}

bool Texture::TryFormat(VkFormat format, VkFormatFeatureFlagBits feature)
{
	VkFormatProperties format_properties = {};
	vkGetPhysicalDeviceFormatProperties(myDevice->GetGPU(), format, &format_properties);

	if (static_cast<VkFormatFeatureFlagBits>(format_properties.optimalTilingFeatures & feature) == feature)
	{
		return true;
	}

	return false;
}

void Texture::Initialize()
{
	if (myImageView != VK_NULL_HANDLE)
	{
		Destroy();
	}

	if (myImage == VK_NULL_HANDLE)
	{
		VkImageCreateInfo image_create_info = {};
		image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_create_info.format = myFormat;
		image_create_info.arrayLayers = 1;
		image_create_info.extent.width = myWidth;
		image_create_info.extent.height = myHeight;
		image_create_info.extent.depth = 1;
		image_create_info.imageType = myImageType;
		image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_create_info.mipLevels = 1;
		image_create_info.samples = myMultiSampleCount;
		image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_create_info.usage = myImageUsage;

		ErrorCheck(vkCreateImage(*myDevice, &image_create_info, VK_ALLOCATOR, &myImage));

		VkMemoryRequirements memory_requirements;
		vkGetImageMemoryRequirements(*myDevice, myImage, &memory_requirements);

		uint32_t memory_index = UINT32_MAX;
		auto gpu_memory_properties = myDevice->GetVkPhysicalDeviceMemoryProperties();
		uint32_t required_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		for (uint32_t i = 0; i < gpu_memory_properties.memoryTypeCount; ++i)
		{
			if (memory_requirements.memoryTypeBits & (1 << i))
			{
				if ((gpu_memory_properties.memoryTypes[i].propertyFlags & required_properties) == required_properties)
				{
					memory_index = i;
					break;
				}
			}
		}

		assert(memory_index != UINT32_MAX && "Out of bounds");

		VkMemoryAllocateInfo allocate_info = {};
		allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocate_info.allocationSize = memory_requirements.size;
		allocate_info.memoryTypeIndex = memory_index;

		ErrorCheck(vkAllocateMemory(*myDevice, &allocate_info, VK_ALLOCATOR, &myDeviceMemory));

		ErrorCheck(vkBindImageMemory(*myDevice, myImage, myDeviceMemory, 0));
	}

	VkImageViewCreateInfo view_create_info = {};
	view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_create_info.viewType = myImageViewType;
	view_create_info.image = myImage;
	view_create_info.format = myFormat;
	view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_create_info.subresourceRange.aspectMask = myImageAspectMask;
	view_create_info.subresourceRange.baseArrayLayer = 0;
	view_create_info.subresourceRange.layerCount = 1;
	view_create_info.subresourceRange.baseMipLevel = 0;
	view_create_info.subresourceRange.levelCount = 1;

	ErrorCheck(vkCreateImageView(*myDevice, &view_create_info, VK_ALLOCATOR, &myImageView));
}

void Texture::Destroy()
{
	if (myImage != VK_NULL_HANDLE)
	{
		vkDestroyImage(*myDevice, myImage, VK_ALLOCATOR);
		myImage = VK_NULL_HANDLE;
	}

	if (myDeviceMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(*myDevice, myDeviceMemory, VK_ALLOCATOR);
		myDeviceMemory = VK_NULL_HANDLE;
	}

	if (myImageView != VK_NULL_HANDLE)
	{
		vkDestroyImageView(*myDevice, myImageView, VK_ALLOCATOR);
		myImageView = VK_NULL_HANDLE;
	}
}

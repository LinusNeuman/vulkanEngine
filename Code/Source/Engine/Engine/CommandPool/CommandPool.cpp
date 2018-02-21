#include "stdafx.h"
#include "CommandPool.h"
#include "Engine/Engine.h"
#include "../Device/Device.h"

CommandPool::CommandPool(Device& device, uint32_t createFlags)
	: myDevice(&device),
	myResetFlags(static_cast<VkCommandPoolCreateFlags>(createFlags))
{
	myCommandPool = VK_NULL_HANDLE;

	Initialize();
}

CommandPool::~CommandPool()
{
	vkDestroyCommandPool(*myDevice, myCommandPool, VK_ALLOCATOR);
}

void CommandPool::Initialize()
{
	VkCommandPoolCreateInfo pool_create_info = {};
	pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_create_info.queueFamilyIndex = myDevice->GetQueueFamilyIndex();
	pool_create_info.flags = myResetFlags;

	vkCreateCommandPool(*myDevice, &pool_create_info, VK_ALLOCATOR, &myCommandPool);
}

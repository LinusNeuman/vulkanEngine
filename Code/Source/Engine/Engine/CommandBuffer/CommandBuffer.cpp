#include "stdafx.h"
#include "CommandBuffer.h"
#include "../CommandPool/CommandPool.h"
#include "Engine/Engine.h"
#include "../Device/Device.h"
#include "../Queue/Queue.h"

CommandBuffer::CommandBuffer(CommandPool* commandPool, bool isPrimaryLevel)
	: myCommandPool(commandPool),
	myIsPrimaryLevel(isPrimaryLevel)
{
	myDevice = myCommandPool->GetDevice();

	Initialize();
}

CommandBuffer::~CommandBuffer()
{
	vkFreeCommandBuffers(*myDevice, *myCommandPool, 1, &myVkCommandBuffer);

	myVkCommandBuffer = VK_NULL_HANDLE;
}

void CommandBuffer::Initialize()
{
	VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
	command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.commandPool = *myCommandPool;
	command_buffer_allocate_info.commandBufferCount = 1;
	command_buffer_allocate_info.level = myIsPrimaryLevel == true ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;

	ErrorCheck(vkAllocateCommandBuffers(*myDevice, &command_buffer_allocate_info, &myVkCommandBuffer));
}

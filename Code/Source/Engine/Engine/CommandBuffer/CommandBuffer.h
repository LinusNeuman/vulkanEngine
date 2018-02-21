#pragma once

class CommandPool;
class Device;

class CommandBuffer
{
public:
	CommandBuffer(CommandPool * commandPool, bool isPrimaryLevel);
	~CommandBuffer();
	CommandBuffer(const CommandBuffer&) = delete;

	void Initialize();

	inline VkCommandBuffer GetVkCommandBuffer() { return myVkCommandBuffer; }

	operator VkCommandBuffer() { return myVkCommandBuffer; }

	CommandBuffer& operator=(const CommandBuffer&) = delete;

private:
	Device* myDevice;
	CommandPool* myCommandPool;
	VkCommandBuffer myVkCommandBuffer;

	bool myIsPrimaryLevel;
};

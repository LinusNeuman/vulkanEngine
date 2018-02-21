#pragma once

class Device;

class CommandPool
{
public:
	CommandPool(Device& device, uint32_t resetFlags);
	CommandPool(const Device&) = delete;
	~CommandPool();

	inline VkCommandPool GetVkCommandPool() { return myCommandPool; }
	inline Device* GetDevice() { return myDevice; }

	operator VkCommandPool() { return myCommandPool; }

	CommandPool& operator=(const CommandPool&) = delete;

private:
	void Initialize();

	Device* myDevice;
	VkCommandPool myCommandPool;

	VkCommandPoolCreateFlags myResetFlags;
};

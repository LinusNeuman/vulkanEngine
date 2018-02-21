/* TODO: 
		One Device per gpu - Done
		On every Device create different queues for different types
			-Graphics and transfers
			-Compute
			-Dedicated Transfer Buffer
*/

#pragma once

class GPU;

class Device
{
public:
	Device(GPU& gpu, bool exclusiveSupportedFlags, VkQueueFlags supportedFlags);
	Device(GPU& gpu, bool exclusiveSupportedFlags, int supportedFlags);
	Device(const Device&) = delete;
	~Device();

	inline VkDevice GetVkDevice() { return myVkDevice; }
	inline GPU& GetGPU() { return *myGPU; }
	inline uint32_t GetQueueFamilyIndex() { return myQueueFamilyIndex; }

	inline VkPhysicalDeviceProperties GetVkPhysicalDeviceProperties() { return myPhysicalDeviceProperties; }
	inline VkPhysicalDeviceMemoryProperties GetVkPhysicalDeviceMemoryProperties() { return myPhysicalDeviceMemoryProperties; }

	operator VkDevice() { return myVkDevice; }

	// Operator
	Device& operator=(const Device&) = delete;

private:
	uint32_t GetBestQueueFamilyIndex(GPU& gpu, bool exclusiveSupportedFlags, VkQueueFlags supportedFlags);

	void Initialize();

	VkPhysicalDeviceProperties myPhysicalDeviceProperties;
	VkPhysicalDeviceMemoryProperties myPhysicalDeviceMemoryProperties;

	VkInstance myVkInstanceReference;
	GPU* myGPU;
	VkDevice myVkDevice;

	uint32_t myQueueFamilyIndex;
};

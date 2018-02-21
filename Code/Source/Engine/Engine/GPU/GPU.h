#pragma once

class GPU
{
public:
	GPU(VkPhysicalDevice physicalDevice);
	GPU(const GPU&) = delete;

	GPU& operator=(const GPU&) = delete;

	inline VkPhysicalDevice GetPhysicalDevice() { return myPhysicalDevice; }

	operator VkPhysicalDevice() { return myPhysicalDevice; }

private:
	VkPhysicalDevice myPhysicalDevice;
};

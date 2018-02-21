#include "stdafx.h"
#include "Queue.h"
#include "Engine\Engine.h"
#include "..\Device\Device.h"

Queue::Queue()
{
	myVkQueue = nullptr;
}

void Queue::Initialize()
{
	Device* device = Engine::GetInstance()->GetMainDevice();
	VkDevice vkDevice = device->GetVkDevice();
	uint32_t queueFamilyIndex = device->GetQueueFamilyIndex();

	vkGetDeviceQueue(vkDevice, queueFamilyIndex, 0, &myVkQueue);
}

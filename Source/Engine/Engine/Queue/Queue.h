#pragma once

class Queue
{
public:
	Queue();

	void Initialize();

	inline VkQueue GetVkQueue() { return myVkQueue; }

	operator VkQueue() { return myVkQueue; }

private:
	VkQueue myVkQueue;
};

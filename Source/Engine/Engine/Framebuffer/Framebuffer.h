#pragma once

class Device;
class Texture;
class RenderPass;

class Framebuffer
{
public:
	Framebuffer(Device& device, RenderPass& renderPass);
	~Framebuffer();

	void Initialize();

	operator VkFramebuffer() { return myFramebuffer; }

private:
	void Destroy();

	Device& myDevice;
	RenderPass& myRenderPass;

	VkFramebuffer myFramebuffer;
};

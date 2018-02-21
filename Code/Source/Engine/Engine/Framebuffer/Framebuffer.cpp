#include "stdafx.h"
#include "Framebuffer.h"
#include "..\Device\Device.h"
#include "..\Texture\Texture.h"
#include "..\RenderPass\RenderPass.h"

Framebuffer::Framebuffer(Device& device, RenderPass& renderPass)
	: myDevice(device),
	myRenderPass(renderPass)
{
	myFramebuffer = VK_NULL_HANDLE;

	Initialize();
}

Framebuffer::~Framebuffer()
{
	Destroy();
}

void Framebuffer::Destroy()
{
	if (myFramebuffer != VK_NULL_HANDLE)
	{
		vkDestroyFramebuffer(myDevice, myFramebuffer, VK_ALLOCATOR);
		myFramebuffer = VK_NULL_HANDLE;
	}
}

void Framebuffer::Initialize()
{
	Destroy();

	std::vector<VkImageView> attachments = {};
	int width = 0;
	int height = 0;

	const std::vector<Texture*>& textures = myRenderPass.GetAttachments();

	if (textures.size() > 0)
	{

		for (size_t i = 0; i < textures.size(); ++i)
		{
			attachments.push_back(*textures[i]);
		}

		width = textures[0]->GetWidth();
		height = textures[0]->GetHeight();
	}

	VkFramebufferCreateInfo framebuffer_create_info = {};
	framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebuffer_create_info.renderPass = myRenderPass;
	framebuffer_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
	framebuffer_create_info.pAttachments = attachments.data();
	framebuffer_create_info.width = width;
	framebuffer_create_info.height = height;
	framebuffer_create_info.layers = 1;

	ErrorCheck(vkCreateFramebuffer(myDevice, &framebuffer_create_info, VK_ALLOCATOR, &myFramebuffer));
}

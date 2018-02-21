#include "stdafx.h"
#include "RenderPass.h"

#include "..\Device\Device.h"
#include "..\Texture\Texture.h"

#include <array>

RenderPass::RenderPass(Device& device)
	: myDevice(&device)
{
	myRenderPass = VK_NULL_HANDLE;
}

RenderPass::~RenderPass()
{
	if (myRenderPass != VK_NULL_HANDLE)
	{
		vkDestroyRenderPass(*myDevice, myRenderPass, VK_ALLOCATOR);
		myRenderPass = VK_NULL_HANDLE;
	}
}

void RenderPass::Initialize()
{
	// Attachments
	std::array<VkAttachmentDescription, 2> attachments = {};
	attachments[0].format = myTextures[0]->GetFormat();
	attachments[0].samples = myTextures[0]->GetMultiSampleCount();
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	attachments[1].format = myTextures[1]->GetFormat();
	attachments[1].samples = myTextures[1]->GetMultiSampleCount();
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//Sub passes
	std::array<VkAttachmentReference, 1> sub_pass_0_depth_stencil_attachments = {};
	sub_pass_0_depth_stencil_attachments[0].attachment = 1;
	sub_pass_0_depth_stencil_attachments[0].layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	std::array<VkAttachmentReference, 1> sub_pass_0_color_attachments = {};
	sub_pass_0_color_attachments[0].attachment = 0;
	sub_pass_0_color_attachments[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	std::array<VkSubpassDescription, 1> sub_passes = {};
	sub_passes[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	sub_passes[0].colorAttachmentCount = static_cast<uint32_t>(sub_pass_0_color_attachments.size());
	sub_passes[0].pColorAttachments = sub_pass_0_color_attachments.data();
	sub_passes[0].pDepthStencilAttachment = sub_pass_0_depth_stencil_attachments.data();

	// Render pass
	VkRenderPassCreateInfo render_pass_create_info = {};
	render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
	render_pass_create_info.pAttachments = attachments.data();
	render_pass_create_info.subpassCount = static_cast<uint32_t>(sub_passes.size());
	render_pass_create_info.pSubpasses = sub_passes.data();

	ErrorCheck(vkCreateRenderPass(*myDevice, &render_pass_create_info, VK_ALLOCATOR, &myRenderPass));
}

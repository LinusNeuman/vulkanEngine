#pragma once

class Device;
class Texture;

class RenderPass
{
public:
	RenderPass(Device& device);
	~RenderPass();

	void Initialize();

	void SetAttachments(std::vector<Texture*> textures) { myTextures = textures; }
	const std::vector<Texture*>& GetAttachments() const { return myTextures; }

	operator VkRenderPass() { return myRenderPass; }

private:
	std::vector<Texture*> myTextures;

	Device* myDevice;
	VkRenderPass myRenderPass;
};

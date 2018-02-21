#pragma once

class Device;

class Texture
{
public:
	Texture(Device& device);
	Texture(const Texture& texture);
	virtual ~Texture();
	
	inline void SetWidth(const uint32_t width) { myWidth = width; }
	inline void SetHeight(const uint32_t height) { myHeight = height; }
	inline void SetFormat(const VkFormat format) { myFormat = format; }
	inline void SetImage(const VkImage image) { myImage = image; }
	inline void SetImageUsageFlags(const VkImageUsageFlagBits usage) { myImageUsage = usage; }
	inline void SetMultiSampleCount(const VkSampleCountFlagBits multiSampling) { myMultiSampleCount = multiSampling; }
	inline void SetImageType(const VkImageType imageType) { myImageType = imageType; }
	inline void SetImageViewType(const VkImageViewType imageViewType) { myImageViewType = imageViewType; }
	inline void SetImageAspectMask(const VkImageAspectFlagBits imageViewAspectMask) { myImageAspectMask = imageViewAspectMask; }

	bool TryFormat(VkFormat format, VkFormatFeatureFlagBits feature);

	void Initialize();

	inline uint32_t GetWidth() const { return myWidth; }
	inline uint32_t GetHeight() const { return myHeight; }
	inline VkFormat GetFormat() const { return myFormat; }
	inline VkSampleCountFlagBits GetMultiSampleCount() const { return myMultiSampleCount; }
	inline VkImageView GetImageView() { return myImageView; }

	operator VkImage() { return myImage; }
	operator VkImageView() { return myImageView; }

	Texture& operator=(const Texture&) = delete;

private:
	void Destroy();

	Device* myDevice;

	VkDeviceMemory myDeviceMemory;
	VkImage myImage;
	VkImageView myImageView;
	VkFormat myFormat;
	VkImageType myImageType;
	VkImageViewType myImageViewType;
	VkImageUsageFlagBits myImageUsage;
	VkSampleCountFlagBits myMultiSampleCount;
	VkImageAspectFlagBits myImageAspectMask;

	uint32_t myWidth;
	uint32_t myHeight;

	const char* myFileName;
};

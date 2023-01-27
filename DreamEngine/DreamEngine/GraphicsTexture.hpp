#pragma once
#pragma once
#include <vulkan/vulkan.h>
#include"GraphicsTextureUtil.hpp"
#include "GraphicsInstance.hpp"
#include "GraphicsDevice.h"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h" //Can not be used in .h
#endif // !1

#include"GraphicsCommandBuffer.hpp"
namespace Graphics
{
	class GraphicsTexture {
		
	public:
		static enum TextureType
		{
			Depth,
			Default,
			DepthWithSampler
		};
		GraphicsTexture( VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) 
		{
			m_device = GraphicsDevice::getInstance()->getLogicDevice();
			m_physicalDevice = GraphicsDevice::getInstance()->getPhysicDevice();
			m_queue = GraphicsDevice::getInstance()->getGraphicsQueue();
			m_commandPool = VK_NULL_HANDLE;
			m_aspectFlags = aspectFlags;
			m_imageView = createImageView(image, format, aspectFlags);
		};

		GraphicsTexture(VkCommandPool commandPool,VkImageAspectFlags aspectFlags, VkFormat format,std::string path) {
			m_device = GraphicsDevice::getInstance()->getLogicDevice();
			m_physicalDevice = GraphicsDevice::getInstance()->getPhysicDevice();
			m_queue = GraphicsDevice::getInstance()->getGraphicsQueue();
			m_commandPool = commandPool;
			m_aspectFlags = aspectFlags;
			m_format = format;
			createTextureImage(path);
			m_imageView = createImageView(m_image, m_format, aspectFlags);
			createTextureSampler();
			m_type = Default;
		};

		GraphicsTexture(TextureType type) {
			m_device = GraphicsDevice::getInstance()->getLogicDevice();
			m_physicalDevice = GraphicsDevice::getInstance()->getPhysicDevice();
			if (type == Depth) {
				createDepthResources();
				m_type = type;
			}
		};


		void createDepthResources() {
			VkExtent2D swapChainExtent = GraphicsDevice::getInstance()->getSwapChainExtent();
			VkFormat depthFormat = GraphicsUtil::getInstance()->findDepthFormat(m_physicalDevice);
			createImage(swapChainExtent.width, swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_image, m_imageMemory);
			m_imageView = createImageView(m_image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
		}

		void createTextureSampler() {
			VkSamplerCreateInfo samplerInfo{};
			samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerInfo.magFilter = VK_FILTER_LINEAR;
			samplerInfo.minFilter = VK_FILTER_LINEAR;
			samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.anisotropyEnable = VK_FALSE;
			VkPhysicalDeviceProperties properties{};
			vkGetPhysicalDeviceProperties(m_physicalDevice, &properties);
			samplerInfo.maxAnisotropy = 1.0F;
			samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			samplerInfo.unnormalizedCoordinates = VK_FALSE;
			samplerInfo.compareEnable = VK_FALSE;
			samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
			samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerInfo.mipLodBias = 0.0f;
			samplerInfo.minLod = 0.0f;
			samplerInfo.maxLod = 0.0f;
			if (vkCreateSampler(m_device, &samplerInfo, nullptr, &m_textureSampler) != VK_SUCCESS) {
				throw std::runtime_error("failed to create texture sampler!");
			}
		}
		void createTextureImage(std::string path) {
			int texWidth, texHeight, texChannels;

			stbi_uc* pixels = stbi_load(path.data(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
			VkDeviceSize imageSize = texWidth * texHeight * 4;
			width = texWidth;
			height = texHeight;

			if (pixels == nullptr) {
				throw std::runtime_error("failed to load texture image!");
			}

			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;
			createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				, stagingBuffer, stagingBufferMemory);

			void* data;
			vkMapMemory(m_device, stagingBufferMemory, 0, imageSize, 0, &data);
			memcpy(data, pixels, static_cast<size_t>(imageSize));
			vkUnmapMemory(m_device, stagingBufferMemory);

			createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_image, m_imageMemory);
			stbi_image_free(pixels);

			transitionImageLayout(m_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			copyBufferToImage(stagingBuffer, m_image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
			transitionImageLayout(m_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

			vkDestroyBuffer(m_device, stagingBuffer, nullptr);
			vkFreeMemory(m_device, stagingBufferMemory, nullptr);
		}
		VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
		{
			return GraphicsTextureUtil::createImageView(m_device ,image, format, aspectFlags);
		}

		void createBuffer(VkDeviceSize buffersize, VkBufferUsageFlags bufferusageflags, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
			VkBufferCreateInfo bufferInfo{};
			auto graphicsHelper = GraphicsUtil::getInstance();
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = buffersize;
			bufferInfo.usage = bufferusageflags;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			if (vkCreateBuffer(m_device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
				throw std::runtime_error("failed to create vertex Buffer!");
			}
			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(m_device, buffer, &memRequirements);
			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = graphicsHelper->findMemoryType(memRequirements.memoryTypeBits, properties, m_physicalDevice);
			if (vkAllocateMemory(m_device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
				throw std::runtime_error("failed to alloc vertex Buffer Memory!");
			}
			vkBindBufferMemory(m_device, buffer, bufferMemory, 0);
		}

		void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = width;
			imageInfo.extent.height = height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = format;
			imageInfo.tiling = tiling;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = usage;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			auto graphicsHelper = GraphicsUtil::getInstance();
			if (vkCreateImage(m_device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
				throw std::runtime_error("failed to create image!");
			}

			VkMemoryRequirements memRequirements;
			vkGetImageMemoryRequirements(m_device, image, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = graphicsHelper->findMemoryType(memRequirements.memoryTypeBits, properties, m_physicalDevice);

			if (vkAllocateMemory(m_device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate image memory!");
			}

			vkBindImageMemory(m_device, image, imageMemory, 0);
		}

		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
			GraphicsCommandBuffer graphicsCommandBuffer(m_device, m_commandPool, m_queue);
			VkCommandBuffer commandBuffer = graphicsCommandBuffer.beginSingleTimeCommands();
			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout = oldLayout;
			barrier.newLayout = newLayout;

			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

			barrier.image = image;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseMipLevel = 0;
			barrier.subresourceRange.levelCount = 1;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;

			VkPipelineStageFlags sourceStage;
			VkPipelineStageFlags destinationStage;

			if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			}
			else {
				throw std::invalid_argument("unsupported layout transition!");
			}

			if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

				if (hasStencilComponent(format)) {
					barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
				}
			}
			else {
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			}
			vkCmdPipelineBarrier(
				commandBuffer,
				sourceStage, destinationStage,
				0,
				0, nullptr,
				0, nullptr,
				1, &barrier
			);
			graphicsCommandBuffer.endSingleTimeCommands();
		}
		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
			GraphicsCommandBuffer graphicsCommandBuffer(m_device, m_commandPool, m_queue);
			VkCommandBuffer commandBuffer = graphicsCommandBuffer.beginSingleTimeCommands();
			VkBufferImageCopy region{};
			region.bufferOffset = 0;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;

			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;

			region.imageOffset = { 0, 0, 0 };
			region.imageExtent = {
				width,
				height,
				1
			};
			vkCmdCopyBufferToImage(
				commandBuffer,
				buffer,
				image,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&region
			);
			graphicsCommandBuffer.endSingleTimeCommands();
		}

		bool hasStencilComponent(VkFormat format) {
			return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
		}

		VkImage getImage() {
			return m_image;
		}
		VkImageView getImageView() {
			return m_imageView;
		}

		VkDeviceMemory getImageMemory() {
			return m_imageMemory;
		}

		VkSampler getSampler() {
			return m_textureSampler;
		}

		uint16_t getWidth() {
			return width;
		}

		uint16_t getHeight() {
			return height;
		}

		~GraphicsTexture() {
			if(m_type == Default)
				vkDestroySampler(m_device, m_textureSampler, nullptr);
			vkDestroyImageView(m_device, m_imageView, nullptr);
			vkDestroyImage(m_device, m_image, nullptr);
			vkFreeMemory(m_device, m_imageMemory, nullptr);
		}
	private:
		uint16_t width, height;
		VkDevice m_device;
		VkPhysicalDevice m_physicalDevice;
		VkImageView m_imageView;
		VkCommandPool m_commandPool;
		VkImage m_image;
		VkDeviceMemory m_imageMemory;
		VkQueue m_queue;
		VkFormat m_format;
		VkImageAspectFlags m_aspectFlags;
		TextureType m_type;
		VkSampler m_textureSampler;
	};

}
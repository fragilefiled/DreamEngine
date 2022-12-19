#pragma once
#pragma once
#pragma once
#include <vulkan/vulkan.h>
#include"GraphicsUtil.hpp"
#include "GraphicsInstance.hpp"
#include "GraphicsDevice.hpp"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h" //Can not be used in .h
#endif // !1

namespace Graphics
{
	class GraphicsBuffer {

	public:
		GraphicsBuffer(std::shared_ptr<GraphicsDevice> graphicsDevice, VkFormat format, VkCommandPool commandPool) {
			m_graphicsDevice = graphicsDevice;
			m_device = graphicsDevice->getLogicDevice();
			m_physicalDevice = graphicsDevice->getPhysicDevice();
			m_queue = graphicsDevice->getGraphicsQueue();
			m_commandPool = commandPool;
		};

		

	private:
		std::shared_ptr<GraphicsDevice> m_graphicsDevice;
		VkDevice m_device;
		VkPhysicalDevice m_physicalDevice;
		VkImageView m_imageView;
		VkCommandPool m_commandPool;
		VkImage m_image;
		VkDeviceMemory m_imageMemory;
		VkQueue m_queue;
		VkFormat m_format;
		VkImageAspectFlags m_aspectFlags;
	};

}
#pragma once
#pragma once
#pragma once
#include <vulkan/vulkan.h>
#include"GraphicsUtil.h"
#include "GraphicsInstance.hpp"
#include "GraphicsDevice.h"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h" //Can not be used in .h
#endif // !1

namespace Graphics
{
	class GraphicsBuffer {

	public:
		GraphicsBuffer(VkFormat format, VkCommandPool commandPool) {
			m_device = GraphicsDevice::getInstance()->getLogicDevice();
			m_physicalDevice = GraphicsDevice::getInstance()->getPhysicDevice();
			m_queue = GraphicsDevice::getInstance()->getGraphicsQueue();
			m_commandPool = commandPool;
		};

		

	private:
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
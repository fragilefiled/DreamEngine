#pragma once
#pragma once
#include"GraphicsRenderPass.hpp"
#include"GraphicsTexture.h"
namespace Graphics {
	class GraphicsCommandPool
	{
	public:
		GraphicsCommandPool() 
		{
			m_device = GraphicsDevice::getInstance()->getLogicDevice();
			m_physicalDevice = GraphicsDevice::getInstance()->getPhysicDevice();
			m_surface = GraphicsDevice::getInstance()->getGraphicsInstance()->getSurface();
			createCommandPool();
		};


		~GraphicsCommandPool() {
			vkDestroyCommandPool(m_device, m_commandPool, nullptr);

		}
		void createCommandPool() {
			GraphicsUtil::QueueFamilyIndices queueFamilyIndices = GraphicsUtil::getInstance()->findQueueFamilies(m_physicalDevice, m_surface);
			VkCommandPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
			if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
				throw std::runtime_error("failed to create command pool!");
			GraphicsUtil::getInstance()->setMainCommandPool(m_commandPool);
		}

		VkCommandPool getCommandPool() {
			return m_commandPool;
		}

	private:
		VkDevice m_device;
		VkPhysicalDevice  m_physicalDevice;
		VkCommandPool m_commandPool;
		VkSurfaceKHR m_surface;

	};
}
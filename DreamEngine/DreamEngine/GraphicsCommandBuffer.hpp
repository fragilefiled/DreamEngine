#pragma once
#include <vulkan/vulkan.h>
#include"GraphicsUtil.hpp"
#include "GraphicsInstance.hpp"
namespace Graphics
{
	class GraphicsCommandBuffer {
	public:
		GraphicsCommandBuffer(VkDevice device, VkCommandPool commandPool, VkQueue queue) {
			m_device = device;
			m_commandPool = commandPool;
			m_Queue = queue;
		}
		VkCommandBuffer beginSingleTimeCommands() {
			m_commandBuffer = beginSingleTimeCommandsInline(m_device, m_commandPool);
			return m_commandBuffer;
		}
		void endSingleTimeCommands() 
		{
			endSingleTimeCommandsInline(m_commandBuffer, m_device, m_commandPool, m_Queue);
		}

	private:
		VkCommandBuffer beginSingleTimeCommandsInline(VkDevice device, VkCommandPool commandPool) {
			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = commandPool;
			allocInfo.commandBufferCount = 1;

			VkCommandBuffer commandBuffer;
			vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(commandBuffer, &beginInfo);

			return commandBuffer;
		}

		void endSingleTimeCommandsInline(VkCommandBuffer commandBuffer, VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue) {
			vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
			vkQueueWaitIdle(graphicsQueue);

			vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
		}

	private:
		VkCommandPool m_commandPool;
		VkDevice m_device;
		VkQueue m_Queue;
		VkCommandBuffer m_commandBuffer;
	};

}
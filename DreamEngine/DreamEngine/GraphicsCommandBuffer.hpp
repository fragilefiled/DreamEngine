#pragma once
#include <vulkan/vulkan.h>
#include "GraphicsUtil.h"
#include "GraphicsInstance.hpp"
#include "GraphicsCommandPool.hpp"
namespace Graphics
{
	class GraphicsCommandBuffer {
	public:
		GraphicsCommandBuffer(VkDevice device, VkCommandPool commandPool, VkQueue queue) {
			m_device = device;
			m_commandPool = commandPool;
			m_Queue = queue;
		}
		GraphicsCommandBuffer(std::shared_ptr<GraphicsCommandPool> m_graphicsCommandPool) {
			m_device = GraphicsDevice::getInstance()->getLogicDevice();
			m_commandPool = m_graphicsCommandPool->getCommandPool();
			m_Queue = GraphicsDevice::getInstance()->getPresentQueue();
		}

		VkCommandBuffer beginSingleTimeCommands() {
			m_commandBuffers.resize(1);
			m_commandBuffers[0] = beginSingleTimeCommandsInline(m_device, m_commandPool);
			return m_commandBuffers[0];
		}
		void endSingleTimeCommands() 
		{
			endSingleTimeCommandsInline(m_commandBuffers[0], m_device, m_commandPool, m_Queue);
		}

		void createCommandBuffers() {
			auto graphicsHelper = GraphicsUtil::getInstance();
			int MAX_FRAMES_IN_FLIGHT = graphicsHelper->MAX_FRAMES_IN_FLIGHT;
			m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = m_commandPool;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;
			if (vkAllocateCommandBuffers(m_device, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
				throw std::runtime_error("failed to allocate command buffers!");
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
	public:
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
			VkCommandBuffer commandBuffer = beginSingleTimeCommands();

			VkBufferCopy copyRegion{};
			copyRegion.srcOffset = 0; // Optional
			copyRegion.dstOffset = 0; // Optional
			copyRegion.size = size;
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

			endSingleTimeCommands();
		}



		std::vector<VkCommandBuffer> getCommandBufffers() {
			return m_commandBuffers;
		}

	private:
		VkCommandPool m_commandPool;
		VkDevice m_device;
		VkQueue m_Queue;
		std::vector<VkCommandBuffer> m_commandBuffers;
	};

}
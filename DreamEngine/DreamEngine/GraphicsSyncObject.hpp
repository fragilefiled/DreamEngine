#pragma once
#pragma once
#pragma once
#include"GraphicsRenderPass.hpp"
#include"GraphicsTexture.h"
namespace Graphics {
	class GraphicsSyncObject
	{
	public:
		GraphicsSyncObject()
		{
			m_device = GraphicsDevice::getInstance()->getLogicDevice();
			createSyncObjects();
		};


		~GraphicsSyncObject() {
			auto graphicsHelper = GraphicsUtil::getInstance();
			int MAX_FRAMES_IN_FLIGHT = graphicsHelper->MAX_FRAMES_IN_FLIGHT;
			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
				vkDestroySemaphore(m_device, m_renderFinishedSemaphores[i], nullptr);
				vkDestroySemaphore(m_device, m_imageAvailableSemaphores[i], nullptr);
				vkDestroyFence(m_device, m_inFlightFences[i], nullptr);
			}
		}
		void createSyncObjects() {
			m_imageAvailableSemaphores.resize(2);
			m_renderFinishedSemaphores.resize(2);
			m_inFlightFences.resize(2);
			auto graphicsHelper = GraphicsUtil::getInstance();
			int MAX_FRAMES_IN_FLIGHT = graphicsHelper->MAX_FRAMES_IN_FLIGHT;
			VkSemaphoreCreateInfo semaphoreInfo{};
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			VkFenceCreateInfo fenceInfo{};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;//First frame signal
			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
				if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
					vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
					vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS) {

					throw std::runtime_error("failed to create synchronization objects for a frame!");
				}
			}
		}

		VkCommandPool getCommandPool() {
			return m_commandPool;
		}
		std::vector<VkSemaphore> getimageAvailableSemaphores() {
			return m_imageAvailableSemaphores;
		}
		std::vector<VkFence> getinFlightFences() {
			return m_inFlightFences;
		}
		std::vector<VkSemaphore> getrenderFinishedSemaphores() {
			return m_renderFinishedSemaphores;
		}

	private:
		VkDevice m_device;
		VkCommandPool m_commandPool;
		VkSurfaceKHR m_surface;
		std::vector<VkSemaphore> m_imageAvailableSemaphores;
		std::vector<VkSemaphore> m_renderFinishedSemaphores;
		std::vector<VkFence> m_inFlightFences;
	};
}
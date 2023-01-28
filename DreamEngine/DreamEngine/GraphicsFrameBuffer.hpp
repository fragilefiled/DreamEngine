#pragma once
#include"GraphicsRenderPass.hpp"
#include"GraphicsTexture.h"
namespace Graphics {
	class GraphicsFrameBuffer 
	{
	public:
		GraphicsFrameBuffer(std::shared_ptr<GraphicsRenderPass> graphicsRenderPass, std::shared_ptr<GraphicsSwapChain> graphicsSwapChain, std::shared_ptr<GraphicsTexture> graphicsDepthResources) {
			m_graphicsRenderPass = graphicsRenderPass;
			m_graphicsSwapChain = graphicsSwapChain;
			m_device = GraphicsDevice::getInstance()->getLogicDevice();
			m_depthImageView = graphicsDepthResources->getImageView();
			createFramebuffers();
		};

		void createFramebuffers() {
			auto swapChainImageViews = m_graphicsSwapChain->getswapChainImageViews();
			m_swapChainFramebuffers.resize(swapChainImageViews.size());
			for (int i = 0; i < m_swapChainFramebuffers.size(); i++) {
				std::array<VkImageView, 2> attachments = {
					swapChainImageViews[i],
					m_depthImageView
				};
				VkExtent2D swapChainExtent = GraphicsDevice::getInstance()->getSwapChainExtent();
				VkFramebufferCreateInfo framebufferInfo{};
				framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebufferInfo.renderPass = m_graphicsRenderPass->getRenderPass();
				framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
				framebufferInfo.pAttachments = attachments.data();
				framebufferInfo.width = swapChainExtent.width;
				framebufferInfo.height = swapChainExtent.height;
				framebufferInfo.layers = 1;
				if (vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]) != VK_SUCCESS)
					throw std::runtime_error("failed to create framebuffer!");
			}

		}
		~GraphicsFrameBuffer() {
			m_graphicsRenderPass.reset();
			for (size_t i = 0; i < m_swapChainFramebuffers.size(); i++) {
				vkDestroyFramebuffer(m_device, m_swapChainFramebuffers[i], nullptr);
			}

		}

		std::vector<VkFramebuffer> getSwapChainFrameBuffers() {
			return m_swapChainFramebuffers;
		}
	private:
		std::shared_ptr<GraphicsRenderPass> m_graphicsRenderPass;
		std::shared_ptr<GraphicsSwapChain> m_graphicsSwapChain;
		std::vector<VkFramebuffer> m_swapChainFramebuffers;
		VkDevice m_device;
		VkImageView m_depthImageView;

	};
}
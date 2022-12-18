#pragma once
#include <vulkan/vulkan.h>
#include"GraphicsUtil.hpp"
#include "GraphicsInstance.hpp"
#include "GraphicsDevice.hpp"
namespace Graphics
{
	class GraphicsSwapChain 
	{
		public:
			GraphicsSwapChain(std::shared_ptr<GraphicsInstance> graphicsInstance, std::shared_ptr<GraphicsDevice> graphicsDevice) {
				this->m_graphicsDevice = graphicsDevice;
				this->m_graphicsInstance = graphicsInstance;
				m_physicalDevice = m_graphicsDevice->getPhysicDevice();
				m_device = m_graphicsDevice->getLogicDevice();
				m_vkSurface = m_graphicsInstance->getSurface();
				m_window = m_graphicsInstance->getGLTFWindow();
				createSwapChain();
			}
			void createSwapChain() 
			{
				auto graphicsHelper = GraphicsUtil::getInstance();
				GraphicsUtil::SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_physicalDevice);
				VkSurfaceFormatKHR surfaceFormat = graphicsHelper->chooseSwapSurfaceFormat(swapChainSupport.formats);
				VkPresentModeKHR presentMode = graphicsHelper->choosePresentMode(swapChainSupport.presentModes);
				VkExtent2D extent = graphicsHelper->chooseCapabilities(swapChainSupport.capabilities, m_window);
				uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
				if (swapChainSupport.capabilities.maxImageCount > 0 && swapChainSupport.capabilities.maxImageCount < imageCount)
					imageCount = swapChainSupport.capabilities.maxImageCount;
				VkSwapchainCreateInfoKHR createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
				createInfo.surface = m_vkSurface;
				createInfo.minImageCount = imageCount;
				createInfo.imageFormat = surfaceFormat.format;
				createInfo.imageColorSpace = surfaceFormat.colorSpace;
				createInfo.imageExtent = extent;
				createInfo.imageArrayLayers = 1;
				createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

				GraphicsUtil::QueueFamilyIndices indices = graphicsHelper->findQueueFamilies(m_physicalDevice, m_vkSurface);
				uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
				if (indices.graphicsFamily.value() != indices.presentFamily.value()) {
					createInfo.pQueueFamilyIndices = queueFamilyIndices;
					createInfo.queueFamilyIndexCount = 2;
					createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				}
				else
				{
					createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
					createInfo.queueFamilyIndexCount = 0; // Optional
					createInfo.pQueueFamilyIndices = nullptr; // Optional
				}
				createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
				createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; //Don't Blend With Other Window
				createInfo.presentMode = presentMode;
				createInfo.clipped = VK_TRUE;
				createInfo.oldSwapchain = VK_NULL_HANDLE;

				m_swapChainImageFormat = surfaceFormat.format;
				m_swapChainExtent = extent;
				if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapChain) != VK_SUCCESS)
					throw std::runtime_error("swapChain Create Failed !");
				vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, nullptr);
				m_swapChainImages.resize(imageCount);
				vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, m_swapChainImages.data());

			}

			GraphicsUtil::SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice& device) {
				GraphicsUtil::SwapChainSupportDetails details;

				vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_vkSurface, &details.capabilities);
				uint32_t formatCount;
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_vkSurface, &formatCount, nullptr);
				if (formatCount != 0) {
					details.formats.resize(formatCount);
					vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_vkSurface, &formatCount, details.formats.data());
				}
				uint32_t presentModeCount;
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_vkSurface, &presentModeCount, nullptr);
				if (presentModeCount != 0) {
					details.presentModes.resize(presentModeCount);
					vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_vkSurface, &presentModeCount, details.presentModes.data());
				}
				return details;
			}
			VkSwapchainKHR getswapChain() {
				return m_swapChain;
			}

			std::vector<VkImage> getswapChainImages() {
				return m_swapChainImages;
			}

			std::vector<VkImageView> getswapChainImageViews() {
				return m_swapChainImageViews;
			}

			VkFormat getswapChainImageFormat() {
				return m_swapChainImageFormat;
			}

			VkExtent2D getswapChainExtent() {
				return m_swapChainExtent;
			}

			~GraphicsSwapChain() {

				m_graphicsDevice.reset();
				m_graphicsInstance.reset();
				//暂时先不释放
			}
	private:
		VkSurfaceKHR m_vkSurface;
		VkDevice m_device;
		GLFWwindow* m_window;
		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		std::shared_ptr<GraphicsInstance> m_graphicsInstance;
		std::shared_ptr<GraphicsDevice> m_graphicsDevice;

		VkSwapchainKHR m_swapChain;
		std::vector<VkImage> m_swapChainImages;
		std::vector<VkImageView> m_swapChainImageViews;
		VkFormat m_swapChainImageFormat;
		VkExtent2D m_swapChainExtent;
	};
}
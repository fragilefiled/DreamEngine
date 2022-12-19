#pragma once
#include <iostream>
#include <memory>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <optional>
#include <set>
#include <cstdint> // Necessary for uint32_t
#include <limits> // Necessary for std::numeric_limits
#include <algorithm> // Necessary for std::clamp
#include <vulkan/vulkan.h>
#include <GLFW\glfw3.h>
namespace Graphics 
{
	class GraphicsUtil 
	{
		public:
			struct QueueFamilyIndices {
				std::optional<uint32_t> graphicsFamily; //两个队列组可能不太一样 所以需要两个不同的
				std::optional<uint32_t> presentFamily;

				bool isComplete() {
					return graphicsFamily.has_value() && presentFamily.has_value();
				}
			};
			struct SwapChainSupportDetails
			{
				VkSurfaceCapabilitiesKHR capabilities;
				std::vector<VkSurfaceFormatKHR> formats;
				std::vector<VkPresentModeKHR> presentModes;

			};


			static GraphicsUtil* getInstance() {
				if (m_instance == nullptr) {
					m_instance = std::make_unique<GraphicsUtil>();
				}
				return m_instance.get();
			}
			bool checkExtensionSupport()
			{
				uint32_t extensionCount = 0;
				vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
				std::vector<VkExtensionProperties> extensions(extensionCount);
				vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
				const char** needExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);

				for (const auto& extension : extensions) {
					std::cout << '\t' << extension.extensionName << '\n';
				}

				for (int i = 0; i < extensionCount; i++) {
					std::cout << '\t' << needExtensions[i] << '\n';
				}

				int extensionSuccessCount = 0;
				for (const auto& extension : extensions) {
					int i = 0;
					for (; i < extensionCount; i++) {
						if (strcmp(needExtensions[i], extension.extensionName) == 0) {
							std::cout << "init success " << needExtensions[i] << '\n';
							extensionSuccessCount++;
							break;
						}
					}

				}
				return true;
			}

			bool checkValidationLayerSupport() 
			{
				uint32_t layerCount;
				vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
				std::vector<VkLayerProperties> availableLayers(layerCount);
				vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
				for (const char* layerName : _validationLayers) {
					bool layerFound = false;

					for (const auto& layerProperties : availableLayers) {
						if (strcmp(layerName, layerProperties.layerName) == 0) {
							layerFound = true;
							break;
						}
					}

					if (!layerFound) {
						return false;
					}
				}
				return true;
			}
			std::vector<const char* > getRequiredExtensions(bool enableValidationLayers) {
				uint32_t glfwExtensionCount = 0;
				const char** glfwExtensions;
				glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
				std::vector<const char* > extensions = std::vector<const char* >(glfwExtensions, glfwExtensions + glfwExtensionCount); //初始地址 初始地址加偏移好Hack
				if (enableValidationLayers) {
					extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
				}
				return extensions;
			}

			bool isDeviceSuitable(VkPhysicalDevice& device, VkSurfaceKHR surface) {
				VkPhysicalDeviceProperties deviceProperties;
				vkGetPhysicalDeviceProperties(device, &deviceProperties);
				VkPhysicalDeviceFeatures deviceFeatures;
				vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
				//给硬件打分机制 Work On
				QueueFamilyIndices indices = findQueueFamilies(device, surface);
				bool extensionsSupported = checkDeviceExtensionSupport(device);
				bool swapChainAdequate = false;
				if (extensionsSupported) {
					swapChainAdequate = checkSwapChainAdequate(device, surface);
				}
				VkPhysicalDeviceFeatures supportedFeatures;
				vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
				return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;;

			}

			QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR& surface) {
				QueueFamilyIndices indices;
				uint32_t queueFamilyCount = 0;
				vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
				if (queueFamilyCount == 0) {
					throw std::runtime_error("failed to find queueFamily!");
				}
				else
				{
					std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
					vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
					for (int i = 0; i < queueFamilies.size(); i++) {
						const VkQueueFamilyProperties queueFamily = queueFamilies[i];
						if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
							indices.graphicsFamily = i;
						VkBool32 presentSupport = false;
						vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
						if (presentSupport)
							indices.presentFamily = i;
						if (indices.isComplete())
							break;
					}
				}
				return indices;
			};

			VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
				for (auto availableFormat : availableFormats) {
					if (availableFormat.colorSpace == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.format == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
						return availableFormat;
				}
				return availableFormats[0];//return default
			}
			VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
				for (auto availablePresentMode : availablePresentModes) {
					if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
						return availablePresentMode;
				}
				return VK_PRESENT_MODE_FIFO_KHR;;//return default
			}
			VkExtent2D chooseCapabilities(const VkSurfaceCapabilitiesKHR capabilities, GLFWwindow* window) {
				if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)()) {
					return capabilities.currentExtent;
				}
				else {
					int width, height;
					glfwGetFramebufferSize(window, &width, &height);

					VkExtent2D actualExtent = {
						static_cast<uint32_t>(width),
						static_cast<uint32_t>(height)
					};

					actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
					actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

					return actualExtent;
				}
			}

			uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice) {

				VkPhysicalDeviceMemoryProperties memProperties;
				vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
				for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
					if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
						return i;
					}
				}

				throw std::runtime_error("failed to find suitable memory type!");
			}



			static void release() {
				m_instance.reset();
			}

			const std::vector<const char*> _validationLayers = {
				"VK_LAYER_KHRONOS_validation"
			};
			const std::vector<const char*> _deviceExtensions = {
				VK_KHR_SWAPCHAIN_EXTENSION_NAME
			};
			~GraphicsUtil() {
				std::cout << "constructor called!" << std::endl;
			}
	private:
			static std::unique_ptr<GraphicsUtil> m_instance;



			bool checkDeviceExtensionSupport(const VkPhysicalDevice& device)
			{
				uint32_t extensionCount = 0;
				vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
				std::vector<VkExtensionProperties> extensions(extensionCount);
				vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());
				std::set<std::string> requiredExtensions(_deviceExtensions.begin(), _deviceExtensions.end());
				for (const auto& extension : extensions) {
					requiredExtensions.erase(extension.extensionName);

				}
				return requiredExtensions.empty();
			}

			bool checkSwapChainAdequate(VkPhysicalDevice& device, VkSurfaceKHR &surface) {

				bool swapChainAdequate = false;
				SwapChainSupportDetails details = querySwapChainSupport(device, surface);
				swapChainAdequate = !details.formats.empty() && !details.presentModes.empty();
				return swapChainAdequate;
			}

			SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice& device, VkSurfaceKHR &surface) {
				SwapChainSupportDetails details;

				vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
				uint32_t formatCount;
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
				if (formatCount != 0) {
					details.formats.resize(formatCount);
					vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
				}
				uint32_t presentModeCount;
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
				if (presentModeCount != 0) {
					details.presentModes.resize(presentModeCount);
					vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
				}
				return details;
			}


			
	};
	std::unique_ptr<GraphicsUtil> GraphicsUtil::m_instance = nullptr;//类内只是定义没有声明
}
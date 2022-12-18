#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <optional>
#include <set>
#include <cstdint> // Necessary for uint32_t
#include <limits> // Necessary for std::numeric_limits
#include <algorithm> // Necessary for std::clamp
#include "FileOperation.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include"glm/glm.hpp"
#include"glm/ext/matrix_transform.hpp"
#include"glm/ext/matrix_clip_space.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h" //Can not be used in .h
#include <array>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include "GraphicsInstance.hpp"
#include "GraphicsDevice.hpp"
#include "GraphicsSwapChain.hpp"
namespace Dream {
	class DreamEngine
	{
		const int MAX_FRAMES_IN_FLIGHT = 2;
		struct Vertex {
			glm::vec3 pos;
			glm::vec3 color;
			glm::vec2 texCoord;
			static VkVertexInputBindingDescription getBindingDescription() {
				VkVertexInputBindingDescription bindingDescription{};
				bindingDescription.binding = 0;
				bindingDescription.stride = sizeof(Vertex);
				bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
				return bindingDescription;
			}
			static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
				std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
				attributeDescriptions[0].binding = 0;
				attributeDescriptions[0].location = 0;
				attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[0].offset = offsetof(Vertex, pos);

				attributeDescriptions[1].binding = 0;
				attributeDescriptions[1].location = 1;
				attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[1].offset = offsetof(Vertex, color);

				attributeDescriptions[2].binding = 0;
				attributeDescriptions[2].location = 2;
				attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[2].offset = offsetof(Vertex, texCoord);
				return attributeDescriptions;
			}
		};
		struct UniformBufferObject {
			alignas(16) glm::mat4 model;
			alignas(16) glm::mat4 view;
			alignas(16) glm::mat4 proj;
		};
		const std::vector<Vertex> vertices = {
			{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
			{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

			{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
			{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
		};

		const std::vector<uint16_t> indices = {
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4
		};

		public :
			void run() 
			{
				//glm::mat4 a = glm::perspective(glm::radians( 60.0f), 1.0f, -1.0f, -100.0f);
				/*for (int i = 0; i < 4; i++)
					for (int j = 0; j < 4; j++)
						std::cout << a[i][j]<< " " ;*/
				initWindow();
				initVulkan();
				mainLoop();
				cleanUp();
			}
		private :
			void initWindow() {
				glfwInit();

				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);//NOT TO USE OPENGL CONTEXT
				glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
				_window = glfwCreateWindow(WIDTH, HEIGHT, "DreamEngine", nullptr, nullptr);
				glfwSetWindowUserPointer(_window, this);
				glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);
			}
			static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
				auto app = reinterpret_cast<DreamEngine*>(glfwGetWindowUserPointer(window));
				app->framebufferResized = true;
			}
			void initVulkan() {
				Graphics::GraphicsUtil::getInstance()->checkExtensionSupport();
				createInstanceAndSurface();
				//setUpDebugMessenger();
				//createSurface(); // before physical device
				//pickPhysicDevice();
				//createLogicDevice();
				createGraphicsDevice();
				//createSwapChain();
				createGraphicsSwapChain();
				createImageViews();
				createRenderPass();
				createDescriptorSetLayout();
				createGraphicsPipline();
				createDepthResources();
				createFramebuffers();
				createCommandPool();

				createTextureImage();
				createTextureImageView();
				createTextureSampler();
				createVertexBuffer();
				createIndexBuffer();
				createUniformBuffers();
				createDescriptorPool();
				createDescriptorSets();
				createCommandBuffers();
				createSyncObjects();
			}

			void mainLoop() {
				while (!glfwWindowShouldClose(_window)) {
					glfwPollEvents();
					drawFrame();
				}
				vkDeviceWaitIdle(_device);
			}

			void cleanUp() {	
				
				for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
					vkDestroySemaphore(_device, _renderFinishedSemaphores[i], nullptr);
					vkDestroySemaphore(_device, _imageAvailableSemaphores[i], nullptr);
					vkDestroyFence(_device, _inFlightFences[i], nullptr);
				}
				vkDestroyCommandPool(_device, _commandPool, nullptr);
				cleanUpSwapChain();
				vkDestroySampler(_device, _textureSampler, nullptr);
				vkDestroyImageView(_device, _textureImageView, nullptr);
				vkDestroyImage(_device, _textureImage, nullptr);
				vkFreeMemory(_device, _textureImageMemory, nullptr);
				for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
					vkDestroyBuffer(_device, _uniformBuffers[i], nullptr);
					vkFreeMemory(_device, _uniformBuffersMemory[i], nullptr);
				}
				vkDestroyDescriptorPool(_device, _descriptorPool, nullptr);
				vkDestroyDescriptorSetLayout(_device, _descriptorSetLayout, nullptr);
				vkDestroyBuffer(_device, _vertexBuffer, nullptr);
				vkFreeMemory(_device, _vertexBufferMemory, nullptr);
				vkDestroyBuffer(_device, _indexBuffer, nullptr);
				vkFreeMemory(_device, _indexBufferMemory, nullptr);
				/*for (auto framebuffer : _swapChainFramebuffers) {
					vkDestroyFramebuffer(_device, framebuffer, nullptr);
				}
				for (auto imageView : _swapChainImageViews) {
					vkDestroyImageView(_device, imageView, nullptr);
				}
				vkDestroySwapchainKHR(_device, _swapChain, nullptr);*/
				vkDestroyPipeline(_device, _graphicsPipeline, nullptr);
				vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
				vkDestroyRenderPass(_device, _renderPass, nullptr);
				
				//  vkDestroyDevice(_device, nullptr);
				
				//vkDestroySurfaceKHR(_instance, _surface, nullptr);
				//if (_enableValidationLayers)
				//	DestroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
				//vkDestroyInstance(_instance, nullptr);
				graphicsDevice.reset();
				graphicsInstance.reset();
				glfwDestroyWindow(_window);
				glfwTerminate();
			}

			void createInstanceAndSurface() {
				graphicsInstance =std::make_shared<Graphics::GraphicsInstance>(_enableValidationLayers, _window);
				_instance = (graphicsInstance->getInstance());
				_surface = graphicsInstance->getSurface();
			}

			void createGraphicsDevice() {
				graphicsDevice = std::make_shared<Graphics::GraphicsDevice>(graphicsInstance);
				_device = graphicsDevice->getLogicDevice();
				_physicalDevice = graphicsDevice->getPhysicDevice();
				_graphicsQueue = graphicsDevice->getGraphicsQueue();
				_presentQueue = graphicsDevice->getPresentQueue();
			}

			void createGraphicsSwapChain() {
				graphicsSwapChain = std::make_shared<Graphics::GraphicsSwapChain>(graphicsInstance, graphicsDevice);
				_swapChain = graphicsSwapChain->getswapChain();
				_swapChainImages = graphicsSwapChain->getswapChainImages();
				_swapChainImageViews = graphicsSwapChain->getswapChainImageViews();
				_swapChainImageFormat = graphicsSwapChain->getswapChainImageFormat();
				_swapChainExtent = graphicsSwapChain->getswapChainExtent();
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

				for (int i = 0; i < extensionCount ; i++) {					
					std::cout << '\t' << needExtensions[i] << '\n';
				}

				int extensionSuccessCount = 0;
				for (const auto& extension : extensions) {
					int i = 0;				
					for (; i < extensionCount ; i++) {
						if(strcmp(needExtensions[i], extension.extensionName) == 0){
							std::cout <<"init success "<<  needExtensions[i] << '\n';
							extensionSuccessCount++;
							break;
						}
					}
					
				}

				if (extensionSuccessCount == extensionCount) {
					std::cout << "Extension Init Success";
					return true;
				}
				else {					
					return false;
					throw std::runtime_error("Extension Init Fail");
				}

			}
			
			bool checkValidationLayerSupport() {
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
			std::vector<const char* > getRequiredExtensions() {
				uint32_t glfwExtensionCount = 0;
				const char** glfwExtensions;
				glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
				std::vector<const char* > extensions = std::vector<const char* >(glfwExtensions, glfwExtensions + glfwExtensionCount); //��ʼ��ַ ��ʼ��ַ��ƫ�ƺ�Hack
				if (_enableValidationLayers) {
					extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
				}
				return extensions;
			}
			static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
				VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				VkDebugUtilsMessageTypeFlagsEXT messageType,
				const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
				void* pUserData
			)//��һ���������ش����������  
			{
				std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

				return VK_FALSE;

			}

			void setUpDebugMessenger() 
			{
				if (!_enableValidationLayers)
					return;
				else 
				{
					VkDebugUtilsMessengerCreateInfoEXT createInfo{};
					populateDebugMessengerCreateInfo(createInfo);
					if (CreateDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_debugMessenger) != VK_SUCCESS) {
						throw std::runtime_error("failed to set up debug messenger!");
					}

				}

			}
			VkResult  CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo
				, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
				auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
				if (func != nullptr)
					return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
				else
					return VK_ERROR_EXTENSION_NOT_PRESENT;

			}
			void  DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT DebugMessenger,const VkAllocationCallbacks* pAllocator) {
				auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
				if (func != nullptr)
					func(instance, DebugMessenger,pAllocator);
				

			}
			void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
				createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
				createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
				createInfo.pfnUserCallback = debugCallback; //����ָ��
			}

			void pickPhysicDevice() {
				uint32_t deviceCount = 0;
				vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
				
				if (deviceCount == 0)
					throw std::runtime_error("failed to find GPUs with Vulkan support!");
				else {
					std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
					vkEnumeratePhysicalDevices(_instance, &deviceCount, physicalDevices.data());
					for(auto device : physicalDevices) 
					{
						if (isDeviceSuitable(device)) {
							_physicalDevice = device;
							break;
						}
					}
				}
				if(_physicalDevice == VK_NULL_HANDLE)
					throw std::runtime_error("failed to find suitable GPU!");
			}
			
			bool isDeviceSuitable(VkPhysicalDevice & device) {
				VkPhysicalDeviceProperties deviceProperties;
				vkGetPhysicalDeviceProperties(device, &deviceProperties);
				VkPhysicalDeviceFeatures deviceFeatures;
				vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
				//��Ӳ����ֻ��� Work On
				QueueFamilyIndices indices = findQueueFamilies(device);
				bool extensionsSupported = checkDeviceExtensionSupport(device);
				bool swapChainAdequate = false;
				if (extensionsSupported) {
					swapChainAdequate = checkSwapChainAdequate(device);
				}
				VkPhysicalDeviceFeatures supportedFeatures;
				vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
				return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;;

			}
			struct QueueFamilyIndices {
				std::optional<uint32_t> graphicsFamily; //������������ܲ�̫һ�� ������Ҫ������ͬ��
				std::optional<uint32_t> presentFamily;

				bool isComplete() {
					return graphicsFamily.has_value() && presentFamily.has_value();
				}
			};
			QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
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
						vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport);
						if (presentSupport)
							indices.presentFamily = i;
						if (indices.isComplete())
							break;
					}
				}
				return indices;
			};
			struct SwapChainSupportDetails
			{
				VkSurfaceCapabilitiesKHR capabilities;
				std::vector<VkSurfaceFormatKHR> formats;
				std::vector<VkPresentModeKHR> presentModes;

			};
			void createLogicDevice() {
				QueueFamilyIndices indices = findQueueFamilies(_physicalDevice);

				std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
				std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
				float queuePriority = 1.0f;
				for (uint32_t queueFamily : uniqueQueueFamilies) {
					VkDeviceQueueCreateInfo queueCreateInfo{};
					queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
					queueCreateInfo.queueFamilyIndex = queueFamily;
					queueCreateInfo.queueCount = 1;
					queueCreateInfo.pQueuePriorities = &queuePriority;
					queueCreateInfos.push_back(queueCreateInfo);
				}

				VkPhysicalDeviceFeatures deviceFeatures{}; //Like Gemetory Shaderɶ��

				VkDeviceCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
				createInfo.pEnabledFeatures = &deviceFeatures;
				createInfo.pQueueCreateInfos = queueCreateInfos.data();
				createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

				createInfo.enabledExtensionCount = static_cast<uint32_t>(_deviceExtensions.size());
				createInfo.ppEnabledExtensionNames = _deviceExtensions.data();

				if (_enableValidationLayers) {
					createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
					createInfo.ppEnabledLayerNames = _validationLayers.data();
				}
				else {
					createInfo.enabledLayerCount = 0;
				}//will be ignored by up-to-date implementations.

				if(vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS)
					throw std::runtime_error("failed to create logical device!");

				vkGetDeviceQueue(_device, indices.graphicsFamily.value(), 0, &_graphicsQueue); //��������
				vkGetDeviceQueue(_device, indices.presentFamily.value(), 0, &_presentQueue); //��������
				// logical device, queue family, queue index and a pointer to the variable to store the queue handle 
			}
			bool createSurface() {
				if(glfwCreateWindowSurface(_instance, _window, nullptr, &_surface) !=VK_SUCCESS)
					throw std::runtime_error("failed to create window surface!");

			}


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
			SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice& device) {
				SwapChainSupportDetails details;

				vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &details.capabilities);
				uint32_t formatCount;
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);
				if (formatCount != 0) {
					details.formats.resize(formatCount);
					vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, details.formats.data());
				}
				uint32_t presentModeCount;
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, nullptr);
				if (presentModeCount != 0) {
					details.presentModes.resize(presentModeCount);
					vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, details.presentModes.data());
				}
				return details;
			}
			bool checkSwapChainAdequate(VkPhysicalDevice& device) {

				bool swapChainAdequate = false;		
				SwapChainSupportDetails details = querySwapChainSupport(device);
				swapChainAdequate = !details.formats.empty() && !details.presentModes.empty();
				return swapChainAdequate;
			}

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
			VkExtent2D chooseCapabilities(const VkSurfaceCapabilitiesKHR capabilities) {
				if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)()) {
					return capabilities.currentExtent;
				}
				else {
					int width, height;
					glfwGetFramebufferSize(_window, &width, &height);

					VkExtent2D actualExtent = {
						static_cast<uint32_t>(width),
						static_cast<uint32_t>(height)
					};

					actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
					actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

					return actualExtent;
				}
			}
			void createSwapChain() {
				SwapChainSupportDetails swapChainSupport = querySwapChainSupport(_physicalDevice);
				VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
				VkPresentModeKHR presentMode = choosePresentMode(swapChainSupport.presentModes);
				VkExtent2D extent = chooseCapabilities(swapChainSupport.capabilities);
				uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
				if (swapChainSupport.capabilities.maxImageCount > 0 && swapChainSupport.capabilities.maxImageCount < imageCount)
					imageCount = swapChainSupport.capabilities.maxImageCount;
				VkSwapchainCreateInfoKHR createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
				createInfo.surface = _surface;
				createInfo.minImageCount = imageCount;
				createInfo.imageFormat = surfaceFormat.format;
				createInfo.imageColorSpace = surfaceFormat.colorSpace;
				createInfo.imageExtent = extent;
				createInfo.imageArrayLayers = 1;
				createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

				QueueFamilyIndices indices = findQueueFamilies(_physicalDevice);
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

				_swapChainImageFormat = surfaceFormat.format;
				_swapChainExtent = extent;
				if(vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swapChain) != VK_SUCCESS)
					throw std::runtime_error("swapChain Create Failed !");
				vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, nullptr);
				_swapChainImages.resize(imageCount);
				vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, _swapChainImages.data());

			}

			void createImageViews() {
				_swapChainImageViews.resize(_swapChainImages.size());
				for (size_t i = 0; i < _swapChainImageViews.size(); i++) {
					_swapChainImageViews[i] = createImageView(_swapChainImages[i], _swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
				}
			}

			void createGraphicsPipline() {

				auto vertShaderCode = FileOperation::readfile("Resources/Shaders/shader1.vert.spv");
				auto fragShaderCode = FileOperation::readfile("Resources/Shaders/shader1.frag.spv");
				VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
				VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

				VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
				vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
				vertShaderStageInfo.module = vertShaderModule;
				vertShaderStageInfo.pName = "main";//ѡ�����

				VkPipelineShaderStageCreateInfo fragShaderStageInfo{};//pSpecializationInfo ���Դ�������ǰ��ȫ�ֱ���
				//���������Shader�б���Ч�ʸ���
				fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				fragShaderStageInfo.module = fragShaderModule;
				fragShaderStageInfo.pName = "main";//ѡ�����
				VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

				std::vector<VkDynamicState> dynamicStates = {
					VK_DYNAMIC_STATE_VIEWPORT,
					VK_DYNAMIC_STATE_SCISSOR
				};

				VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
				dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
				dynamicStateInfo.pDynamicStates = dynamicStates.data();
				dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());

				auto bindingDescription = Vertex::getBindingDescription();
				auto attributeDescription = Vertex::getAttributeDescriptions();


				VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
				vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
				vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
				vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();
				vertexInputInfo.vertexBindingDescriptionCount = 1;
				vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;

				VkPipelineInputAssemblyStateCreateInfo inputAssemlyInfo{};
				inputAssemlyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
				inputAssemlyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
				inputAssemlyInfo.primitiveRestartEnable = VK_FALSE;


				VkViewport viewport{};
				viewport.x = 0.0f;
				viewport.y = 0.0f;
				viewport.width = (float)_swapChainExtent.width;
				viewport.height = (float)_swapChainExtent.height;
				viewport.minDepth = 0.0f;
				viewport.maxDepth = 1.0f;

				VkRect2D scissor{};
				scissor.offset = { 0, 0 };
				scissor.extent = _swapChainExtent;

				VkPipelineViewportStateCreateInfo viewportState{};
				viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
				viewportState.viewportCount = 1;
				viewportState.scissorCount = 1;
				//���Viewportû�����ö�̬״̬
				//viewportState.pViewports = &viewport;
				//viewportState.pScissors = &scissor;
				VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
				rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
				rasterizationInfo.depthClampEnable = VK_FALSE;//f depthClampEnable is set to VK_TRUE, then fragments that are beyond the near and far planes are clamped to them as opposed to discarding them
				//�������Զ��ƽ��֮�⣬��ȻᱻClamp
				rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
				rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
				rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
				rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
				rasterizationInfo.depthBiasEnable = VK_FALSE;
				rasterizationInfo.depthBiasConstantFactor = 0.0f; // Optional
				rasterizationInfo.depthBiasClamp = 0.0f; // Optional
				rasterizationInfo.depthBiasSlopeFactor = 0.0f; // Optional
				//
				rasterizationInfo.lineWidth = 1.0f; //Ĭ��ֵ
				
				VkPipelineMultisampleStateCreateInfo multisampleInfo{};
				multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
				multisampleInfo.sampleShadingEnable = VK_FALSE;
				multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
				multisampleInfo.minSampleShading = 1.0f; // Optional
				multisampleInfo.pSampleMask = nullptr; // Optional
				multisampleInfo.alphaToCoverageEnable = VK_FALSE; // Optional
				multisampleInfo.alphaToOneEnable = VK_FALSE; // Optional

				VkPipelineColorBlendAttachmentState colorBlendAttachmentInfo{};
				colorBlendAttachmentInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
				colorBlendAttachmentInfo.blendEnable = VK_FALSE;
				colorBlendAttachmentInfo.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
				colorBlendAttachmentInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
				colorBlendAttachmentInfo.colorBlendOp = VK_BLEND_OP_ADD; // Optional
				colorBlendAttachmentInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
				colorBlendAttachmentInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
				colorBlendAttachmentInfo.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

				VkPipelineColorBlendStateCreateInfo colorBlending{};
				colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
				colorBlending.logicOpEnable = VK_FALSE;
				colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
				colorBlending.attachmentCount = 1;
				colorBlending.pAttachments = &colorBlendAttachmentInfo;
				colorBlending.blendConstants[0] = 0.0f; // Optional
				colorBlending.blendConstants[1] = 0.0f; // Optional
				colorBlending.blendConstants[2] = 0.0f; // Optional
				colorBlending.blendConstants[3] = 0.0f; // Optional

				VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
				pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
				pipelineLayoutInfo.setLayoutCount = 1; // Optional
				pipelineLayoutInfo.pSetLayouts = &_descriptorSetLayout; // Optional
				pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
				pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

				if (vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
					throw std::runtime_error("failed to create pipeline layout!");
				}

				VkPipelineDepthStencilStateCreateInfo depthStencil{};
				depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
				depthStencil.depthTestEnable = VK_TRUE;
				depthStencil.depthWriteEnable = VK_TRUE;
				depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
				depthStencil.depthBoundsTestEnable = VK_FALSE;
				depthStencil.minDepthBounds = 0.0f; // Optional
				depthStencil.maxDepthBounds = 1.0f; // Optional
				depthStencil.stencilTestEnable = VK_FALSE;
				depthStencil.front = {}; // Optional
				depthStencil.back = {}; // Optional

				VkGraphicsPipelineCreateInfo pipelineInfo{};
				pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
				pipelineInfo.stageCount = 2;
				pipelineInfo.pStages = shaderStages;
				pipelineInfo.pVertexInputState = &vertexInputInfo;
				pipelineInfo.pInputAssemblyState = &inputAssemlyInfo;
				pipelineInfo.pViewportState = &viewportState;
				pipelineInfo.pRasterizationState = &rasterizationInfo;
				pipelineInfo.pMultisampleState = &multisampleInfo;
				pipelineInfo.pDepthStencilState = &depthStencil; // Optional
				pipelineInfo.pColorBlendState = &colorBlending;
				pipelineInfo.pDynamicState = &dynamicStateInfo;
				pipelineInfo.layout = _pipelineLayout;
				pipelineInfo.renderPass = _renderPass;
				pipelineInfo.subpass = 0;

				pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
				pipelineInfo.basePipelineIndex = -1; // Optional
				if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_graphicsPipeline) != VK_SUCCESS) {
					throw std::runtime_error("failed to create graphics pipeline!");
				}
				vkDestroyShaderModule(_device, fragShaderModule, nullptr);
				vkDestroyShaderModule(_device, vertShaderModule, nullptr);
			}

			VkShaderModule createShaderModule(const std::vector<char>& code) {
				VkShaderModuleCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				createInfo.codeSize = code.size();
				createInfo.pCode = reinterpret_cast<const uint32_t*> (code.data());//�ֽ���ָ��Ϊuint32_t*

				VkShaderModule shaderModule;
				if(vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
					throw std::runtime_error("ShaderModule Create Failed !");
				return shaderModule;
			}
			
			void createRenderPass() {
				VkAttachmentDescription colorAttachment{};
				colorAttachment.format = _swapChainImageFormat;
				colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
				colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

				VkAttachmentDescription depthAttachment{};
				depthAttachment.format = findDepthFormat();
				depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
				depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				VkAttachmentReference colorAttachmentRef{};
				colorAttachmentRef.attachment = 0;
				colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				VkAttachmentReference depthAttachmentRef{};
				depthAttachmentRef.attachment = 1;
				depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				VkSubpassDescription subpass{};
				subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
				subpass.colorAttachmentCount = 1;
				subpass.pColorAttachments = &colorAttachmentRef;
				subpass.pDepthStencilAttachment = &depthAttachmentRef;

				std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
				VkRenderPassCreateInfo renderPassInfo{};
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
				renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
				renderPassInfo.pAttachments = attachments.data();
				renderPassInfo.subpassCount = 1;
				renderPassInfo.pSubpasses = &subpass;

				VkSubpassDependency dependency{};
				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				dependency.srcAccessMask = 0;
				dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

				renderPassInfo.dependencyCount = 1;
				renderPassInfo.pDependencies = &dependency;
				if(vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_renderPass) != VK_SUCCESS)
					throw std::runtime_error("failed to create render pass!");
			}	
			void createFramebuffers() {
				_swapChainFramebuffers.resize(_swapChainImageViews.size());
				for (int i = 0; i < _swapChainFramebuffers.size(); i++) {
					std::array<VkImageView, 2> attachments = {
						_swapChainImageViews[i],
						_depthImageView
					};
					VkFramebufferCreateInfo framebufferInfo{};
					framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
					framebufferInfo.renderPass = _renderPass;
					framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
					framebufferInfo.pAttachments = attachments.data();
					framebufferInfo.width = _swapChainExtent.width;
					framebufferInfo.height = _swapChainExtent.height;
					framebufferInfo.layers = 1;
					if(vkCreateFramebuffer(_device, &framebufferInfo, nullptr, &_swapChainFramebuffers[i]) != VK_SUCCESS)
						throw std::runtime_error("failed to create framebuffer!");
				}

			}
			void createCommandPool() {
				QueueFamilyIndices queueFamilyIndices = findQueueFamilies(_physicalDevice);
				VkCommandPoolCreateInfo poolInfo{};
				poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
				poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
				if(vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool) != VK_SUCCESS)
					throw std::runtime_error("failed to create command pool!");
			}
			void createCommandBuffers() {
				_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
				VkCommandBufferAllocateInfo allocInfo{};
				allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocInfo.commandPool = _commandPool;
				allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;
				if(vkAllocateCommandBuffers(_device, &allocInfo, _commandBuffers.data()) != VK_SUCCESS )
					throw std::runtime_error("failed to allocate command buffers!");				
			}

			void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
				VkCommandBufferBeginInfo beginInfo{};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags = 0;
				beginInfo.pInheritanceInfo = nullptr;

				if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
					throw std::runtime_error("failed to begin recording command buffer!");
				}


				VkRenderPassBeginInfo renderPassInfo{};
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassInfo.renderPass = _renderPass;
				renderPassInfo.framebuffer = _swapChainFramebuffers[imageIndex];
				renderPassInfo.renderArea.offset = { 0, 0 };
				renderPassInfo.renderArea.extent = _swapChainExtent;
				std::array<VkClearValue, 2> clearValues{};
				clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
				clearValues[1].depthStencil = { 1.0f, 0 };

				renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
				renderPassInfo.pClearValues = clearValues.data();
				vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);
				VkBuffer vertexBuffers[] = { _vertexBuffer };
				VkDeviceSize offsets[] = { 0 };
				vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
				vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT16);

				VkViewport viewport{};
				viewport.x = 0.0f;
				viewport.y = 0.0f;
				viewport.width = static_cast<float>(_swapChainExtent.width);
				viewport.height = static_cast<float>(_swapChainExtent.height);
				viewport.minDepth = 0.0f;
				viewport.maxDepth = 1.0f;
				vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

				VkRect2D scissor{};
				scissor.offset = { 0, 0 };
				scissor.extent = _swapChainExtent;
				vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

				//vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1, &_descriptorSets[currentFrame], 0, nullptr);
				vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
				vkCmdEndRenderPass(commandBuffer);
				if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
					throw std::runtime_error("failed to record command buffer!");
				}
			}

			void drawFrame() {
				vkWaitForFences(_device, 1, &_inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

				uint32_t imageIndex;
				VkResult result = vkAcquireNextImageKHR(_device, _swapChain, UINT64_MAX, _imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

				if (result == VK_ERROR_OUT_OF_DATE_KHR) {
					recreateSwapChain();
					return;
				}
				else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
					throw std::runtime_error("failed to acquire swap chain image!");
				}

				updateUniformBuffer(currentFrame);

				vkResetFences(_device, 1, &_inFlightFences[currentFrame]);

				vkResetCommandBuffer(_commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
				recordCommandBuffer(_commandBuffers[currentFrame], imageIndex);

				VkSubmitInfo submitInfo{};
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

				VkSemaphore waitSemaphores[] = { _imageAvailableSemaphores[currentFrame] };
				VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
				submitInfo.waitSemaphoreCount = 1;
				submitInfo.pWaitSemaphores = waitSemaphores;
				submitInfo.pWaitDstStageMask = waitStages;

				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = &_commandBuffers[currentFrame];

				VkSemaphore signalSemaphores[] = { _renderFinishedSemaphores[currentFrame] };
				submitInfo.signalSemaphoreCount = 1;
				submitInfo.pSignalSemaphores = signalSemaphores;

				if (vkQueueSubmit(_graphicsQueue, 1, &submitInfo, _inFlightFences[currentFrame]) != VK_SUCCESS) {
					throw std::runtime_error("failed to submit draw command buffer!");
				}

				VkPresentInfoKHR presentInfo{};
				presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

				presentInfo.waitSemaphoreCount = 1;
				presentInfo.pWaitSemaphores = signalSemaphores;

				VkSwapchainKHR swapChains[] = { _swapChain };
				presentInfo.swapchainCount = 1;
				presentInfo.pSwapchains = swapChains;

				presentInfo.pImageIndices = &imageIndex;

				result = vkQueuePresentKHR(_presentQueue, &presentInfo);

				if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
					framebufferResized = false;
					recreateSwapChain();
				}
				else if (result != VK_SUCCESS) {
					throw std::runtime_error("failed to present swap chain image!");
				}

				currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
			}
			void createSyncObjects() {
				_imageAvailableSemaphores.resize(2);
				_renderFinishedSemaphores.resize(2);
				_inFlightFences.resize(2);
				VkSemaphoreCreateInfo semaphoreInfo{};
				semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
				VkFenceCreateInfo fenceInfo{};
				fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;//First frame signal
				for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
					if (vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]) != VK_SUCCESS ||
						vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]) != VK_SUCCESS ||
						vkCreateFence(_device, &fenceInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS) {

						throw std::runtime_error("failed to create synchronization objects for a frame!");
					}
				}
			}
			void cleanUpSwapChain() {
				vkDestroyImageView(_device, _depthImageView, nullptr);
				vkDestroyImage(_device, _depthImage, nullptr);
				vkFreeMemory(_device, _depthImageMemory, nullptr);
				for (size_t i = 0; i < _swapChainFramebuffers.size(); i++) {
					vkDestroyFramebuffer(_device, _swapChainFramebuffers[i], nullptr);
				}

				for (size_t i = 0; i < _swapChainImageViews.size(); i++) {
					vkDestroyImageView(_device, _swapChainImageViews[i], nullptr);
				}

				vkDestroySwapchainKHR(_device, _swapChain, nullptr);

			}
			void recreateSwapChain() {
				int width = 0, height = 0;
				while (width == 0 || height == 0) {
					glfwGetFramebufferSize(_window, &width, &height);
					glfwWaitEvents();
				}

				vkDeviceWaitIdle(_device);

				cleanUpSwapChain();

				createSwapChain();
				createImageViews();
				createDepthResources();
				createFramebuffers();
			}
			uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {

				VkPhysicalDeviceMemoryProperties memProperties;
				vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memProperties);
				for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
					if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
						return i;
					}
				}

				throw std::runtime_error("failed to find suitable memory type!");
			}

			void createBuffer(VkDeviceSize buffersize, VkBufferUsageFlags bufferusageflags, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
				VkBufferCreateInfo bufferInfo{};
				bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				bufferInfo.size = buffersize;
				bufferInfo.usage = bufferusageflags;
				bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
				if (vkCreateBuffer(_device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
					throw std::runtime_error("failed to create vertex Buffer!");
				}
				VkMemoryRequirements memRequirements;
				vkGetBufferMemoryRequirements(_device, buffer, &memRequirements);
				VkMemoryAllocateInfo allocInfo{};
				allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocInfo.allocationSize = memRequirements.size;
				allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
				if (vkAllocateMemory(_device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
					throw std::runtime_error("failed to alloc vertex Buffer Memory!");
				}
				vkBindBufferMemory(_device, buffer, bufferMemory, 0);
			}
			void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {

				VkCommandBuffer commandBuffer = beginSingleTimeCommands();

				VkBufferCopy copyRegion{};
				copyRegion.srcOffset = 0; // Optional
				copyRegion.dstOffset = 0; // Optional
				copyRegion.size = size;
				vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

				endSingleTimeCommands(commandBuffer);
			}
			void createVertexBuffer() {
				VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
				VkBuffer stagingBuffer;
				VkDeviceMemory stagingBufferMemory;

				createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer
					, stagingBufferMemory);
				void* data;
				vkMapMemory(_device, stagingBufferMemory, 0, bufferSize, 0, &data);
				memcpy(data, vertices.data(), (size_t)bufferSize);
				vkUnmapMemory(_device, stagingBufferMemory);

			
				createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _vertexBuffer
					, _vertexBufferMemory);
				copyBuffer(stagingBuffer, _vertexBuffer, bufferSize);

				vkDestroyBuffer(_device, stagingBuffer, nullptr);
				vkFreeMemory(_device, stagingBufferMemory, nullptr);
			}
			void createIndexBuffer() {
				VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
				VkBuffer stagingBuffer;
				VkDeviceMemory stagingBufferMemory;

				createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer
					, stagingBufferMemory);
				void* data;
				vkMapMemory(_device, stagingBufferMemory, 0, bufferSize, 0, &data);
				memcpy(data, indices.data(), (size_t)bufferSize);
				vkUnmapMemory(_device, stagingBufferMemory);


				createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _indexBuffer
					, _indexBufferMemory);
				copyBuffer(stagingBuffer, _indexBuffer, bufferSize);

				vkDestroyBuffer(_device, stagingBuffer, nullptr);
				vkFreeMemory(_device, stagingBufferMemory, nullptr);
			}

			void createUniformBuffers() {
				VkDeviceSize bufferSize = sizeof(UniformBufferObject);

				_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
				_uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
				for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
					createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _uniformBuffers[i], _uniformBuffersMemory[i]);


				}
			}

			void createDescriptorSetLayout() {
				VkDescriptorSetLayoutBinding uboLayoutBinding{};
				uboLayoutBinding.binding = 0;
				uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				uboLayoutBinding.descriptorCount = 1;
				uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
				uboLayoutBinding.pImmutableSamplers = nullptr;

				VkDescriptorSetLayoutBinding samplerLayoutBinding{};
				samplerLayoutBinding.binding = 1;
				samplerLayoutBinding.descriptorCount = 1;
				samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				samplerLayoutBinding.pImmutableSamplers = nullptr;
				samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

				std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
				VkDescriptorSetLayoutCreateInfo layoutInfo{};
				layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
				layoutInfo.pBindings = bindings.data();

				if (vkCreateDescriptorSetLayout(_device, &layoutInfo, nullptr, &_descriptorSetLayout)) {
					throw std::runtime_error("failed to create descriptor set layout!");
				}

			}

			void updateUniformBuffer(uint32_t currentImage) {
				static auto startTime = std::chrono::high_resolution_clock::now();

				auto currentTime = std::chrono::high_resolution_clock::now();
				float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

				UniformBufferObject ubo{};
				ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				ubo.proj = glm::perspective(glm::radians(45.0f), _swapChainExtent.width / (float)_swapChainExtent.height, 0.1f, 10.0f);
				ubo.proj[1][1] *= -1; //glm for opengl ,vk's y is inversed
				size_t bufferSize = sizeof(ubo);
				void* data;
				vkMapMemory(_device, _uniformBuffersMemory[currentImage], 0, bufferSize, 0, &data);
				memcpy(data, &ubo, sizeof(ubo));
				vkUnmapMemory(_device, _uniformBuffersMemory[currentImage]);

			}

			void createDescriptorPool() {
				std::array<VkDescriptorPoolSize, 2> poolSizes{};
				poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
				poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

				VkDescriptorPoolCreateInfo poolInfo{};
				poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
				poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
				poolInfo.pPoolSizes = poolSizes.data();
				poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

				if (vkCreateDescriptorPool(_device, &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS) {
					throw std::runtime_error("failed to create descriptor pool!");
				}

			}

			void createDescriptorSets() {
				std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, _descriptorSetLayout);
				VkDescriptorSetAllocateInfo allocInfo{};
				allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);;
				allocInfo.pSetLayouts = layouts.data();;
				allocInfo.descriptorPool = _descriptorPool;
				_descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

				if (vkAllocateDescriptorSets(_device, &allocInfo, _descriptorSets.data()) != VK_SUCCESS) {
					throw std::runtime_error("failed to allocate descriptor sets!");
				}

				for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
					VkDescriptorBufferInfo bufferInfo{};
					bufferInfo.buffer = _uniformBuffers[i];
					bufferInfo.offset = 0;
					bufferInfo.range = sizeof(UniformBufferObject);

					VkDescriptorImageInfo imageInfo{};
					imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					imageInfo.imageView = _textureImageView;
					imageInfo.sampler =_textureSampler;

					std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
					descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[0].dstSet = _descriptorSets[i];
					descriptorWrites[0].dstBinding = 0;
					descriptorWrites[0].dstArrayElement = 0;
					descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					descriptorWrites[0].descriptorCount = 1;
					descriptorWrites[0].pBufferInfo = &bufferInfo;

					descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[1].dstSet = _descriptorSets[i];
					descriptorWrites[1].dstBinding = 1;
					descriptorWrites[1].dstArrayElement = 0;
					descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					descriptorWrites[1].descriptorCount = 1;
					descriptorWrites[1].pImageInfo = &imageInfo;
					vkUpdateDescriptorSets(_device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
				}

				
			}

			void createTextureImage() {
				int texWidth, texHeight, texChannels;
				stbi_uc* pixels = stbi_load("Resources/Images/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
				VkDeviceSize imageSize = texWidth * texHeight * 4;

				if (pixels == nullptr) {
					throw std::runtime_error("failed to load texture image!");
				}

				VkBuffer stagingBuffer;
				VkDeviceMemory stagingBufferMemory;
				createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
					, stagingBuffer, stagingBufferMemory);

				void* data;
				vkMapMemory(_device, stagingBufferMemory, 0, imageSize, 0, &data);
				memcpy(data, pixels, static_cast<size_t>(imageSize));
				vkUnmapMemory(_device, stagingBufferMemory);

				createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _textureImage, _textureImageMemory);
				stbi_image_free(pixels);

				transitionImageLayout(_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
				copyBufferToImage(stagingBuffer, _textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
				transitionImageLayout(_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

				vkDestroyBuffer(_device, stagingBuffer, nullptr);
				vkFreeMemory(_device, stagingBufferMemory, nullptr);
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

				if (vkCreateImage(_device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
					throw std::runtime_error("failed to create image!");
				}

				VkMemoryRequirements memRequirements;
				vkGetImageMemoryRequirements(_device, image, &memRequirements);

				VkMemoryAllocateInfo allocInfo{};
				allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocInfo.allocationSize = memRequirements.size;
				allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

				if (vkAllocateMemory(_device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
					throw std::runtime_error("failed to allocate image memory!");
				}

				vkBindImageMemory(_device, image, imageMemory, 0);
			}

			VkCommandBuffer beginSingleTimeCommands() {
				VkCommandBufferAllocateInfo allocInfo{};
				allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocInfo.commandPool = _commandPool;
				allocInfo.commandBufferCount = 1;

				VkCommandBuffer commandBuffer;
				vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer);

				VkCommandBufferBeginInfo beginInfo{};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

				vkBeginCommandBuffer(commandBuffer, &beginInfo);

				return commandBuffer;
			}

			void endSingleTimeCommands(VkCommandBuffer commandBuffer) {
				vkEndCommandBuffer(commandBuffer);

				VkSubmitInfo submitInfo{};
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = &commandBuffer;

				vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
				vkQueueWaitIdle(_graphicsQueue);

				vkFreeCommandBuffers(_device, _commandPool, 1, &commandBuffer);
			}

			void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
				VkCommandBuffer commandBuffer = beginSingleTimeCommands();
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
				endSingleTimeCommands(commandBuffer);
			}

			void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
				VkCommandBuffer commandBuffer = beginSingleTimeCommands();
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
				endSingleTimeCommands(commandBuffer);
			}

			void createTextureImageView() {
			    _textureImageView = createImageView(_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);



			}

			VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
				VkImageViewCreateInfo viewInfo{};
				viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				viewInfo.image = image;
				viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				viewInfo.format = format;
				viewInfo.subresourceRange.aspectMask = aspectFlags;
				viewInfo.subresourceRange.baseMipLevel = 0;
				viewInfo.subresourceRange.levelCount = 1;
				viewInfo.subresourceRange.baseArrayLayer = 0;
				viewInfo.subresourceRange.layerCount = 1;

				VkImageView imageView;
				if (vkCreateImageView(_device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
					throw std::runtime_error("failed to create texture image view!");
				}

				return imageView;
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
				vkGetPhysicalDeviceProperties(_physicalDevice, &properties);
				samplerInfo.maxAnisotropy = 1.0F;
				samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
				samplerInfo.unnormalizedCoordinates = VK_FALSE;
				samplerInfo.compareEnable = VK_FALSE;
				samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
				samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
				samplerInfo.mipLodBias = 0.0f;
				samplerInfo.minLod = 0.0f;
				samplerInfo.maxLod = 0.0f;
				if (vkCreateSampler(_device, &samplerInfo, nullptr, &_textureSampler) != VK_SUCCESS) {
					throw std::runtime_error("failed to create texture sampler!");
				}
			}

			void createDepthResources() {
				VkFormat depthFormat = findDepthFormat();
				createImage(_swapChainExtent.width, _swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _depthImage, _depthImageMemory);
				_depthImageView = createImageView(_depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
			}

			VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
				for (VkFormat format : candidates) {
					VkFormatProperties props;
					vkGetPhysicalDeviceFormatProperties(_physicalDevice, format, &props);

					if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
						return format;
					}
					else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
						return format;
					}
				}

				throw std::runtime_error("failed to find supported format!");
			}

			VkFormat findDepthFormat() {
				return findSupportedFormat(
					{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
					VK_IMAGE_TILING_OPTIMAL,
					VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
				);
			}

			bool hasStencilComponent(VkFormat format) {
				return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
			}
		private:
			uint32_t currentFrame = 0;
			GLFWwindow* _window;
			const uint32_t WIDTH = 800;
			const uint32_t HEIGHT = 600;
			VkInstance _instance;
			VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE; // will destroy with instance
			VkDevice _device;
			VkSurfaceKHR _surface;
			VkSwapchainKHR _swapChain;
			std::vector<VkImage> _swapChainImages;
			std::vector<VkImageView> _swapChainImageViews;
			VkFormat _swapChainImageFormat;
			VkExtent2D _swapChainExtent;
			VkRenderPass _renderPass;
			VkDescriptorSetLayout _descriptorSetLayout;
			VkPipelineLayout _pipelineLayout;
			VkPipeline _graphicsPipeline;
			std::vector<VkFramebuffer> _swapChainFramebuffers;
			const std::vector<const char*> _validationLayers = {
				"VK_LAYER_KHRONOS_validation"
			};
			const std::vector<const char*> _deviceExtensions = {
				VK_KHR_SWAPCHAIN_EXTENSION_NAME
			};
			std::optional<uint32_t> _GraphiscFamily;
			VkQueue _graphicsQueue;
			VkQueue _presentQueue;
			VkCommandPool _commandPool;
			VkBuffer _vertexBuffer;
			VkDeviceMemory _vertexBufferMemory;
			VkBuffer _indexBuffer;
			VkDeviceMemory _indexBufferMemory;
			VkDescriptorPool _descriptorPool;
			std::vector<VkDescriptorSet> _descriptorSets;
			std::vector<VkBuffer> _uniformBuffers;
			std::vector<VkDeviceMemory> _uniformBuffersMemory;
			//VkCommandBuffer _commandBuffer;
			//VkSemaphore _imageAvailableSemaphore;
			//VkSemaphore _renderFinishedSemaphore;
			//VkFence _inFlightFence;
			std::vector<VkCommandBuffer> _commandBuffers;
			std::vector<VkSemaphore> _imageAvailableSemaphores;
			std::vector<VkSemaphore> _renderFinishedSemaphores;
			std::vector<VkFence> _inFlightFences;


			VkImage _textureImage;
			VkImageView _textureImageView;
			VkDeviceMemory _textureImageMemory;

			VkImage _depthImage;
			VkImageView _depthImageView;
			VkDeviceMemory _depthImageMemory;
			VkSampler _textureSampler;
			bool framebufferResized = false;
#ifdef NDEBUG
			const bool _enableValidationLayers = false;
#else
			const bool _enableValidationLayers = true;
#endif // NDEBUG
			VkDebugUtilsMessengerEXT _debugMessenger;

			std::shared_ptr<Graphics::GraphicsInstance> graphicsInstance;
			std::shared_ptr<Graphics::GraphicsDevice> graphicsDevice;
			std::shared_ptr<Graphics::GraphicsSwapChain> graphicsSwapChain;
	};
}


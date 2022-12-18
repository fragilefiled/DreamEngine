#pragma once
#pragma once
#include <vulkan/vulkan.h>
#include"GraphicsUtil.hpp"
#include "GraphicsInstance.hpp"
namespace Graphics
{
	class GraphicsDevice {

	public:
		GraphicsDevice(std::shared_ptr<GraphicsInstance> graphicsInstance) {
			m_graphicsInstance = graphicsInstance;
			pickPhysicDevice();
			createLogicDevice();
		}
		void pickPhysicDevice() 
		{
			auto graphicsHelper = GraphicsUtil::getInstance();
			uint32_t deviceCount = 0;
			m_vkInstance = m_graphicsInstance->getInstance();
			m_vkSurface = m_graphicsInstance->getSurface();
			vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, nullptr);

			if (deviceCount == 0)
				throw std::runtime_error("failed to find GPUs with Vulkan support!");
			else {
				std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
				vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, physicalDevices.data());
				for (auto device : physicalDevices)
				{
					if (graphicsHelper->isDeviceSuitable(device, m_vkSurface)) {
						m_physicalDevice = device;
						break;
					}
				}
			}
			if (m_physicalDevice == VK_NULL_HANDLE)
				throw std::runtime_error("failed to find suitable GPU!");
		}
		void createLogicDevice() {
			auto graphicsHelper = GraphicsUtil::getInstance();
			GraphicsUtil::QueueFamilyIndices indices = graphicsHelper->findQueueFamilies(m_physicalDevice, m_vkSurface);

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

			VkPhysicalDeviceFeatures deviceFeatures{}; //Like Gemetory Shader啥的

			VkDeviceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.pEnabledFeatures = &deviceFeatures;
			createInfo.pQueueCreateInfos = queueCreateInfos.data();
			createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

			createInfo.enabledExtensionCount = static_cast<uint32_t>(graphicsHelper->_deviceExtensions.size());
			createInfo.ppEnabledExtensionNames = graphicsHelper->_deviceExtensions.data();

			if (m_graphicsInstance->getValidationLayersEnable()) {
				createInfo.enabledLayerCount = static_cast<uint32_t>(graphicsHelper->_validationLayers.size());
				createInfo.ppEnabledLayerNames = graphicsHelper->_validationLayers.data();
			}
			else {
				createInfo.enabledLayerCount = 0;
			}//will be ignored by up-to-date implementations.

			if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
				throw std::runtime_error("failed to create logical device!");

			vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue); //创建队列
			vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue); //创建队列
			// logical device, queue family, queue index and a pointer to the variable to store the queue handle 
		}
		~GraphicsDevice() {
			m_graphicsInstance.reset();
			vkDestroyDevice(m_device, nullptr);
		}

		VkDevice getLogicDevice() {
			return  m_device;
		}

		VkPhysicalDevice getPhysicDevice() {
			return  m_physicalDevice;
		}

		VkQueue getGraphicsQueue() {
			return m_graphicsQueue;
		}

		VkQueue getPresentQueue() {
			return m_presentQueue;
		}

		std::shared_ptr<GraphicsInstance> getGraphicsInstance() 
		{
			return m_graphicsInstance;
		}
		
	private:
		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		std::shared_ptr<GraphicsInstance> m_graphicsInstance;
		VkQueue m_graphicsQueue;
		VkQueue m_presentQueue;
		VkDevice m_device;
		VkSurfaceKHR m_vkSurface;
		VkInstance m_vkInstance;
	};



}
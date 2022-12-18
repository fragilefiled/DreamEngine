#pragma once
#include <vulkan/vulkan.h>
#include"GraphicsUtil.hpp"
namespace Graphics
{
	class GraphicsInstance
	{
		public:
			GraphicsInstance(bool enableValidationLayers, GLFWwindow* window) {
				this->m_window = window;
				this->m_enableValidationLayers = enableValidationLayers;

				auto graphicsHelper = GraphicsUtil::getInstance();
				if (enableValidationLayers && !graphicsHelper->checkValidationLayerSupport()) {
					throw std::runtime_error("validation layers requested , but not available");
				}

				VkApplicationInfo appInfo{};
				appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
				appInfo.pApplicationName = "Dream Engine Start";
				appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
				appInfo.pEngineName = "NO Engine";
				appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
				appInfo.apiVersion = VK_API_VERSION_1_3;// 升级这个不知道为什么有用Desu

				VkInstanceCreateInfo createInfo{}; //指定全局扩展和验证层
				createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
				createInfo.pApplicationInfo = &appInfo;

				auto extensions = graphicsHelper->getRequiredExtensions(enableValidationLayers);
				createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
				createInfo.ppEnabledExtensionNames = extensions.data();

				VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
				if (enableValidationLayers) {
					createInfo.ppEnabledLayerNames =graphicsHelper->_validationLayers.data();//create info include the validation layer names
					createInfo.enabledLayerCount = static_cast<uint32_t>(graphicsHelper->_validationLayers.size());;
					populateDebugMessengerCreateInfo(debugCreateInfo);
					createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
				}
				else {
					createInfo.enabledLayerCount = 0;

					createInfo.pNext = nullptr;
				}

				//uint32_t glfwExtensionCount = 0;
				//const char** glfwExtensions;

				//glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

				//createInfo.enabledExtensionCount = glfwExtensionCount;
				//createInfo.ppEnabledExtensionNames = glfwExtensions;
				//createInfo.enabledLayerCount = 0;

				if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
					throw std::runtime_error("failed to create instance!");

				setUpDebugMessenger();
				createSurface();
			}

			VkInstance getInstance() 
			{
				if (m_instance != VK_NULL_HANDLE)
					return m_instance;
				else 
				{
					throw std::runtime_error("Graphics Instance not exist!");
					return nullptr;
				}

			}
			VkSurfaceKHR getSurface()
			{
				if (m_surface != VK_NULL_HANDLE)
					return m_surface;
				else
				{
					throw std::runtime_error("Graphics Instance not exist!");
					return nullptr;
				}

			}

			GLFWwindow* getGLTFWindow()
			{
				return m_window;
			}
			bool getValidationLayersEnable()
			{
				return m_enableValidationLayers;
			}

			void cleanUp() 
			{
				vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
				if (m_enableValidationLayers)
					DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
				vkDestroyInstance(m_instance, nullptr);
			}
			~GraphicsInstance() {
				cleanUp();
				GraphicsUtil::release();
			}

			
	private:
			bool m_enableValidationLayers = true;
			void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
				createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
				createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
				createInfo.pfnUserCallback = debugCallback; //函数指针
			}
			static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
				VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				VkDebugUtilsMessageTypeFlagsEXT messageType,
				const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
				void* pUserData
			)//第一个变量返回错误的严重性  
			{
				std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

				return VK_FALSE;

			}

			VkResult  CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo
				, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
				auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
				if (func != nullptr)
					return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
				else
					return VK_ERROR_EXTENSION_NOT_PRESENT;

			}
			void  DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT DebugMessenger, const VkAllocationCallbacks* pAllocator) {
				auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
				if (func != nullptr)
					func(instance, DebugMessenger, pAllocator);
			}
			void setUpDebugMessenger()
			{
				if (!m_enableValidationLayers)
					return;
				else
				{
					VkDebugUtilsMessengerCreateInfoEXT createInfo{};
					populateDebugMessengerCreateInfo(createInfo);
					if (CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS) {
						throw std::runtime_error("failed to set up debug messenger!");
					}

				}

			}
			bool createSurface() {
				if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS)
					throw std::runtime_error("failed to create window surface!");
			}
			VkInstance m_instance;
			GLFWwindow* m_window;
			VkSurfaceKHR m_surface;
			VkDebugUtilsMessengerEXT m_debugMessenger;

	};




}
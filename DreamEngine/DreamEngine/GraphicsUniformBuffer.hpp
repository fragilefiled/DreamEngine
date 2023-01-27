#pragma once
#pragma once
#include"GraphicsRenderPass.hpp"
#include"GraphicsTexture.hpp"
namespace Graphics {
	template<typename UniformBufferObject>
	class GraphicsUniformBuffer
	{
	public:
		GraphicsUniformBuffer() {
			m_device = GraphicsDevice::getInstance()->getLogicDevice();
			m_physicalDevice = GraphicsDevice::getInstance()->getPhysicDevice();
			createUniformBuffers();

		};

		~GraphicsUniformBuffer() {
			auto graphicsHelper = GraphicsUtil::getInstance();
			for (size_t i = 0; i < graphicsHelper->MAX_FRAMES_IN_FLIGHT; i++) {
				vkDestroyBuffer(m_device, m_uniformBuffers[i], nullptr);
				vkFreeMemory(m_device, m_uniformBuffersMemory[i], nullptr);
			}
		}

		void createUniformBuffers() {
			auto graphicsHelper = GraphicsUtil::getInstance();
			VkDeviceSize bufferSize = sizeof(UniformBufferObject);

			m_uniformBuffers.resize(graphicsHelper->MAX_FRAMES_IN_FLIGHT);
			m_uniformBuffersMemory.resize(graphicsHelper->MAX_FRAMES_IN_FLIGHT);
			for (size_t i = 0; i < graphicsHelper->MAX_FRAMES_IN_FLIGHT; i++) {
				graphicsHelper->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_uniformBuffers[i], m_uniformBuffersMemory[i], m_device, m_physicalDevice);

			}
		}

		std::vector<VkBuffer> getUniformBuffers() {
			return m_uniformBuffers;
		}

		std::vector<VkDeviceMemory> getUniformBuffersMemory() {
			return m_uniformBuffersMemory;
		}

		uint16_t getUBOSize() {
			return sizeof(UniformBufferObject);
		}

		void updateUniformBuffer(uint32_t currentImage, UniformBufferObject ubo) {
			size_t bufferSize = sizeof(ubo);
			void* data;
			vkMapMemory(m_device, m_uniformBuffersMemory[currentImage], 0, bufferSize, 0, &data);
			memcpy(data, &ubo, sizeof(ubo));
			vkUnmapMemory(m_device, m_uniformBuffersMemory[currentImage]);

		}

	private:
		std::vector<VkBuffer> m_uniformBuffers;
		std::vector<VkDeviceMemory> m_uniformBuffersMemory;
		VkDevice m_device;
		VkPhysicalDevice m_physicalDevice;

	};
}
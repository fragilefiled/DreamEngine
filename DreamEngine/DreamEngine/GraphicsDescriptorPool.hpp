#pragma once
#pragma once
#include"GraphicsRenderPass.hpp"
#include"GraphicsTexture.hpp"
namespace Graphics {
	class GraphicsDescriptorPool
	{
	public:
		GraphicsDescriptorPool() {
			m_device = GraphicsDevice::getInstance()->getLogicDevice();
			createDescriptorPool();
		};

		~GraphicsDescriptorPool() {
			vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
		}


		void createDescriptorPool() {
			auto graphicsHelper = GraphicsUtil::getInstance();
			int MAX_FRAMES_IN_FLIGHT = graphicsHelper->MAX_FRAMES_IN_FLIGHT;
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

			if (vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
				throw std::runtime_error("failed to create descriptor pool!");
			}

		}

		VkDescriptorPool getDescriptorPool() {
			return m_descriptorPool;
		}

	private:
		VkDescriptorPool m_descriptorPool;
		VkDevice m_device;
	};
}
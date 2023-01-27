#pragma once
#pragma once
#pragma once
#include"GraphicsRenderPass.hpp"
#include"GraphicsTexture.hpp"
#include "GraphicsDescriptorPool.hpp"
#include "GraphicsUniformBuffer.hpp"
#include "GraphicsDescriptorSetLayout.hpp"
namespace Graphics {
	class GraphicsDescriptorSet
	{
	public:
		GraphicsDescriptorSet(std::shared_ptr<GraphicsDescriptorSetLayout> graphicsDescriptorSetLayout, std::shared_ptr<GraphicsDescriptorPool> graphicsDescriptorPool
			,std::shared_ptr<GraphicsTexture> graphicsTexture, std::vector<VkBuffer> uniformBuffers, uint16_t uboSize) {
			m_graphicsDescriptorSetLayout = graphicsDescriptorSetLayout;
			m_graphicsDescriptorPool = graphicsDescriptorPool;
			m_graphicsTexture = graphicsTexture;
			m_device = GraphicsDevice::getInstance()->getLogicDevice();

			m_descriptorPool = m_graphicsDescriptorPool->getDescriptorPool();
			m_descriptorSetLayout = m_graphicsDescriptorSetLayout->GetDescriptSetLayout();
			m_uniformBuffers = uniformBuffers;
			m_uboSize = uboSize;
			createDescriptorSets();
		};

		~GraphicsDescriptorSet() {
			m_graphicsDescriptorSetLayout.reset();
			m_graphicsDescriptorPool.reset();
			m_graphicsTexture.reset();
		}


		void createDescriptorSets() {
			auto graphicsHelper = GraphicsUtil::getInstance();
			int MAX_FRAMES_IN_FLIGHT = graphicsHelper->MAX_FRAMES_IN_FLIGHT;
			std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_descriptorSetLayout);
			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);;
			allocInfo.pSetLayouts = layouts.data();;
			allocInfo.descriptorPool = m_descriptorPool;
			m_descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

			if (vkAllocateDescriptorSets(m_device, &allocInfo, m_descriptorSets.data()) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate descriptor sets!");
			}

			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
				VkDescriptorBufferInfo bufferInfo{};
				bufferInfo.buffer = m_uniformBuffers[i];
				bufferInfo.offset = 0;
				bufferInfo.range = m_uboSize;

				VkDescriptorImageInfo imageInfo{};
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = m_graphicsTexture->getImageView();
				imageInfo.sampler = m_graphicsTexture->getSampler();

				std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
				descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[0].dstSet = m_descriptorSets[i];
				descriptorWrites[0].dstBinding = 0;
				descriptorWrites[0].dstArrayElement = 0;
				descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrites[0].descriptorCount = 1;
				descriptorWrites[0].pBufferInfo = &bufferInfo;

				descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[1].dstSet = m_descriptorSets[i];
				descriptorWrites[1].dstBinding = 1;
				descriptorWrites[1].dstArrayElement = 0;
				descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrites[1].descriptorCount = 1;
				descriptorWrites[1].pImageInfo = &imageInfo;
				vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
			}


		}

		std::vector<VkDescriptorSet> getDescriptorSets() {
			return m_descriptorSets;
		}


	private:
		VkDescriptorPool m_descriptorPool;
		VkDevice m_device;
		VkDescriptorSetLayout m_descriptorSetLayout;
		std::vector<VkDescriptorSet> m_descriptorSets;
		std::shared_ptr<GraphicsDescriptorPool> m_graphicsDescriptorPool;
		std::vector<VkBuffer> m_uniformBuffers;
		std::shared_ptr<GraphicsTexture> m_graphicsTexture;
		std::shared_ptr<GraphicsDescriptorSetLayout> m_graphicsDescriptorSetLayout;

		uint16_t m_uboSize;
	};
}
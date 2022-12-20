#pragma once
#pragma once
#include <vulkan/vulkan.h>
#include"GraphicsRenderPass.hpp"
#include "FileOperation.h"
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#endif
namespace Graphics
{
	class GraphicsDescriptorSetLayout
	{

	public:
		GraphicsDescriptorSetLayout()
		{
			m_device = GraphicsDevice::getInstance()->getLogicDevice();
			createDescriptorSetLayout();
		};
		void createDescriptorSetLayout()
		{
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

			if (vkCreateDescriptorSetLayout(m_device, &layoutInfo, nullptr, &m_descriptorSetLayout)) {
				throw std::runtime_error("failed to create descriptor set layout!");
			}

		}
		VkDescriptorSetLayout GetDescriptSetLayout() {
			return m_descriptorSetLayout;
		}
		~GraphicsDescriptorSetLayout() {
			vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayout, nullptr);;
		}
	private:
		VkDevice m_device;
		VkDescriptorSetLayout m_descriptorSetLayout;

	};

}